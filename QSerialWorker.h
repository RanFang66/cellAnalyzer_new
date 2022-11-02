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
    void sendSerialData(const char *buf, int len);
    void onSerialRecvData();

public:
    Qt::HANDLE getThreadID();
    void setSerialTimeout(int sec);
    void setSerialParams(QString name, int baudrate = QSerialPort::Baud115200, int parity = QSerialPort::NoParity,
                         int dataBits = QSerialPort::Data8, int stopBits = QSerialPort::OneStop);

private:
    QSerialPort *m_serialPort = nullptr;
    QString m_serialName = "ttyAMA1";
    int     m_baudrate = QSerialPort::Baud115200;
    int     m_serialParity = QSerialPort::NoParity;
    int     m_serialDataBits = QSerialPort::Data8;
    int     m_serialStopBits = QSerialPort::OneStop;
    QMutex  serialWriteMutex;
    int     m_serialTimeout = 3000;
    QByteArray dataBuff;
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
