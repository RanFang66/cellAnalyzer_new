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
    }
    emit serialConnected(ret);
}

void QSerialWorker::sendSerialData(const char *data, int len)
{
    if (data == nullptr || len <= 0)
        return;

    QMutexLocker Locker(&serialWriteMutex);
    if (m_serialPort && m_serialPort->isOpen()) {
        m_serialPort->clear();
        int ret = m_serialPort->write(data, len);
        m_serialPort->flush();
        emit serialSentData(ret);
        m_serialPort->waitForBytesWritten(m_serialTimeout);
    }
}

void QSerialWorker::onSerialRecvData()
{
    if (m_serialPort && m_serialPort->isOpen()) {
        dataBuff.clear();
        dataBuff.append(m_serialPort->readAll());
        if (dataBuff.isEmpty()) {
            qDebug() << "read data error";
            return;
        }
        int len = dataBuff.length();
        emit serialRecvData(dataBuff.data(), len);
    }
}

void QSerialWorker::setSerialParams(QString name, int baudrate, int parity, int dataBits, int stopBits) {
    m_serialName = name;
    m_baudrate = baudrate;
    m_serialParity = parity;
    m_serialDataBits = dataBits;
    m_serialStopBits = stopBits;
}
