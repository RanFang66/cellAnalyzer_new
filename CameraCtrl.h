#ifndef CAMERACTRL_H
#define CAMERACTRL_H

#include <QObject>
#include "JHCap.h"

#define CAM_INDEX_0         (0)

class CameraCtrl : public QObject
{
    Q_OBJECT
public:
    explicit CameraCtrl(QObject *parent = nullptr);
    ~CameraCtrl();

signals:
    void imageUpdated(unsigned char *data, int width, int height);
    void cameraInitRet(bool);
    void cameraErrorHandle(int error);
    void cameraConnected(bool);

public slots:
    void cameraInit();
    void cameraConnect();
    void updateImage();
    void changeResolution(int index);
    void whiteBalance(bool en);
    void autoExplosure(bool en);
    void cameraDisconnect();

private:
    const static QString errorMsg[20];
    struct ImgResolution {
        int width;
        int height;
    };
    int                 m_id = CAM_INDEX_0;
    QString             m_camName;
    QString             m_camModel;
    QString             m_APIVersion;
    QString             m_FirmVersion;
    int                 m_resolutionCount;
    struct ImgResolution *m_resolutions;
    int                 m_imgWidth;
    int                 m_imgHeight;
    int                 m_bufLen;
    unsigned char       *m_buff;

    void cameraErrorHandle();

public:
    void getGetResolution(int index, int &width, int &height);
    int getResolutionCount(void);
};

inline void CameraCtrl::getGetResolution(int index, int &width, int &height)
{
    if (index < m_resolutionCount && index >= 0) {
        width = m_resolutions[index].width;
        height = m_resolutions[index].height;
    }
}

inline int CameraCtrl::getResolutionCount()
{
    return m_resolutionCount;
}

#endif // CAMERACTRL_H
