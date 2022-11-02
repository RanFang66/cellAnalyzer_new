#ifndef SERIALDATAQUEUE_H
#define SERIALDATAQUEUE_H

#include <QObject>

#define DEFAULT_QUEUE_SIZE (128)
class SerialDataQueue : public QObject
{
    Q_OBJECT
public:
    explicit SerialDataQueue(QObject *parent = nullptr);
    SerialDataQueue(int size, QObject *parent = nullptr);
signals:

public:
    int enQueueData(const char *data, int len);
    int deQueueData(int len);
    int queueDataLen();
    bool isFull();
    bool isEmpty();
    char at(int index);

private:
    char *m_data;
    int  m_front;
    int  m_rear;
    int  m_len;
    int  m_size = DEFAULT_QUEUE_SIZE + 1;
};

inline int SerialDataQueue::queueDataLen()
{
    return m_len;
}

inline bool SerialDataQueue::isFull()
{
    return (m_len == m_size);
}

inline bool SerialDataQueue::isEmpty()
{
    return (m_front == m_rear);
}

inline char SerialDataQueue::at(int index)
{
    if (index >= 0 && index < m_len) {
        return m_data[(m_front + index) % m_size];
    } else {
        return 0;
    }
}



#endif // SERIALDATAQUEUE_H
