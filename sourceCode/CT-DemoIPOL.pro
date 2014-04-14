#-------------------------------------------------
#
# Project created by QtCreator 2012-04-23T15:52:47
#
#-------------------------------------------------

QT       += core

#QT       -= gui

TARGET = ctree
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
LIBS            *=      \
                   -lImaGene

SOURCES += main.cpp \
    timer.cpp \
    subimage.cpp \
    segmentationbasedoncomponenttree.cpp \
    meaningfulscales.cpp \
    markedpixelsindicators.cpp \
    imagemarkerfile.cpp \
    histogram.cpp \
    grayscaleimagehistogram.cpp \
    componenttreeinputgenerator.cpp\
    meaningfulscalesalphaadjust.cpp\
    imagegradient.cpp \
    segmentationalphaparameterfinder.cpp


HEADERS += \
    timer.h \
    subimage.h \
    segmentationbasedoncomponenttree.h \
    meaningfulscales.h \
    markedpixelsindicators.h \
    imagemarkerfile.h \
    histogram.h \
    grayscaleimagehistogram.h \
    componenttreeinputgenerator.h \
    timeseries/src/tsutil.h \
    meaningfulscalesalphaadjust.h\
    imagegradient.h\
    segmentationalphaparameterfinder.h

