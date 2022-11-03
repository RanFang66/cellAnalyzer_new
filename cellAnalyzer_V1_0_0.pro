QT       += core gui serialport sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    CameraCtrl.cpp \
    DevCtrl.cpp \
    QDlgLogin.cpp \
    QSerialWorker.cpp \
    SerialDataQueue.cpp \
    debugModeUi.cpp \
    experiDataUi.cpp \
    experiSettingUi.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    CameraCtrl.h \
    DevCtrl.h \
    MainWindow.h \
    QDlgLogin.h \
    QSerialWorker.h \
    SerialDataQueue.h \
    debugModeUi.h \
    experiDataUi.h \
    experiSettingUi.h

FORMS += \
    MainWindow.ui \
    QDlgLogin.ui \
    debugModeUi.ui \
    experiDataUi.ui \
    experiSettingUi.ui

TRANSLATIONS += \
    cellAnalyzer_V1_0_0_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix:!macx: LIBS += -L$$PWD/cam_sdk/arm64/ -lJHCap
unix:!maxx: LIBS += /usr/lib/aarch64-linux-gnu/libopencv*.so

INCLUDEPATH += $$PWD/cam_sdk/arm64 \
               /usr/include/opencv4/.
DEPENDPATH += $$PWD/cam_sdk/arm64
