#include <QtConcurrent>
#include <QJsonObject>
#include "peakCAN.h"

peakCAN::peakCAN(QObject* parent)
    : DeviceIO(parent)
{
    m_bConnect = false;
    //CAN设备
    dev_type = this->CAN;

}

peakCAN::~peakCAN()
{
    m_bConnect = false;
    QThread::msleep(15);
    disConnectDev();
}
bool peakCAN::openDevice(const QJsonObject& param)
{
    if (param.size() < 2)
    {
        return false;
    }
    int ch = param["ch"].toInt();
    int bundRate = param["bundRate"].toInt();
    uint bundr;
    switch (bundRate)
    {
    case 0:
        bundr = PCAN_BAUD_125K; break;
    case 1:
        bundr = PCAN_BAUD_250K; break;
    case 2:
        bundr = PCAN_BAUD_500K; break;
    case 3:
        bundr = PCAN_BAUD_800K; break;
    case 4:
        bundr = PCAN_BAUD_1M; break;
    default:
        bundr = PCAN_BAUD_250K; break;
        break;
    }
    if (ch < m_device_io_list.size())
    {
        m_hCurHandle = m_device_io_list.at(ch);
        TPCANStatus ret = CAN_Initialize(m_hCurHandle, bundr);
        if (ret != PCAN_ERROR_OK)
        {
            m_bConnect = false;
            return false;
        }
        m_bConnect = true;
        QtConcurrent::run(this, &peakCAN::getNewMessage);
        return true;
    }
    return false;
}

void peakCAN::sendData(const ushort ch, const uint fream_id, uchar data[8], bool isStandard,int len, Dirction d)
{
    tagTPCANMsg msg;
    memcpy(msg.DATA, data, 8);
    msg.ID = fream_id;
    msg.LEN = 8;
    if (!isStandard)
        msg.MSGTYPE = PCAN_MESSAGE_EXTENDED;	//扩展帧
    else
        msg.MSGTYPE = PCAN_MESSAGE_STANDARD;	//标准帧
    CAN_Write(m_hCurHandle, &msg);
}

QStringList peakCAN::enumDevice()
{
    QStringList devList;
    int iBuffer;
    TPCANStatus stsResult;
    m_device_io_list.clear();
    //不要虚拟的设备，只要USB及网口的PCAN
    for (int i = 25; i < this->m_HandlesArray.size(); i++)
    {
        stsResult = CAN_GetValue((TPCANHandle)m_HandlesArray[i], PCAN_CHANNEL_CONDITION, (void*)&iBuffer, sizeof(iBuffer));
        if (((stsResult) == PCAN_ERROR_OK) && ((iBuffer & PCAN_CHANNEL_AVAILABLE) == PCAN_CHANNEL_AVAILABLE))
        {
            m_device_io_list.push_back(m_HandlesArray[i]);
            QString str = "PCAN-" + QString::number(devList.size());
            devList.append(str);
        }
    }
    return devList;
}

void peakCAN::disConnectDev()
{
    m_bConnect = false;
    CAN_Uninitialize(m_hCurHandle);
}

void peakCAN::getNewMessage()
{
    TPCANMsg msg;
    TPCANTimestamp tp;
    while (m_bConnect)
    {
        if (PCAN_ERROR_OK == CAN_Read(m_hCurHandle, &msg, &tp))
        {
            if (msg.LEN<8)
            {
                QThread::msleep(1);
                continue;
            }
            QByteArray by((char*)msg.DATA,msg.LEN);// = QByteArray::fromRawData((char*)msg.DATA, msg.LEN);
            /*for (int i = 0; i < msg.LEN; i++)
            {
                by.append(msg.DATA[i]);
            }*/

            emit sigNewMessage(m_hCurHandle - 0x51, msg.ID, by);
            //qDebug() << msg.DATA;
        }
        QThread::msleep(1);
    }
}

