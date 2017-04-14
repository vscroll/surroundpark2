#ifndef CAPTUREWORKERV4L2IMPL_H
#define CAPTUREWORKERV4L2IMPL_H

#include "common.h"
#include "captureworkerbase.h"
#include "v4l2.h"

class CaptureWorkerV4l2Impl : public CaptureWorkerBase
{
    Q_OBJECT
public:
    explicit CaptureWorkerV4l2Impl(QObject *parent = 0, int videoChannelNum = 4);

public:
    virtual int openDevice();
    virtual void closeDevice();
signals:

public slots:
    virtual void onCapture();

private:
    int mInWidth;
    int mInHeight;
    int mInPixfmt;
    int mOutWidth;
    int mOutHeight;
    int mOutPixfmt;

    int mVideoFd[VIDEO_CHANNEL_SIZE];
    v4l2_memory mMemType;
    struct V4l2::buffer mV4l2Buf[VIDEO_CHANNEL_SIZE][V4L2_BUF_COUNT];

    int mIPUFd[VIDEO_CHANNEL_SIZE];
    struct IMXIPU::buffer mInIPUBuf[VIDEO_CHANNEL_SIZE];
    struct IMXIPU::buffer mOutIPUBuf[VIDEO_CHANNEL_SIZE];
};

#endif // CAPTUREWORKERV4L2IMPL_H
