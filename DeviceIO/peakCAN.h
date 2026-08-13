#ifndef PCAN_H
#define PCAN_H
#include <array>
#include "./3rd/pcan/include/PCANBasic.h"
#include "deviceio.h"

class DEVICEIO_EXPORT peakCAN : public DeviceIO
{
    Q_OBJECT

public:
    explicit peakCAN(QObject* parent=nullptr);
    ~peakCAN();
protected:
    virtual bool openDevice(const QJsonObject& param) override;
    virtual void sendData(const ushort ch, const uint fream_id, uchar data[8],bool isStandard, int len = 8, Dirction d = Master) override;
    virtual QStringList enumDevice() override;
    virtual void disConnectDev() override;
private:

    void getNewMessage();
private:
    //当前连接状态
    bool m_bConnect;
    //当前CAN的句柄
    TPCANHandle m_hCurHandle;
    //可用的设备索引
    std::vector<TPCANHandle> m_device_io_list;
    //PCAN默认的设备句柄，这里初始化必须C++17及以上才能编译
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
};

#endif // PCAN_H
