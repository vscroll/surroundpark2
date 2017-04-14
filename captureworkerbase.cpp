#include "captureworkerbase.h"

CaptureWorkerBase::CaptureWorkerBase(QObject *parent, int videoChannelNum) :
    QObject(parent),
    mVideoChannelNum(videoChannelNum < VIDEO_CHANNEL_SIZE ? videoChannelNum : VIDEO_CHANNEL_SIZE)
{
    mDropFrameCount = 10;
    mLastTimestamp = 0.0;
}

int CaptureWorkerBase::openDevice()
{
    return -1;
}

void CaptureWorkerBase::closeDevice()
{

}

void CaptureWorkerBase::onCapture()
{

}

surround_images_t* CaptureWorkerBase::popOneFrame()
{

    struct surround_images_t* surroundImage = NULL;
    {
        QMutexLocker locker(&mMutexQueue);
        if (mSurroundImageQueue.size() > 0)
        {
            surroundImage = mSurroundImageQueue.dequeue();
        }
    }

    return surroundImage;
}

int CaptureWorkerBase::getFrameCount()
{
    QMutexLocker locker(&mMutexQueue);
    return mSurroundImageQueue.size();
}
