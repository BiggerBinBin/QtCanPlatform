#include "PCAN.h"

PCAN::PCAN(QObject *parent)
	: QThread(parent)
{
    isReceive = false;
}

PCAN::~PCAN()
{}

QStringList PCAN::DetectDevice()
{
    QStringList num;
    m_interfaces = QCanBus::instance()->availableDevices(plugin);
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
    }
}


bool PCAN::ConnectDevice(int usb_index, int bitrate)
{
    if (m_interfaces.empty())
        return false;
    bitrate = bitrate * 1000;
    bool connectflag = false;
    m_canDevice = QCanBus::instance()->createDevice(plugin, m_interfaces.at(usb_index).name(), &errorString);
    m_canDevice->setConfigurationParameter(QCanBusDevice::BitRateKey, QVariant(bitrate));
    connectflag = m_canDevice->connectDevice();
    if (!connectflag) {
        qDebug() << "Connection failed!";
        return false;
    }
    isReceive = true;
    this->start();
    return true;
}


void PCAN::Receive()
{


    if (!m_canDevice)
        return;
    while (m_canDevice->framesAvailable())
    {
        const QCanBusFrame frame = m_canDevice->readFrame();


        QString view;
        if (frame.frameType() == QCanBusFrame::ErrorFrame)
        {
            view = m_canDevice->interpretErrorFrame(frame);
        }
        else if (frame.frameType() == QCanBusFrame::DataFrame)
        {
            view = frame.toString();
            /*uchar data[frame.payload().length()];
            memcpy(&data, frame.payload().data(), sizeof(data));*/
            emit getProtocolData(frame.frameId(), frame.payload());
        }
        //        qDebug()<<"接收："<<view;
    }
}


void PCAN::SendFrame(int FrameID, uchar Data[])
{
    if (!m_canDevice)
        return;


    QCanBusFrame frame;
    frame.setFrameId(FrameID);


    QByteArray payload;
    for (int i = 0; i < 8; i++)
    {
        payload.append(Data[i]);
        //        qDebug()<<Data[i];
    }
    frame.setPayload(payload);
    //    qDebug()<<"打印输出"<<payload;
    m_canDevice->writeFrame(frame);
}
void PCAN::CloseCan()
{
    isReceive = false;
    if (isRunning()) {
        m_canDevice->disconnectDevice();
        this->quit();
    }
}


void PCAN::run()
{
    while (isReceive)
    {
        Receive();
        msleep(50);
    }
}