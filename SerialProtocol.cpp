#include "SerialProtocol.h"

SerialProtocol::SerialProtocol(QObject *parent) : QObject(parent)
{
    recvDataQueue = new SerialDataQueue(128);
}

void SerialProtocol::sendDevCmd(int devId, int cmd, int data)
{
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

    emit writeToSerial(sendBuff, 9);
}

int SerialProtocol::getMotorPos(int devId)
{
    if (devId < CHIP_MOTOR_X || devId > FILTER_MOTOR)
        return 0;
    return motorPos[devId-1];
}

int SerialProtocol::getMotorLimitStatus(int devId)
{
    if (devId < CHIP_MOTOR_X || devId > FILTER_MOTOR)
        return 0;
    return motorLimitState[devId-1];
}

int SerialProtocol::getLedState()
{
    return ledState;
}

int SerialProtocol::getChipState()
{
    return chipState;
}

int char2int(char a, char b, char c, char d)
{
    int val = (int)a & 0x000000FF;
    val |= ((int)b << 8) & 0x0000FF00;
    val |= ((int)c << 16) & 0x00FF0000;
    val |= ((int)d << 24) & 0xFF000000;
    return val;
}

void SerialProtocol::onSerialRecvData(const char* data, int len)
{
    recvDataQueue->enQueueData(data, len);

    if (recvDataQueue->queueDataLen() >= recvFrameLen) {
        if (recvDataQueue->at(0) == 0x58 && recvDataQueue->at(1) == 0x59) {
            int32_t sum = 0;
            for (int i = 2; i < recvFrameLen-1; i++) {
                sum += recvDataQueue->at(i);
            }
            if ((char)(sum & 0x000000ff) == recvDataQueue->at(recvFrameLen-1)) {
                for (int j = 0; j < 4; j++) {
                    motorPos[j] = char2int(recvDataQueue->at(2+j*5), recvDataQueue->at(3+j*5),
                                           recvDataQueue->at(4+j*5), recvDataQueue->at(5+j*5));
                    motorLimitState[j] = (int)(recvDataQueue->at(6+j*5));
                }
                ledState = (int)(recvDataQueue->at(22));
                chipState = (int)(recvDataQueue->at(23));
            }
            emit updateDevStatus();
        }
        recvDataQueue->deQueueData(recvFrameLen);
    }
}



