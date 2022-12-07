#include "QSerialWorker.h"
#include <QDebug>
QSerialWorker::QSerialWorker(QObject *parent) : QObject(parent)
{

}

QSerialWorker::~QSerialWorker()
{
    if (m_serialPort) {
        if(m_serialPort->isOpen()) {
            m_serialPort->clear();
            m_serialPort->close();
            qDebug() << "serial closed";
            emit serialDisconnected();
            m_serialPort->deleteLater();
        }
    }
}

void QSerialWorker::serialInit()
{
    m_serialPort = new QSerialPort();
    m_serialPort->setPortName(m_serialName);
    m_serialPort->setBaudRate(m_baudrate);
    m_serialPort->setDataBits(static_cast<QSerialPort::DataBits>(m_serialDataBits));
    m_serialPort->setStopBits(static_cast<QSerialPort::StopBits>(m_serialStopBits));
    m_serialPort->setParity(static_cast<QSerialPort::Parity>(m_serialParity));

    bool ret = m_serialPort->open(QIODevice::ReadWrite);
    if (ret) {
        connect(m_serialPort, SIGNAL(readyRead()), this, SLOT(onSerialRecvData()));
    } else {
        qDebug() << m_serialPort->errorString();
    }
    emit serialConnected(ret);
}

void QSerialWorker::serialDisconnect()
{
    m_serialPort->clear();
    m_serialPort->close();
}

void QSerialWorker::sendSerialData(const char *data, int len)
{
    if (data == nullptr || len <= 0)
        return;

//    QMutexLocker Locker(&serialWriteMutex);
    if (m_serialPort && m_serialPort->isOpen()) {
        m_serialPort->clear();
        int ret = m_serialPort->write(data, len);
        m_serialPort->flush();
//        emit serialSentData(ret);
//        m_serialPort->waitForBytesWritten(m_serialTimeout);
    }
}

void QSerialWorker::onSerialRecvData()
{
    if (m_serialPort && m_serialPort->isOpen()) {
        QByteArray dataBuff;
        dataBuff.append(m_serialPort->readAll());
        if (dataBuff.isEmpty()) {
            qDebug() << "read data error";
            return;
        }
        for (int i = 0; i < dataBuff.length();i++) {
            recvDataSm(dataBuff.at(i));
        }
    }
}

void QSerialWorker::onSerialSendCmd(int devId, int cmd, int data)
{
    char sendBuff[10];
    int sum = 0;
    sendBuff[0] = (char)0x58;
    sendBuff[1] = (char)0x59;
    sendBuff[2] = (char)(devId & 0x000000FF);
    sendBuff[3] = (char)(cmd & 0x000000FF);
    sendBuff[7] = (char)(data & 0x000000FF);
    sendBuff[6] = (char)((data & 0x0000FF00) >> 8);
    sendBuff[5] = (char)((data & 0x00FF0000) >> 16);
    sendBuff[4] = (char)((data & 0xFF000000) >> 24);
    for (int i = 2; i < 8; i++) {
        sum += sendBuff[i];
    }
    sendBuff[8] = sum & 0x000000FF;

//   QMutexLocker Locker(&serialWriteMutex);
    if (m_serialPort && m_serialPort->isOpen()) {
        m_serialPort->clear();
        int ret = m_serialPort->write(sendBuff, 9);
        m_serialPort->flush();
//        emit serialSentData(ret);
//        m_serialPort->waitForBytesWritten(m_serialTimeout);
    }
}

void QSerialWorker::setSerialParams(QString name, int baudrate, int parity, int dataBits, int stopBits) {
    m_serialName = name;
    m_baudrate = baudrate;
    m_serialParity = parity;
    m_serialDataBits = dataBits;
    m_serialStopBits = stopBits;
}

void QSerialWorker::recvDataSm(const char ch)
{
    switch (recvState) {
    case RECV_IDLE:
    {
        switch (ch) {
        case 'X':
            recvFrameType = CHIP_X_MOTOR_STATE;
            recvFrameLength = 5;
            recvDataLen = 0;
            recvState = RECV_DATA;
            break;
        case 'Y':
            recvFrameType = CHIP_Y_MOTOR_STATE;
            recvFrameLength = 5;
            recvDataLen = 0;
            recvState = RECV_DATA;
            break;

        case 'L':
            recvFrameType = CAMERA_MOTOR_STATE;
            recvFrameLength = 5;
            recvDataLen = 0;
            recvState = RECV_DATA;
            break;
        case 'F':
            recvFrameType = FILTER_MOTOR_STATE;
            recvFrameLength = 5;
            recvDataLen = 0;
            recvState = RECV_DATA;
            break;
        case 'S':
            recvFrameType = DEV_STATUS;
            recvFrameLength = 22;
            recvDataLen = 0;
            recvState = RECV_DATA;
            break;
        default:
            return ;
        }
        break;
    }

    case RECV_DATA:
    {
        recvDataBuff[recvDataLen++] = ch;
        if (recvDataLen >= recvFrameLength) {
            recvDataLen = 0;
            recvState = RECV_CHECKSUM;
        }
        break;
    }
    case RECV_CHECKSUM:
    {
        quint32 sum = 0;
        for (int i = 0; i < recvFrameLength; i++) {
            sum += recvDataBuff[i];
        }
        if ((quint8)(sum & 0x000000FF) == ch) {
            emit serialRecvData(recvDataBuff, recvFrameType);
            recvFrameLength = 0;
            recvState= RECV_IDLE;
        } else {
            qDebug() << "checksum error!";
            recvState = RECV_IDLE;
        }
        break;
    }
    default:
        break;
    }
}


