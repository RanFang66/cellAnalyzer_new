#ifndef QSERIALWORKER_H
#define QSERIALWORKER_H

#include <QObject>
#include <QThread>
#include <QSerialPort>
#include <QMutex>

#define RECV_BUFF_LEN    (32)

class QSerialWorker : public QObject
{
    Q_OBJECT
public:
    explicit QSerialWorker(QObject *parent = nullptr);
    ~QSerialWorker();

signals:
    void serialConnected(bool);
    void serialDisconnected();
    void serialRecvData(char *data, int rx);
    void serialSentData(int tx);

public slots:
    void serialInit();
    void serialDisconnect();
    void sendSerialData(const char *buf, int len);
    void onSerialRecvData();
    void onSerialSendCmd(int dev_id, int cmd, int data);

public:
    Qt::HANDLE getThreadID();
    void setSerialTimeout(int sec);
    void setSerialParams(QString name, int baudrate = QSerialPort::Baud115200, int parity = QSerialPort::NoParity,
                         int dataBits = QSerialPort::Data8, int stopBits = QSerialPort::OneStop);

    enum FRAME_TYPE {
        CHIP_X_MOTOR_STATE = 1,
        CHIP_Y_MOTOR_STATE,
        CAMERA_MOTOR_STATE,
        FILTER_MOTOR_STATE,
        DEV_STATUS = 10,
    };

private:
    enum RECV_STATE {
        RECV_IDLE = 0,
        RECV_ID,
        RECV_DATA,
        RECV_CHECKSUM,
    };

    QSerialPort *m_serialPort = nullptr;
    QString m_serialName = "ttyAMA1";
    int     m_baudrate = QSerialPort::Baud115200;
    int     m_serialParity = QSerialPort::NoParity;
    int     m_serialDataBits = QSerialPort::Data8;
    int     m_serialStopBits = QSerialPort::OneStop;
    QMutex  serialWriteMutex;
    int     m_serialTimeout = 3000;

    int     recvFrameType = 0;
    int     recvFrameLength = 0;
    int     recvState = 0;
    int     recvDataLen = 0;
    char    recvDataBuff[22];
    void    recvDataSm(const char ch);

};

inline Qt::HANDLE getThreadID()
{
    return QThread::currentThreadId();
}

inline void QSerialWorker::setSerialTimeout(int sec) {
    if(sec > 0) {
        m_serialTimeout = sec;
    }
}

#endif // QSERIALWORKER_H
