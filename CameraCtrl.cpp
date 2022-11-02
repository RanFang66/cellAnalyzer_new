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
                m_resolutions = new struct ImgResolution[m_resolutionCount];
                for (int i = 0; i < m_resolutionCount; i++) {
                    CameraGetResolution(m_id, 0, &m_resolutions[i].width, &m_resolutions->height);
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

void CameraCtrl::updateImage()
{
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

void CameraCtrl::cameraErrorHandle()
{
    int error = 0;
    CameraGetLastError(&error);
    QString errorInfo = QString("Camera error: [%1]").arg(error) + errorMsg[error];

    qDebug() << errorInfo;
    emit cameraErrorHandle(error);
}
