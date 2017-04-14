#include "captureimpl.h"
#include "captureworkerv4l2impl.h"

CaptureImpl::CaptureImpl(QObject *parent) :
    QObject(parent),
    mCaptureWorker(NULL)
{
}

CaptureImpl::~CaptureImpl()
{

}

int CaptureImpl::openDevice()
{
    if (NULL == mCaptureWorker)
    {
        mCaptureWorker = new CaptureWorkerV4l2Impl(NULL, VIDEO_CHANNEL_SIZE);
        mCaptureWorker->openDevice();
        connect(&mVideoCaptureTimer, SIGNAL(timeout()), mCaptureWorker, SLOT(onCapture()));
    }

    return 0;
}

int CaptureImpl::closeDevice()
{
    if (NULL != mCaptureWorker)
    {
        mCaptureWorker->closeDevice();
        delete mCaptureWorker;
        mCaptureWorker = NULL;
    }

    return 0;
}

int CaptureImpl::start(VIDEO_FPS fps)
{
    mFPS = fps;
    mVideoCaptureTimer.start(1000/mFPS);

    //capture speed up, update speed down
    if (NULL != mCaptureWorker)
    {
        mCaptureWorker->moveToThread(&mCaptureThread);
        mCaptureThread.start(QThread::TimeCriticalPriority);
    }

    return 0;
}

int CaptureImpl::stop()
{
    mVideoCaptureTimer.stop();
    mCaptureThread.quit();

    return 0;
}

surround_images_t* CaptureImpl::popOneFrame()
{
    surround_images_t* pFrame = NULL;
    if (NULL != mCaptureWorker)
    {
        pFrame = mCaptureWorker->popOneFrame();
    }
    return pFrame;
}
