#ifndef DEVICEIO_H
#define DEVICEIO_H
#include <QObject>
#include "DeviceIO_global.h"
//#include "../../CANMaster/MainProject/GlobalDataStruct.h"
class DEVICEIO_EXPORT DeviceIO:public QObject
{
    Q_OBJECT
public:
    explicit DeviceIO(QObject* parent = nullptr);
    ~DeviceIO() {};
public:
    enum Dirction{
        Master = 0,
        Slave
    };
    enum DevType
    {
        CAN = 0,
        LIN
    };
    
    virtual bool openDevice(const QJsonObject& param) = 0;
    virtual void sendData(const ushort ch,const uint fream_id,uchar data[8],bool isStandard,int len = 8, Dirction d = Master) = 0;
    virtual QStringList enumDevice() = 0;
    virtual void disConnectDev()=0;
public:
    DevType dev_type;
signals:
    void sigNewMessage(int ch, quint32 fream_id, QByteArray data);
};

#endif // DEVICEIO_H