#include "SerialDataQueue.h"

SerialDataQueue::SerialDataQueue(QObject *parent) : QObject(parent)
{
    m_data = new char[m_size];
    m_front = 0;
    m_rear = 0;
    m_len = 0;
}

SerialDataQueue::SerialDataQueue(int size, QObject *parent) : QObject(parent), m_size(size+1)
{
    m_data = new char[m_size];
    m_front = 0;
    m_rear = 0;
    m_len = 0;
}

int SerialDataQueue::enQueueData(const char *data, int len)
{
    if (m_len + len > m_size)
        return 0;
    for (int i = 0; i < len; i++) {
        m_data[m_rear] = data[i];
        m_rear = (m_rear+1) % (m_size);
    }
    m_len += len;
    return len;
}

int SerialDataQueue::deQueueData(int len)
{
    if (m_len < len)
        return 0;
    for (int i = 0; i < len; i++) {
        m_data[m_front] = 0;
        m_front = (m_front + 1) % m_size;
    }
    m_len -= len;
    return len;
}



