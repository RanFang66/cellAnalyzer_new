#include "CameraCtrl.h"
#include <QDebug>
#include <QElapsedTimer>

const QString CameraCtrl::errorMsg[] = {
    "API OK",
    "camera index not valid",
    "parameter not valid",
    "camera not initialized",
    "usb disconnected",
    "camera not found",
    "camera has been initialized",
    "reconnected failed",
    "function not supported in this camera",
    "control pipe not valid",
    "read data error",
    "write data error",
    "camera size not valid",
    "bad frame",
    "user aborted",
    "data transfer failed",
    "read data timeout",
    "out of memory",
    "camera is reseting",
};

CameraCtrl::CameraCtrl(QObject *parent) : QObject(parent)
{
    db = QSqlDatabase::database("cellDataConn");
    query = new QSqlQuery(db);
}

CameraCtrl::~CameraCtrl()
{
    CameraFree(m_id);
}



void CameraCtrl::cameraInit()
{
    int count;
    bool ret = false;
    if (API_OK == CameraGetCount(&count)) {
        if (count > 0) {
            CameraFree(m_id);
            if (API_OK == CameraInit(m_id)) {
                char name[128];
                char model[128];
                CameraGetName(m_id, name, model);
                m_camName = QString::fromLatin1(name);
                m_camModel = QString::fromLatin1(model);
                CameraGetResolutionCount(m_id, &m_resolutionCount);
                for (int i = 0; i < m_resolutionCount; i++) {
                    CameraGetResolution(m_id, 0, &m_resolutions[i].width, &m_resolutions[i].height);
                }
                CameraGetImageSize(m_id, &m_imgWidth, &m_imgHeight);
                CameraGetImageBufferSize(m_id, &m_bufLen, CAMERA_IMAGE_RGB24);
                m_buff = new unsigned char[m_bufLen];
                ret = true;
            } else {
                cameraErrorHandle();
            }
        } else {
            qDebug() << "Camera: can not find any camera!";
        }
    }
    emit cameraInitRet(ret);
}

void CameraCtrl::cameraConnect()
{
    int count;
    bool ret = false;
    if (API_OK == CameraGetCount(&count)) {
        if (count > 0) {
            if (API_OK == CameraInit(m_id)) {
                ret = true;
            } else {
                cameraErrorHandle();
            }
        } else {
            qDebug() << "Camera: can not find any camera!";
        }
    }
    emit cameraConnected(ret);
}

void CameraCtrl::updateImage()
{
    CameraClearHostBuffer(m_id);
    if (API_OK == CameraQueryImage(m_id, m_buff, &m_bufLen,
                                   CAMERA_IMAGE_RGB24 | CAMERA_IMAGE_BGR)){
        emit imageUpdated(m_buff, m_imgWidth, m_imgHeight);
    } else {
        cameraErrorHandle();
    }
}

void CameraCtrl::changeResolution(int index)
{
    if (API_OK != CameraSetResolution(m_id, index, &m_imgWidth, &m_imgHeight)) {
        cameraErrorHandle();
    }
}


void CameraCtrl::cameraDisconnect()
{
    CameraFree(m_id);
}

void CameraCtrl::onSetCamParas(int type)
{
    switch (type) {
    case 1:
        cameraBrightInit();
        break;
    case 2:
        cameraFL1Init();
        break;
    case 3:
        cameraFL2Init();
        break;
    default:
        break;
    }
    QElapsedTimer delay;
    delay.start();
    while(delay.elapsed() < 500)
        ;
    emit cameraParasSetRet();
}

void CameraCtrl::cameraBrightInit()
{
    QString qryStr = QString("SELECT * FROM camParas WHERE paraId=%1").arg(1);
    if (query->exec(qryStr)&& query->next()) {
        int aec = query->value(2).toInt();
        int agc = query->value(3).toInt();
        int awb = query->value(4).toInt();
        int expoTime = query->value(5).toInt();
        int expoGain = query->value(6).toInt();
        int aeTarget = query->value(7).toInt();
        double gamma = query->value(8).toDouble();
        double contrast = query->value(9).toDouble();
        double sat = query->value(10).toDouble();
        int blackLevel = query->value(11).toInt();
        double rg = query->value(12).toDouble();
        double gg = query->value(13).toDouble();
        double bg = query->value(14).toDouble();

        CameraSetSaturation(m_id, sat);
        CameraSetContrast(m_id, contrast);
        CameraSetGamma(m_id, gamma);
        CameraSetBlackLevel(m_id, blackLevel);

        CameraSetAEC(m_id, aec);
        CameraSetAGC(m_id, agc);
        CameraSetAWB(m_id, awb);

        if (!aec)
            CameraSetExposure(m_id, expoTime);
        else
            CameraSetAETarget(m_id, aeTarget);

        if (!agc)
            CameraSetGain(m_id, expoGain);

        if (!awb)
            CameraSetWBGain(m_id, rg, gg, bg);
//        CameraOnePushWB(m_id);
    }

}

void CameraCtrl::cameraFL1Init()
{
    QString qryStr = QString("SELECT * FROM camParas WHERE paraId=%1").arg(2);
    if (query->exec(qryStr)&& query->next()) {
        int aec = query->value(2).toInt();
        int agc = query->value(3).toInt();
        int awb = query->value(4).toInt();
        int expoTime = query->value(5).toInt();
        int expoGain = query->value(6).toInt();
        int aeTarget = query->value(7).toInt();
        double gamma = query->value(8).toDouble();
        double contrast = query->value(9).toDouble();
        double sat = query->value(10).toDouble();
        int blackLevel = query->value(11).toInt();
        double rg = query->value(12).toDouble();
        double gg = query->value(13).toDouble();
        double bg = query->value(14).toDouble();

        CameraSetSaturation(m_id, sat);
        CameraSetContrast(m_id, contrast);
        CameraSetGamma(m_id, gamma);
        CameraSetBlackLevel(m_id, blackLevel);

        CameraSetAEC(m_id, aec);
        CameraSetAGC(m_id, agc);
        CameraSetAWB(m_id, awb);

        if (!aec)
            CameraSetExposure(m_id, expoTime);
        else
            CameraSetAETarget(m_id, aeTarget);

        if (!agc)
            CameraSetGain(m_id, expoGain);

        if (!awb)
            CameraSetWBGain(m_id, rg, gg, bg);
//        CameraOnePushWB(m_id);
    }
}

void CameraCtrl::cameraFL2Init()
{
    QString qryStr = QString("SELECT * FROM camParas WHERE paraId=%1").arg(3);
    if (query->exec(qryStr)&& query->next()) {
        int aec = query->value(2).toInt();
        int agc = query->value(3).toInt();
        int awb = query->value(4).toInt();
        int expoTime = query->value(5).toInt();
        int expoGain = query->value(6).toInt();
        int aeTarget = query->value(7).toInt();
        double gamma = query->value(8).toDouble();
        double contrast = query->value(9).toDouble();
        double sat = query->value(10).toDouble();
        int blackLevel = query->value(11).toInt();
        double rg = query->value(12).toDouble();
        double gg = query->value(13).toDouble();
        double bg = query->value(14).toDouble();

        CameraSetSaturation(m_id, sat);
        CameraSetContrast(m_id, contrast);
        CameraSetGamma(m_id, gamma);
        CameraSetBlackLevel(m_id, blackLevel);

        CameraSetAEC(m_id, aec);
        CameraSetAGC(m_id, agc);
        CameraSetAWB(m_id, awb);

        if (!aec)
            CameraSetExposure(m_id, expoTime);
        else
            CameraSetAETarget(m_id, aeTarget);

        if (!agc)
            CameraSetGain(m_id, expoGain); 

        if (!awb)
            CameraSetWBGain(m_id, rg, gg, bg);
//        CameraOnePushWB(m_id);
    }
}

void CameraCtrl::cameraErrorHandle()
{
    int error = 0;
    CameraGetLastError(&error);
    QString errorInfo = QString("Camera error: [%1]").arg(error) + errorMsg[error];

    qDebug() << errorInfo;
    emit cameraErrorHandle(error);
}
