
#ifndef ZLGCANMANAGE_H
#define ZLGCANMANAGE_H


#include <QObject>
#include "3rd/zlgcan/zlgcan.h"
#include <QThread>
#include "deviceio.h"
class DEVICEIO_EXPORT zlgCAN : public DeviceIO
{
    Q_OBJECT

public:
    explicit zlgCAN(QObject* parent = nullptr);
    ~zlgCAN();
protected:
    virtual bool openDevice(const QJsonObject& param) override;
    virtual void sendData(const ushort ch, const uint fream_id, uchar data[8], bool standard = false, int len = 8, Dirction d = Master) override;
    virtual QStringList enumDevice() override;
    virtual void disConnectDev() override;
private:
    void getNewMessage();
private:
    //当前连接状态
    bool m_bConnect;

    DEVICE_HANDLE device_handle_;
    CHANNEL_HANDLE channel_handle_;
    int m_channel;
    bool b_open_;
};


#endif // ZLGCANMANAGE_H
