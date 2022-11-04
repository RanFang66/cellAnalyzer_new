#include "DevCtrl.h"
#include <QDebug>

int DevCtrl::autoFocusStartPos = 130;
int DevCtrl::autoFocusEndPos = 170;
int DevCtrl::autoFocusStep = 2;
DevCtrl::DevCtrl(QObject *parent) : QObject(parent)
{
    initDeviceCtrl();
    initCameraCtrl();
    initAutoFocus();
    updateDevStatus();
}

DevCtrl::~DevCtrl()
{
    if (m_camThread) {
        m_camThread->quit();
        m_camThread->wait();
    }
    if (m_serialThread) {
        m_serialThread->quit();
        m_serialThread->wait();
    }
}

void DevCtrl::onSerialConnected(bool ok)
{
    qDebug() << "Device Ctrl: serial connected!";
    m_serialState = ok;
    motorReset(CAMERA_MOTOR);
}

void DevCtrl::onSerialRecvFrame(const char *data, int len)
{
    if (len < 22) {
        return ;
    }
    for (int i = 0; i < 4; i++) {
        m_motorPos[i] = str2int(data + 5*i, 4);
        m_motorLimitState[i] = data[5*i+4] - '0';
    }
    m_ledState = data[20] - '0';
    m_chipState = data[21] - '0';
//    if (m_autoFocusState == FOCUS_PROCESS && m_motorPos[2] >= m_focusNextPos) {
//        emit capImage();
//    } else if (m_autoFocusState == FOCUS_COMPLETE && m_motorPos[2] >= m_focusNextPos) {
//        m_autoFocusState = FOCUS_IDLE;
//        emit autoFocusComplete();
//    } else {
//        emit devStatusUpdated();
//    }
    emit devStatusUpdated();
}

void DevCtrl::onCamInitRet(bool ok)
{
    m_camState = ok;

}

void DevCtrl::onCamImageUpdate(unsigned char *data, int width, int height)
{
    if (m_autoFocusState == FOCUS_PROCESS) {
        m_cvImage = Mat(height, width, CV_8UC3, data);
        m_clarity = calcClarity(m_cvImage);
        if (m_clarity > m_maxClarity) {
            m_maxClarity = m_clarity;
            m_focusPos = m_focusNextPos;
        }
        if (m_focusNextPos >= autoFocusEndPos) {
            m_focusNextPos =  m_focusPos;
            m_autoFocusState = FOCUS_COMPLETE;
        } else {
            m_focusNextPos += autoFocusStep;
        }
        emit sendDevCmd(CAMERA_MOTOR, MOTOR_RUN_POS, m_focusNextPos);
    } else {
        m_qImage = QImage(data, width, height, QImage::Format_BGR888);
        emit imageUpdated();
    }
}

void DevCtrl::onCamTimerTimeout()
{
    emit capImage();
}

void DevCtrl::onFocusTimerTimeout()
{
    //updateDevStatus();
    if (m_autoFocusState == FOCUS_COMPLETE){
        m_autoFocusState = FOCUS_IDLE;
        emit autoFocusComplete();
    } else {
        emit capImage();
    }
}

void DevCtrl::motorRun(int id, int cmd, int pos)
{
    emit sendDevCmd(id, cmd, pos);
}

void DevCtrl::motorStop(int id)
{
    emit sendDevCmd(id, MOTOR_STOP, 0);
}

void DevCtrl::motorSetSpeed(int id, int speed)
{
    emit sendDevCmd(id, MOTOR_SET_SPEED,speed);
}

void DevCtrl::motorReset(int id)
{
    emit sendDevCmd(id, MOTOR_RESET, 0);
}

void DevCtrl::ledLigthOn(int color)
{
    emit sendDevCmd(LED, color, 0);
}

void DevCtrl::ledLightOff()
{
    emit sendDevCmd(LED, LED_OFF, 0);
}

void DevCtrl::updateDevStatus()
{
    emit sendDevCmd(8, 0, 0);
}

void DevCtrl::camSnap()
{
    emit capImage();
}

void DevCtrl::cameraStop()
{
    m_camTimer->stop();
}

void DevCtrl::cameraAutoExplosure(bool checked)
{
    m_camCtrl->autoExplosure(checked);
}

void DevCtrl::cameraWhiteBalance()
{
    m_camCtrl->whiteBalance(true);
}

void DevCtrl::startAutoFocus(bool act)
{
    if (act) {
        if (m_camTimer->isActive())
            m_camTimer->stop();
        m_focusNextPos = autoFocusStartPos;
        motorRun(CAMERA_MOTOR, MOTOR_RUN_POS, m_focusNextPos);
        m_autoFocusState = FOCUS_PROCESS;
        m_focusTimer->start();
    }
}

void DevCtrl::cameraRun(int framePeriod)
{
    m_camTimer->start(framePeriod);
    emit capImage();
}

void DevCtrl::initDeviceCtrl()
{
    m_serialWorker = new QSerialWorker();
    m_serialThread = new QThread();
    m_serialWorker->moveToThread(m_serialThread);

    m_serialWorker->setSerialParams(QString("ttyAMA1"));
    connect(m_serialWorker, &QSerialWorker::serialRecvData, this, &DevCtrl::onSerialRecvFrame);
    connect(this, &DevCtrl::sendDevCmd, m_serialWorker, &QSerialWorker::onSerialSendCmd);
    connect(m_serialWorker, &QSerialWorker::serialConnected, this, &DevCtrl::onSerialConnected);
//    connect(m_serialWorker, &QSerialWorker::serialDisconnected, this, &MainWindow::onSerialDisconnected);
    connect(m_serialThread, &QThread::finished, m_serialWorker, &QObject::deleteLater);
    connect(m_serialThread, &QThread::started, m_serialWorker, &QSerialWorker::serialInit);
    m_serialThread->start();
}

void DevCtrl::initCameraCtrl()
{
    m_camCtrl = new CameraCtrl();
    m_camThread = new QThread();
    m_camTimer = new QTimer(this);
    m_camTimer->setInterval(1000);
    m_camTimer->stop();

    m_camCtrl->moveToThread(m_camThread);
    connect(this, &DevCtrl::capImage, m_camCtrl, &CameraCtrl::updateImage);
    connect(m_camCtrl, &CameraCtrl::cameraInitRet, this, &DevCtrl::onCamInitRet);
    connect(m_camCtrl, &CameraCtrl::imageUpdated, this, &DevCtrl::onCamImageUpdate);
    connect(m_camThread, &QThread::finished, m_camCtrl, &QObject::deleteLater);
    connect(m_camThread, &QThread::started, m_camCtrl, &CameraCtrl::cameraInit);
    connect(m_camTimer, SIGNAL(timeout()), this, SLOT(onCamTimerTimeout()));
    m_camThread->start();
}

void DevCtrl::initAutoFocus()
{
    m_focusTimer = new QTimer(this);
    m_focusTimer->setInterval(1000);
    m_focusTimer->stop();
    m_autoFocusState = FOCUS_IDLE;
    m_focusNextPos = autoFocusStartPos;
    m_focusPos = 150;
    m_maxClarity = 0;
    m_clarity = 0;

    connect(m_focusTimer, SIGNAL(timeout()), this, SLOT(onFocusTimerTimeout()));
}

double DevCtrl::calcClarity(Mat &img)
{
    Mat grayImg;
    cvtColor(img, grayImg, COLOR_BGR2GRAY);
    Laplacian(grayImg, grayImg, CV_8U);

    double meanVal;

    meanVal = mean(grayImg)[0];

    return meanVal;
}


int DevCtrl::str2int(const char *data, int len)
{
    int sum = 0;
    for(int i = len-1; i >= 0; i--) {
        sum *= 10;
        sum += data[i] - '0';
    }
    return sum;
}
