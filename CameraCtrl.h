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

signals:
    void imageUpdated(const unsigned char *data, int width, int height);
    void cameraInitRet(bool);
    void cameraErrorHandle(int error);
public slots:
    void cameraInit();
    void updateImage();
    void changeResolution(int index);

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
};

#endif // CAMERACTRL_H
