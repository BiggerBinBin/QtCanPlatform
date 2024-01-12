
#ifndef ZLGCANMANAGE_H
#define ZLGCANMANAGE_H


#include <QObject>
#include "zlgcan.h"
#include <QThread>

typedef struct _DeviceInfo
{
    UINT device_type;  //设备类型
    UINT channel_count;//设备的通道个数
}DeviceInfo;
/*
列表数据需要和对话框中设备列表数据一一对应
*/
static const DeviceInfo kDeviceType[] = {
    {ZCAN_USBCAN1, 1},
    {ZCAN_USBCAN2, 2},
    {ZCAN_PCI9820I,2},
    {ZCAN_USBCAN_E_U, 1},
    {ZCAN_USBCAN_2E_U, 2},
    {ZCAN_USBCAN_4E_U, 4},
    {ZCAN_PCIE_CANFD_100U, 1},
    {ZCAN_PCIE_CANFD_200U, 2},
    {ZCAN_PCIE_CANFD_400U_EX, 4 },
    {ZCAN_USBCANFD_200U, 2},
    {ZCAN_USBCANFD_100U, 1},
    {ZCAN_USBCANFD_MINI, 1},
    {ZCAN_CANETTCP, 1},
    {ZCAN_CANETUDP, 1},
    {ZCAN_WIFICAN_TCP, 1},
    {ZCAN_WIFICAN_UDP, 1},
    {ZCAN_CLOUD, 1},
    {ZCAN_CANFDWIFI_TCP, 1},
    {ZCAN_CANFDWIFI_UDP, 1},
    {ZCAN_CANFDNET_TCP, 2},
    {ZCAN_CANFDNET_UDP, 2},
    {ZCAN_CANFDNET_400U_TCP, 4},
    {ZCAN_CANFDNET_400U_UDP, 4},
    };
class zlgcanmanage : public QThread
{
    Q_OBJECT
public:
    explicit zlgcanmanage(QThread *parent = nullptr);
    bool openCAN(int devType,int chnanel,uint bundrate);
    void closeCAN();
    void sendData(UINT ID,uchar qbt[]);
protected:
    void my_run();
    bool start_;
signals:
    void getProtocolData2(QByteArray data,int common,quint32 ID);
    
private:
    DEVICE_HANDLE device_handle_;
    CHANNEL_HANDLE channel_handle_;
    bool b_open_;
};

#endif // ZLGCANMANAGE_H
