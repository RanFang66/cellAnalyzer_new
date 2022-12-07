#ifndef DEVCTRL_H
#define DEVCTRL_H

#include <QObject>
#include <QThread>
#include <QImage>
#include "QSerialWorker.h"
#include "CameraCtrl.h"
#include <opencv2/opencv.hpp>
#include <QTimer>

using namespace cv;
class DevCtrl : public QObject
{
    Q_OBJECT
public:
    explicit DevCtrl(QObject *parent = nullptr);
    ~DevCtrl();
    enum DEVICE_ID {
        CHIP_MOTOR_X = 1,
        CHIP_MOTOR_Y,
        CAMERA_MOTOR,
        FILTER_MOTOR,
        INSERT_MOTOR,
        LED,
        LED_ADJUST,
    };

    enum MOTOR_CMD {
        MOTOR_RUN_POS = 0,
        MOTOR_STOP,
        MOTOR_RUN_FORWARD,
        MOTOR_RUN_BACKWARD,
        MOTOR_RESET,
        MOTOR_SET_SPEED,
    };

    enum LED_CMD {
        LED_BLUE = 1,
        LED_GREEN,
        LED_WHITE,
        LED_OFF,
    };

public slots:
    void onSerialConnected(bool ok);
    void onSerialRecvFrame(const char *data, int frameType);
    void onCamInitRet(bool ok);
    void onCamImageUpdate(unsigned char *data, int width, int height);
    void onCamConnected(bool ok);
    void onCamTimerTimeout();
    void onAutoFocusSet(int low, int high, int step);

signals:
    void sendDevCmd(int devId, int cmd, int data);
    void capImage();
    void imageUpdated();
    void devStatusUpdated();
    void cameraMotorStateUpdated();
    void chipXMotorStateUpdated();
    void chipYMotorStateUpdated();
    void filterMotorStateUpdated();
    void autoFocusComplete();
    void changeResolution(int index);
    void connectCam();
    void disconnectCam();
    void camInitOk();
    void camInitFailed();
    void initCameraParameters(int type);

public:
    void motorRun(int id, int cmd, int data = 0);
    void motorStop(int id);
    void motorSetSpeed(int id, int speed);
    void motorReset(int id);
    void ledLigthOn(int color);
    void ledLightOff();
    void updateDevStatus();
    void camSnap();
    void cameraRun(int framePeriod = 1000);
    void cameraStop();
    void disconnectCamera();
    void connectCamera();
    int  getCamResolutionsCount();
    void getCamResolution(int index, int &width, int &height);
    void camChangeResolution(int index);
    void initCameraParas(int type);


    int getMotorPos(int id);
    int chipPos_X();
    int chipPos_Y();
    int filterPos();
    int getMotorLimitState(int id);
    int getLedState();
    int getChipState();
    Mat getCVImage();
    QImage getQImage();
    int getCameraState();
    int getSerialState();
    double getClarity();

    void startAutoFocus(bool act);
    CameraCtrl          *m_camCtrl;

private:
    enum AUTO_FOCUS_STATE {
        FOCUS_IDLE = 0,
        FOCUS_START,
        FOCUS_INIT,
        FOCUS_PROCESS,
        FOCUS_COMPLETE,
    };

    struct CamResolution {
        int width;
        int height;
    };

    QSerialWorker       *m_serialWorker;
    QThread             *m_serialThread;

    QThread             *m_camThread;
    QTimer              *m_camTimer;
    QTimer              *m_focusTimer;

    int     m_camState;
    int     m_serialState;
    int     m_motorPos[4];
    int     m_motorLimitState[4];
    int     m_ledState;
    int     m_chipState;
    Mat     m_cvImage;
    double  m_clarity = 0.0;
    QImage  m_qImage;
    int     m_resolutionCount;
    struct  CamResolution m_resolutions[8];

    int     m_autoFocusState = FOCUS_IDLE;
    int     m_focusNextPos;
    int     m_focusPos;
    double  m_maxClarity = 0.0;
    int autoFocusStartPos;
    int autoFocusEndPos;
    int autoFocusStep;

    void initDeviceCtrl();

    void initCameraCtrl();

    void initAutoFocus();

    double calcClarity(Mat &img);

    int  str2int(const char *data, int len);
};

inline int DevCtrl::getCamResolutionsCount()
{
    return m_resolutionCount;
}

inline void DevCtrl::getCamResolution(int index, int &width, int &height)
{
    if (index < 0 || index >= m_resolutionCount)
        return;
    width = m_resolutions[index].width;
    height = m_resolutions[index].height;
}

inline int DevCtrl::getMotorPos(int id)
{
    return m_motorPos[id-1];
}

inline int DevCtrl::chipPos_X()
{
    return m_motorPos[0];
}

inline int DevCtrl::chipPos_Y()
{
    return m_motorPos[1];
}

inline int DevCtrl::filterPos()
{
    return m_motorPos[3];
}

inline int DevCtrl::getMotorLimitState(int id)
{
    return m_motorLimitState[id-1];
}

inline int DevCtrl::getLedState()
{
    return m_ledState;
}

inline int DevCtrl::getChipState()
{
    return m_chipState;
}

inline Mat DevCtrl::getCVImage()
{
    return m_cvImage;
}

inline QImage DevCtrl::getQImage()
{
    return m_qImage;
}

inline int DevCtrl::getCameraState()
{
    return m_camState;
}

inline int DevCtrl::getSerialState()
{
    return m_serialState;
}

inline double DevCtrl::getClarity()
{
    return m_clarity;
}



#endif // DEVCTRL_H
