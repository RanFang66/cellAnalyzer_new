#include "CameraCtrl.h"
#include <QDebug>

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
    emit cameraParasSetRet();
}

void CameraCtrl::cameraBrightInit()
{
    CameraSetAEC(m_id, true);
    CameraSetAETarget(m_id, 100);
    CameraSetAGC(m_id, true);
    CameraSetAWB(m_id, true);
    CameraSetSaturation(m_id, 1);
    CameraSetContrast(m_id, 1);
    CameraSetGamma(m_id, 1.33);
    CameraSetBlackLevel(m_id, 14);
}

void CameraCtrl::cameraFL1Init()
{
    CameraSetAEC(m_id, false);
    CameraSetExposure(m_id, 32767);
    CameraSetAGC(m_id, false);
    CameraSetGain(m_id, 126);
    CameraSetAWB(m_id, true);
    CameraSetSaturation(m_id, 1.66);
    CameraSetContrast(m_id, 1.24);
    CameraSetGamma(m_id, 1.33);
    CameraSetBlackLevel(m_id, 52);
}

void CameraCtrl::cameraFL2Init()
{
    CameraSetAEC(m_id, false);
    CameraSetExposure(m_id, 32767);
    CameraSetAGC(m_id, false);
    CameraSetGain(m_id, 45);
    CameraSetAWB(m_id, true);
    CameraSetSaturation(m_id, 1.24);
    CameraSetContrast(m_id, 1.29);
    CameraSetGamma(m_id, 0.82);
    CameraSetBlackLevel(m_id, 115);
}

void CameraCtrl::cameraErrorHandle()
{
    int error = 0;
    CameraGetLastError(&error);
    QString errorInfo = QString("Camera error: [%1]").arg(error) + errorMsg[error];

    qDebug() << errorInfo;
    emit cameraErrorHandle(error);
}
