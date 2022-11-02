#ifndef SERIALPROTOCOL_H
#define SERIALPROTOCOL_H

#include <QObject>
#include <QSerialWorker.h>
#include <SerialDataQueue.h>
#include <QThread>

#define SEND_BUFF_LEN       (9)
#define RECV_FRAME_LEN      (25)
class SerialProtocol : public QObject
{
    Q_OBJECT
public:
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
        RUN_POS = 0,
        STOP,
        RUN_FORWARD,
        RUN_BACKWARD,
        RESET,
        SET_SPEED,
    };
    explicit SerialProtocol(QObject *parent = nullptr);

signals:
    void writeToSerial(const char *buf, int len);
    void updateDevStatus();

public:
    void sendDevCmd(int devId, int cmd, int data);
    int getMotorPos(int devId);
    int getMotorLimitStatus(int devId);
    int getLedState();
    int getChipState();

public slots:
    void onSerialRecvData(const char *data, int len);

private:
    char sendBuff[SEND_BUFF_LEN+1];
    SerialDataQueue *recvDataQueue;
    int  recvFrameLen = RECV_FRAME_LEN;
    int  motorPos[4];
    int  motorLimitState[4];
    int  ledState;
    int  chipState;


    void initSerialWorker();
};

#endif // SERIALPROTOCOL_H
