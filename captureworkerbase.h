#ifndef CAPTUREWORKERBASE_H
#define CAPTUREWORKERBASE_H

#include <QObject>
#include <QQueue>
#include <QMutex>

#include "common.h"

class CaptureWorkerBase : public QObject
{
    Q_OBJECT
public:
    explicit CaptureWorkerBase(QObject *parent = 0, int videoChannelNum = 4);

    virtual int openDevice();
    virtual void closeDevice();
    virtual surround_images_t* popOneFrame();
    virtual int getFrameCount();
    virtual int getVideoChannelNum() { return mVideoChannelNum; }
signals:

public slots:
    virtual void onCapture();

protected:
    int mVideoChannelNum;
    QQueue<surround_images_t*> mSurroundImageQueue;
    QMutex mMutexQueue;

    int mDropFrameCount;
    double mLastTimestamp;
};

#endif // CAPTUREWORKERBASE_H
