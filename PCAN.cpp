#include <array>
#include <QtConcurrent>
#include "PCAN.h"
#include "QsLog.h"
#include "./PCAN-Basic-API/Include/PCANBasic.h"
static constexpr  std::array m_HandlesArray = {
            PCAN_ISABUS1, PCAN_ISABUS2, PCAN_ISABUS3, PCAN_ISABUS4, PCAN_ISABUS5,
            PCAN_ISABUS6, PCAN_ISABUS7, PCAN_ISABUS8,
            PCAN_DNGBUS1, PCAN_PCIBUS1, PCAN_PCIBUS2, PCAN_PCIBUS3, PCAN_PCIBUS4,
            PCAN_PCIBUS5, PCAN_PCIBUS6, PCAN_PCIBUS7, PCAN_PCIBUS8, PCAN_PCIBUS9,
            PCAN_PCIBUS10, PCAN_PCIBUS11, PCAN_PCIBUS12, PCAN_PCIBUS13, PCAN_PCIBUS14,
            PCAN_PCIBUS15, PCAN_PCIBUS16,
            PCAN_USBBUS1, PCAN_USBBUS2, PCAN_USBBUS3, PCAN_USBBUS4, PCAN_USBBUS5, PCAN_USBBUS6,
            PCAN_USBBUS7, PCAN_USBBUS8, PCAN_USBBUS9, PCAN_USBBUS10, PCAN_USBBUS11, PCAN_USBBUS12,
            PCAN_USBBUS13, PCAN_USBBUS14, PCAN_USBBUS15, PCAN_USBBUS16,
            PCAN_PCCBUS1, PCAN_PCCBUS2,
            PCAN_LANBUS1, PCAN_LANBUS2, PCAN_LANBUS3, PCAN_LANBUS4, PCAN_LANBUS5, PCAN_LANBUS6,
            PCAN_LANBUS7, PCAN_LANBUS8, PCAN_LANBUS9, PCAN_LANBUS10, PCAN_LANBUS11, PCAN_LANBUS12,
            PCAN_LANBUS13, PCAN_LANBUS14, PCAN_LANBUS15, PCAN_LANBUS16 };
PCAN::PCAN(QObject *parent)
	: QThread(parent)
{
    m_bConnect = false;
}

PCAN::~PCAN()
{
    CloseCan();
}

QStringList PCAN::DetectDevice()
{
    QStringList devList;;

    int iBuffer;
    TPCANStatus stsResult;
    m_device_io_list.clear();
    //不要虚拟的设备，只要USB及网口的PCAN
    for (int i = 25; i < m_HandlesArray.size(); i++)
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

   /* m_interfaces = QCanBus::instance()->availableDevices(plugin);
    if (!m_interfaces.empty())
    {
        for (int i = 0; i < m_interfaces.count(); i++) {
            num.append(m_interfaces.at(i).name());
            qDebug() << m_interfaces.at(i).name();
        }
        return num;
    }
    else
    {
        qDebug() << "do not find available interface for pcan";
        return num;
    }*/
}


bool PCAN::ConnectDevice(int usb_index, int bitrate)
{

    uint bundr;
    switch (bitrate)
    {
    case 250:
        bundr = PCAN_BAUD_250K; break;
    case 500:
        bundr = PCAN_BAUD_500K; break;
    case 125:
        bundr = PCAN_BAUD_125K; break;
    default:
        bundr = PCAN_BAUD_250K; break;
        break;
    }
    if (usb_index < m_device_io_list.size())
    {
        m_hCurHandle = m_device_io_list.at(usb_index);
        TPCANStatus ret = CAN_Initialize(m_hCurHandle, bundr);
        if (ret != PCAN_ERROR_OK)
        {
            m_bConnect = false;
            return false;
        }
        m_bConnect = true;
        this->start();
        return true;
    }
    return false;
}
//bool PCAN::ConnectDevice(QString usb_name, int bitrate)
//{
//    if (m_interfaces.empty())
//        return false;
//    bitrate = bitrate * 1000;
//    bool connectflag = false;
//    try
//    {
//        m_canDevice = QCanBus::instance()->createDevice(plugin, usb_name, &errorString);
//        if (!m_canDevice)
//        {
//            QLOG_WARN() << "Connection failed: " << usb_name;
//            QLOG_WARN() << "Err: " << errorString;
//            isOpen = false;
//            return false;
//        }
//        m_canDevice->setConfigurationParameter(QCanBusDevice::BitRateKey, QVariant(bitrate));
//        connectflag = m_canDevice->connectDevice();
//        if (!connectflag) {
//            QLOG_WARN() << "Connection failed: " << usb_name;
//            QLOG_WARN() << "Err: " << errorString;
//            isOpen = false;
//            return false;
//        }
//    }
//    catch (const std::exception& e)
//    {
//        QLOG_WARN() << "Connection failed: " << usb_name;
//        QLOG_WARN() << "Err: " << e.what();
//        isOpen = false;
//        isReceive = false;
//        return false;
//    }
//    isOpen = true;
//    isReceive = true;
//    this->start();
//    return true;
//}

void PCAN::SendFrame(int FrameID, uchar Data[])
{
    tagTPCANMsg msg;
    memcpy(msg.DATA, Data, 8);
    msg.ID = FrameID;
    msg.LEN = 8;
    if (FrameID>0x7ff)
        msg.MSGTYPE = PCAN_MESSAGE_EXTENDED;	//扩展帧
    else
        msg.MSGTYPE = PCAN_MESSAGE_STANDARD;	//标准帧
    CAN_Write(m_hCurHandle, &msg);
}
void PCAN::SendFrame(int FrameID, uchar Data[],bool bStandard)
{

    tagTPCANMsg msg;
    memcpy(msg.DATA, Data, 8);
    msg.ID = FrameID;
    msg.LEN = 8;
    if (!bStandard)
        msg.MSGTYPE = PCAN_MESSAGE_EXTENDED;	//扩展帧
    else
        msg.MSGTYPE = PCAN_MESSAGE_STANDARD;	//标准帧
    CAN_Write(m_hCurHandle, &msg);

    //if (!m_canDevice)
    //    return;


    //QCanBusFrame frame;
    //frame.setExtendedFrameFormat(!bStandard);
    //frame.setFrameId(FrameID);


    //QByteArray payload;
    //for (int i = 0; i < 8; i++)
    //{
    //    payload.append(Data[i]);
    //    //        qDebug()<<Data[i];
    //}
    //frame.setPayload(payload);
    ////    qDebug()<<"打印输出"<<payload;
    //m_canDevice->writeFrame(frame);
    //m_canDevice->waitForFramesWritten(100);
}
void PCAN::CloseCan()
{
    m_bConnect = false;
    QThread::msleep(10);
    if (isRunning()) {
        
        this->quit();
    }
    CAN_Uninitialize(m_hCurHandle);
}


void PCAN::run()
{
    TPCANMsg msg;
    TPCANTimestamp tp;
    while (m_bConnect)
    {
        if (PCAN_ERROR_OK == CAN_Read(m_hCurHandle, &msg, &tp))
        {
            static QByteArray by;
            by.clear();
            for (int i = 0; i < msg.LEN; i++)
                by.append(msg.DATA[i]);
            emit getProtocolData(msg.ID, by);
            
            //qDebug() << msg.DATA;
        }
        QThread::msleep(5);
    }
   /* while (isReceive)
    {
        Receive();
        msleep(50);
    }*/

}

void PCAN::disConnectDev()
{
    m_bConnect = false;
    CAN_Uninitialize(m_hCurHandle);
}
bool PCAN::setDeviceList(const  std::vector<ushort>& list)
{
    m_device_io_list.clear();
    for (auto x : list)
        m_device_io_list.push_back(x);
    return true;
}