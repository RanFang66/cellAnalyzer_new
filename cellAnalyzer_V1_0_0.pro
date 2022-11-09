QT       += core gui serialport sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    CameraCtrl.cpp \
    CellImageAlogrithm.cpp \
    DevCtrl.cpp \
    DlgAnimationUi.cpp \
    ExperiCtrl.cpp \
    ExperiData.cpp \
    ExperiSetting.cpp \
    QDlgLogin.cpp \
    QSerialWorker.cpp \
    debugModeUi.cpp \
    experiDataUi.cpp \
    experiSettingUi.cpp \
    inExperimentUi.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    CameraCtrl.h \
    CellImageAlogrithm.h \
    DevCtrl.h \
    DlgAnimationUi.h \
    ExperiCtrl.h \
    ExperiData.h \
    ExperiSetting.h \
    MainWindow.h \
    QDlgLogin.h \
    QSerialWorker.h \
    debugModeUi.h \
    experiDataUi.h \
    experiSettingUi.h \
    inExperimentUi.h

FORMS += \
    DlgAnimationUi.ui \
    MainWindow.ui \
    QDlgLogin.ui \
    debugModeUi.ui \
    experiDataUi.ui \
    experiSettingUi.ui \
    inExperimentUi.ui

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

RESOURCES += \
    resource.qrc
