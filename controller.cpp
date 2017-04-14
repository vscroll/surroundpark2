#include "controller.h"
#include "ICapture.h"
#include "IStitch.h"
#include "stitchimpl.h"
#include "captureimpl.h"

Controller::Controller() :
    mIsRunning(false),
    mCapture(NULL),
    mStitch(NULL)
{

}

void Controller::init()
{
    if (NULL == mCapture)
    {
        mCapture = new CaptureImpl();
        mCapture->openDevice();
    }

    if (NULL == mStitch)
    {
        mStitch = new StitchImpl();
    }
}

void Controller::uninit()
{
    if (NULL != mCapture)
    {
         delete mCapture;
         mCapture = NULL;
    }

    if (NULL != mStitch)
    {
        delete mStitch;
        mStitch = NULL;
    }
}

void Controller::start(VIDEO_FPS captureFps)
{
   if (NULL != mCapture)
   {
       mCapture->start(captureFps);
   }

   if (NULL != mStitch)
   {
       mStitch->start(mCapture);
   }

   //mIsRunning = true;
   //QThread::start();
}

void Controller::stop()
{
    mIsRunning = false;

    if (NULL != mCapture)
    {
         mCapture->stop();
         mCapture->closeDevice();
    }

    if (NULL != mStitch)
    {
        mStitch->stop();
    }
}

void Controller::run()
{
    while (true)
    {
        if (!mIsRunning)
        {
            break;
        }

        if (NULL == mCapture
                || NULL == mStitch)
        {
            break;
        }

        usleep(5000);
    }
}

surround_image_t* Controller::dequeueFullImage()
{
    surround_image_t* tmp = NULL;
    if (NULL != mStitch)
    {
        tmp = mStitch->dequeueFullImage();
    }
    return tmp;
}

surround_image_t* Controller::dequeueSmallImage(VIDEO_CHANNEL channel)
{
    surround_image_t* tmp = NULL;
    if (NULL != mStitch)
    {
        tmp = mStitch->dequeueSmallImage(channel);
    }
    return tmp;
}
