#-------------------------------------------------
#
# Project created by QtCreator 2017-03-15T17:04:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = surroundpark2
TEMPLATE = app

LIBS += -lopencv_core -lopencv_highgui
SOURCES += main.cpp\
        mainwindow.cpp \
    controller.cpp \
    stitchworker.cpp \
    stitchimpl.cpp \
    stitch_algorithm.cpp \
    util.cpp \
    v4l2.cpp \
    GoOnline.cpp \
    settings.cpp \
    imxipu.cpp \
    captureimpl.cpp \
    captureworkerv4l2impl.cpp \
    captureworkerbase.cpp

HEADERS  += mainwindow.h \
    common.h \
    ICapture.h \
    controller.h \
    stitchworker.h \
    IStitch.h \
    stitchimpl.h \
    stitch_algorithm.h \
    util.h \
    v4l2.h \
    settings.h \
    imxipu.h \
    captureworkerv4l2impl.h \
    captureworkerbase.h \
    captureimpl.h

FORMS    += mainwindow.ui
