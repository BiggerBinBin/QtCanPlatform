#ifndef COMMUNICATE_H
#define COMMUNICATE_H

#include <QObject>
#include <QThread>
#include <QStringList>
#include <QSharedPointer>
#include "GlobalDataStruct.h"
#include "../DeviceIO/deviceio.h"
class Communicate : public QObject
{
    Q_OBJECT
public:
    explicit Communicate(QObject *parent = nullptr);
    QStringList setDevType(short int devtype);
    //Using this function to open CAN or LIN Devices
    bool openDevice(const DeviceParameter &param);
    QStringList devTypeList(){return m_deviceName;}
    QStringList devList();
    void closeDevice();
    void sendData(const ushort ch, const uint fream_id, uchar data[8], bool isStandard, int len = 8, int dir = 0);
private Q_SLOTS:
    void on_recFromDevMsg(int ch, unsigned int fream_id, QByteArray data);
Q_SIGNALS:
    void on_sendNewMsg(int ch, unsigned int fream_id, QByteArray data);
private:
    bool readDevName(QString path);

private:
    //QThread *runReceiveMes=nullptr;
    QSharedPointer<DeviceIO>m_sharedPtrDev;
    QStringList m_className = { "peakCAN","CANalyst","zlgCAN","zlgCAN","peakLIN","Kvaser","ZMLIN","HardWareTcan","HardWareTlin"};
    QStringList m_deviceName = { "peakCAN","CANalyst","ZLG-2EU","ZLG-CAN-II","peakLIN","Kvaser","ZMLIN","TomossCAN","TomossLIN" };
    QStringList m_devList;
};

#endif // COMMUNICATE_H
