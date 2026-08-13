#pragma once

//#include <QObject>
#include <windows.h>
#include "3rd/PLIN-API/Include/PLinApi.h"
#include "deviceio.h"

class DEVICEIO_EXPORT peakLIN :public DeviceIO
{
    Q_OBJECT
public:
    explicit peakLIN(QObject* parent = nullptr);
    ~peakLIN();
public:
    virtual bool openDevice(const QJsonObject& param) override;
    virtual void sendData(const ushort ch, const uint fream_id, uchar data[8], bool standard = false, int len = 8, Dirction d = Master) override;
    virtual QStringList enumDevice() override;
    virtual void disConnectDev() override;
private:

    void getNewMessage();
private:
    //当前连接状态
    bool m_bConnect;

    // LIN API object
    // Client handle
    HLINCLIENT m_hClient;
    // Hardware handle
    HLINHW m_hHw;
    // LIN Hardware Modus (Master/Slave)
    TLINHardwareMode m_HwMode;
    // Client filter mask
    unsigned __int64 m_lMask;
    // Baudrate Index of Hardware
    WORD m_wBaudrate;
    // Last LIN error
    TLINError m_LastLINErr;
    // Constant value that indicate the mask of the client filter (64bit)
    static const unsigned __int64 FRAME_FILTER_MASK = 0xFFFFFFFFFFFFFFFF;
    bool bRunRead = false;

    QStringList m_devList;
};