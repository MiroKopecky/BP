QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG += console

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    freqMap.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    freqMap.h \
    mainwindow.h \
    persistence1d.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


#QCustomPlot
unix|win32: LIBS += -L$$PWD/../../../../usr/lib/ -lqcustomplot

INCLUDEPATH += $$PWD/../../../../usr/include
DEPENDPATH += $$PWD/../../../../usr/include

CONFIG += manjaro
#CONFIG += debian

# CUDA - Manjaro (as a package)
unix:!macx: LIBS += -L/usr/local/cuda-10.2/lib64/ -lcudart
INCLUDEPATH += /usr/local/cuda-10.2/include
DEPENDPATH += /usr/local/cuda-10.2/include

#ArrayFire - Manjaro (as a package)
unix:!macx: LIBS += -L/usr/lib/ -lafcuda
INCLUDEPATH += /usr/include
DEPENDPATH += /usr/include

#OpenCV - Manjaro (as a package)
unix:!macx: LIBS += -L/usr/lib/ -lopencv_core
unix:!macx: LIBS += -L/usr/lib/ -lopencv_imgproc
unix:!macx: LIBS += -L/usr/lib/ -lopencv_imgcodecs
unix:!macx: LIBS += -L/usr/lib/ -lopencv_highgui
INCLUDEPATH += /usr/include/opencv4
DEPENDPATH += /usr/include/opencv4

#Caffe - Manjaro (built from source)
unix:!macx: LIBS += -L/usr/local/lib/ -lcaffe
INCLUDEPATH += /usr/local/include
DEPENDPATH += /usr/local/include

#glog, protobuf, boost - Manjaro (as a package)
unix:!macx: LIBS += -L/usr/lib/ -lglog
unix:!macx: LIBS += -L/usr/lib/ -lprotobuf
unix:!macx: LIBS += -L/usr/lib/ -lboost_system
INCLUDEPATH += /usr/include
DEPENDPATH += /usr/include


unix|win32: LIBS += -L$$PWD/../build-libFreqMap-Desktop-Debug/ -llibFreqMap

INCLUDEPATH += $$PWD/../libFreqMap
DEPENDPATH += $$PWD/../libFreqMap

unix|win32: LIBS += -L$$PWD/../../build-openfinger-preprocessor-Desktop-Debug/ -lofpreproc

INCLUDEPATH += $$PWD/../../openfinger-preprocessor
DEPENDPATH += $$PWD/../../openfinger-preprocessor
