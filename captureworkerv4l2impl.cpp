#include "captureworkerv4l2impl.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "util.h"
#include "settings.h"
#include <QDebug>

CaptureWorkerV4l2Impl::CaptureWorkerV4l2Impl(QObject *parent, int videoChannelNum) :
    CaptureWorkerBase(parent, videoChannelNum)
{
    mInWidth = 704;
    mInHeight = 574;
    mInPixfmt = V4L2_PIX_FMT_UYVY;

    mOutWidth = 704;
    mOutHeight = 574;
    mOutPixfmt = V4L2_PIX_FMT_BGR24;

    for (int i = 0; i < VIDEO_CHANNEL_SIZE; ++i)
    {
        mVideoFd[i] = -1;

        mIPUFd[i] = -1;

        mInIPUBuf[i].width = mInWidth;
        mInIPUBuf[i].height = mInHeight;
        mInIPUBuf[i].pixfmt = mInPixfmt;

        mOutIPUBuf[i].width = mOutWidth;
        mOutIPUBuf[i].height = mOutHeight;
        mOutIPUBuf[i].pixfmt = mOutPixfmt;
    }

    mMemType = V4L2_MEMORY_MMAP;
}

int CaptureWorkerV4l2Impl::openDevice()
{
    for (int i = 0; i < mVideoChannelNum; ++i)
    {
        mIPUFd[i] = open("/dev/mxc_ipu", O_RDWR, 0);
        if (mIPUFd[i] < 0)
        {
            qDebug() << "CaptureWorkerV4l2Impl::openDevice"
                    << " open ipu failed";
            return -1;
        }
        qDebug() << "CaptureWorkerV4l2Impl::openDevice"
                << " ipu fd:" << mIPUFd[i];

        unsigned int in_frame_size = mInIPUBuf[i].width * mInIPUBuf[i].height * 2;
        if (-1 == IMXIPU::allocIpuBuf(mIPUFd[i], &(mInIPUBuf[i]),  in_frame_size))
        {
            return -1;
        }

        unsigned int out_frame_size = mOutIPUBuf[i].width * mOutIPUBuf[i].height * 3;
        if (-1 == IMXIPU::allocIpuBuf(mIPUFd[i], &(mOutIPUBuf[i]),  out_frame_size))
        {
            return -1;
        }

        int video_channel = Settings::getInstant()->mVideoChanel[i];
        char devName[16] = {0};
        sprintf(devName, "/dev/video%d", video_channel);
        mVideoFd[i] = open(devName, O_RDWR | O_NONBLOCK);
        if (mVideoFd[i] < 0)
        {
            qDebug() << "Capture4WorkerV4l2Impl::openDevice"
                    << " open video failed";
            return -1;
        }

        V4l2::getVideoCap(mVideoFd[i]);
        V4l2::getVideoFmt(mVideoFd[i], &mPixfmt[i], &mWidth[i], &mHeight[i]);
        //i don't know why
        V4l2::setVideoFmt(mVideoFd[i], mPixfmt[i], mWidth[i]-2, mHeight[i]-2);
        V4l2::getVideoFmt(mVideoFd[i], &mPixfmt[i], &mWidth[i], &mHeight[i]);
        V4l2::setFps(mVideoFd[i], 15);
        V4l2::getFps(mVideoFd[i]);

#if DEBUG_CAPTURE
        qDebug() << "Capture4WorkerV4l2Impl::openDevice"
                 << "mem type: " << mMemType
                 << "buf count:" << V4L2_BUF_COUNT
                << " width:" << mWidth[i]
                << " height:" << mHeight[i];
#endif

        for (unsigned int j = 0; j < V4L2_BUF_COUNT; ++j)
        {
            mV4l2Buf[i][j].width = mWidth[i];
            mV4l2Buf[i][j].height = mHeight[i];
            mV4l2Buf[i][j].pixfmt = mPixfmt[i];
        }

        unsigned int frame_size = mWidth[i] * mHeight[i] * 2;
        if (-1 == V4l2::initV4l2Buf(mVideoFd[i], mV4l2Buf[i], V4L2_BUF_COUNT, mMemType, frame_size))
        {
            return -1;
        }

#if DEBUG_CAPTURE
        qDebug() << "Capture4WorkerV4l2Impl::openDevice"
                 << "initV4l2Buf ok";
#endif

        if (-1 == V4l2::startCapture(mVideoFd[i], mV4l2Buf[i], mMemType))
        {
            return -1;
        }
    }

#if DEBUG_CAPTURE
        qDebug() << "Capture4WorkerV4l2Impl::openDevice"
                 << "startCapture ok";
#endif

    return 0;
}

void CaptureWorkerV4l2Impl::closeDevice()
{
    for (int i = 0; i < mVideoChannelNum; ++i)
    {
        if (mVideoFd[i] > 0)
        {
            V4l2::stoptCapture(mVideoFd[i]);

            close(mVideoFd[i]);
            mVideoFd[i] = -1;
        }
    }
}

void CaptureWorkerV4l2Impl::onCapture()
{
#if DEBUG_CAPTURE
    double start = (double)clock();
    int size = 0;
    int elapsed = 0;
    if (qAbs(mLastTimestamp) > 0.00001f)
    {
        elapsed = (int)(start - mLastTimestamp)/1000;
    }
    mLastTimestamp = start;
#endif

    void* image[VIDEO_CHANNEL_SIZE] = {NULL};
    unsigned char flag = 1;
    double timestamp = (double)clock();
    for (int i = 0; i < mVideoChannelNum; ++i)
    {
        if (mVideoFd[i] == -1)
        {
            return;
        }

        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(mVideoFd[i], &fds);

        struct timeval tv;
        tv.tv_sec = 2;
        tv.tv_usec = 0;

        int r = select (mVideoFd[i] + 1, &fds, NULL, NULL, &tv);
        if (-1 == r) {
            if (EINTR == errno)
                qDebug() << "Capture1WorkerV4l2Impl::onCapture"
                         << "EINTR";
                return;
        }

        if (0 == r) {
            qDebug() << "Capture1WorkerV4l2Impl::onCapture"
                     << " select timeout";
            return;
        }
#if DEBUG_CAPTURE
        double read_start = (double)clock();
#endif
        struct v4l2_buffer buf;
        if (-1 != V4l2::readFrame(mVideoFd[i], &buf, mMemType))
        {
            if (buf.index < V4L2_BUF_COUNT)
            {
                image[i] = (void *)(new unsigned char[mV4l2Buf[i][buf.index].length]);
                flag = flag << 1;
                memcpy(image[i],  (unsigned char*)(mV4l2Buf[i][buf.index].start), mV4l2Buf[i][buf.index].length);
            }
        }

#if DEBUG_CAPTURE
    qDebug() << "Capture4WorkerV4l2Impl::onCapture"
             << " read frame: " << (int)(clock()-read_start)/1000;
#endif

        V4l2::v4l2QBuf(mVideoFd[i], &buf);
    }

    //integrity
    if (flag == (1 << mVideoChannelNum))
    {
        surround_images_t* surroundImage = new surround_images_t();
        surroundImage->timestamp = timestamp;
        for (int i = 0; i < mVideoChannelNum; ++i)
        {
            surroundImage->frame[i].data = image[i];
            surroundImage->frame[i].width = mWidth[i];
            surroundImage->frame[i].height = mHeight[i];
            surroundImage->frame[i].pixfmt = mPixfmt[i];
        }

        mMutexQueue.lock();
        mSurroundImageQueue.append(surroundImage);
#if DEBUG_CAPTURE
        size = mSurroundImageQueue.size();
#endif
        mMutexQueue.unlock();
    }
    else
    {
        for (int i = 0; i < mVideoChannelNum; ++i)
        {
            if (NULL != image[i])
            {
                delete ((unsigned char*)image[i]);
            }
        }
    }

#if DEBUG_CAPTURE
    qDebug() << "Capture4WorkerV4l2Impl::onCapture"
             << ", channel:" << mVideoChannelNum
             << ", flag:" << flag
             << ", size:" << size
             << ", elapsed to last time:" << elapsed
             << ", capture:" << (int)(clock()-timestamp)/1000;
#endif
}
