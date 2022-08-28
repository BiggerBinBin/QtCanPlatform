#pragma once
#include <QObject>
#include <QtSerialBus>
#include <QMetaType>
#include <QThread>

class PCAN  : public QThread
{
	Q_OBJECT

public:
	PCAN(QObject *parent);
	~PCAN();
    QStringList DetectDevice();
    bool ConnectDevice(int usb_index, int bitrate);
    void Receive();
    void SendFrame(int FrameID, uchar Data[]);
    void CloseCan();
    bool isReceive;
    QList<QCanBusDeviceInfo> m_interfaces;
    QCanBusDevice* m_canDevice = nullptr;
    const QString plugin = "peakcan";
    QString errorString;
private:
    void run();
signals:
    void getProtocolData(uint frame_id, QByteArray data);
};
