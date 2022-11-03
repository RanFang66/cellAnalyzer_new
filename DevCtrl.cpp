#include "DevCtrl.h"

DevCtrl::DevCtrl(QObject *parent) : QObject(parent)
{
    initDeviceCtrl();
    initCameraCtrl();

    updateDevStatus();
}

DevCtrl::~DevCtrl()
{
    if (m_camThread) {
        m_camThread->quit();
        m_camThread->wait();
        m_camThread->deleteLater();
    }
    if (m_serialThread) {
        m_serialThread->quit();
        m_serialThread->wait();
        m_serialThread->deleteLater();
    }
}

void DevCtrl::onSerialConnected(bool ok)
{
    m_serialState = ok;
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
    emit devStatusUpdated();
}

void DevCtrl::onCamInitRet(bool ok)
{
    m_camState = ok;
}

void DevCtrl::onCamImageUpdate(unsigned char *data, int width, int height)
{
    m_cvImage = Mat(height, width, CV_8UC3, data);
    m_qImage = QImage(data, width, height, QImage::Format_BGR888);
    emit imageUpdated();
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
    m_camCtrl->moveToThread(m_camThread);
    connect(m_camCtrl, &CameraCtrl::cameraInitRet, this, &DevCtrl::onCamInitRet);
    connect(m_camCtrl, &CameraCtrl::imageUpdated, this, &DevCtrl::onCamImageUpdate);
    connect(m_camThread, &QThread::finished, m_camCtrl, &QObject::deleteLater);
    connect(m_camThread, &QThread::started, m_camCtrl, &CameraCtrl::cameraInit);
    m_camThread->start();
}


int DevCtrl::str2int(const char *data, int len)
{
    int sum = 0;
    for(int i = len-1; i >= 0; i--) {
        sum += data[i] - '0';
        sum *= 10;
    }
    return sum;
}
