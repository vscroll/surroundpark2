#ifndef CAPTUREIMPL_H
#define CAPTUREIMPL_H

#include <QObject>
#include <QTimer>
#include <QThread>

#include "ICapture.h"
#include "captureworkerbase.h"

class CaptureWorker;
class CaptureImpl : public QObject, public ICapture
{
    Q_OBJECT
public:
    explicit CaptureImpl(QObject *parent = 0);
    virtual ~CaptureImpl();

    virtual int openDevice();
    virtual int closeDevice();
    virtual int start(VIDEO_FPS fps);
    virtual int stop();
    virtual surround_images_t* popOneFrame();
signals:

public slots:

private:
    QTimer mVideoCaptureTimer;
    CaptureWorkerBase *mCaptureWorker;
    QThread mCaptureThread;
    VIDEO_FPS mFPS;
};

#endif // CAPTUREIMPL_H
