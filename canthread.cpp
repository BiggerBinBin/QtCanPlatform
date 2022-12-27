#include "canthread.h"
#include "ControlCAN.h"
#include "config.h"
//#include "ControlCAN.h"

#include <QTime>
#include <QCoreApplication>
#include <QMetaType>
#include <string.h>
#include <QDateTime>
#include "QsLog.h"
#pragma execution_character_set("utf-8")
VCI_BOARD_INFO vbi;
CANThread::CANThread()
{
    stopped = false;
    qRegisterMetaType<VCI_CAN_OBJ>("VCI_CAN_OBJ");
    qRegisterMetaType<DWORD>("DWORD");
}

CANThread::~CANThread()
{
    //delete hex_data;
}

void CANThread::stop()
{
    stopped = true;
}

bool CANThread::openCAN(ushort deviceIndex, ushort deviceChannel)
{
    DWORD nDeviceType = (DWORD)deviceType; /* USBCAN-2A或USBCAN-2C或CANalyst-II */
    debicIndex = deviceIndex;
    DWORD nDeviceInd = (DWORD)debicIndex; /* 第1个设备 */
    debicCom = deviceChannel;
    DWORD nCANInd = (DWORD)debicCom; /* 这个是保留的参数，不是通道的，无意义 */

    DWORD dwRel;

    dwRel = VCI_OpenDevice(nDeviceType, nDeviceInd, nCANInd);
    if(dwRel != 1U)
    {
        QLOG_WARN() << "open fail:";
        return false;
    }
    else
    {
        qDebug()<<"open success";
    }
    dwRel = VCI_ClearBuffer(nDeviceType, nDeviceInd, nCANInd);
    VCI_INIT_CONFIG vic;
    vic.AccCode=0x80000008U;
    vic.AccMask=0xFFFFFFFFU;
    vic.Filter=1U;
    vic.Mode=0U;
    switch (baundRate) {
    case 10:
        vic.Timing0=0x31U;
        vic.Timing1=0x1cU;
        break;
    case 20:
        vic.Timing0=0x18U;
        vic.Timing1=0x1cU;
        break;
    case 40:
        vic.Timing0=0x87U;
        vic.Timing1=0xffU;
        break;
    case 50:
        vic.Timing0=0x09U;
        vic.Timing1=0x1cU;
        break;
    case 80:
        vic.Timing0=0x83U;
        vic.Timing1=0xffU;
        break;
    case 100:
        vic.Timing0=0x04U;
        vic.Timing1=0x1cU;
        break;
    case 125:
        vic.Timing0=0x03U;
        vic.Timing1=0x1cU;
        break;
    case 200:
        vic.Timing0=0x81U;
        vic.Timing1=0xfaU;
        break;
    case 250:
        vic.Timing0=0x01U;
        vic.Timing1=0x1cU;
        break;
    case 400:
        vic.Timing0=0x80U;
        vic.Timing1=0xfaU;
        break;
    case 500:
        vic.Timing0=0x00U;
        vic.Timing1=0x1cU;
        break;
    case 666:
        vic.Timing0=0x80U;
        vic.Timing1=0xb6U;
        break;
    case 800:
        vic.Timing0=0x00U;
        vic.Timing1=0x16U;
        break;
    case 1000:
        vic.Timing0=0x00U;
        vic.Timing1=0x14U;
        break;
    case 33:
        vic.Timing0=0x09U;
        vic.Timing1=0x6fU;
        break;
    case 66:
        vic.Timing0=0x04U;
        vic.Timing1=0x6fU;
        break;
    case 83:
        vic.Timing0=0x03U;
        vic.Timing1=0x6fU;
        break;
    default:
        vic.Timing0=0x03U;
        vic.Timing1=0x6fU;
        break;
    }
    dwRel = VCI_InitCAN(nDeviceType, nDeviceInd, nCANInd, &vic);
    if(dwRel !=1U)
    {
        qDebug()<<"init fail:";
        return false;
    }
    else
    {
        qDebug()<<"init success";
    }

    dwRel = VCI_ReadBoardInfo((DWORD)nDeviceType, (DWORD)nDeviceInd, &vbi);
    if(dwRel != 1U)
    {
        qDebug()<<"get dev message fail:";
        return false;
    }
    else
    {
        qDebug()<<"CAN通道数："<<vbi.can_Num;
        qDebug()<<"硬件版本号:"<<vbi.hw_Version;
        qDebug()<<"接口库版本号："<<vbi.in_Version;
        qDebug()<<"中断号"<<vbi.irq_Num;
    }

    if(VCI_StartCAN(nDeviceType, nDeviceInd, nCANInd) !=1U)
    {
        return false;
    }
    else
    {
        qDebug()<<"start"<<nCANInd<<"success:";
    }
    return true;
}
bool CANThread::openCANAll(int bundRate)
{

   
    if (i_CAN_NUM <= 0)
    {
        QLOG_WARN() << "CANayst Not finded!";
        return false;
    }
    //CAN_NUM = 2;

    DWORD dwRel;
    for(int j=0;j<i_DEVICE_NUM;j++)
        for (int i = 0; i < i_CAN_NUM; i++)
        {
            dwRel = VCI_OpenDevice(deviceType, j, i);
            if (dwRel != 1U)
            {
                QLOG_WARN() << "Open fail Canayst:"<<QString::number(i)<<"channel";
                return false;
            }
            else
            {
                QLOG_WARN() << "Open Sucessful Canayst:" << QString::number(i) << "channel";
            }
            dwRel = VCI_ClearBuffer(deviceType, j, i);
            VCI_INIT_CONFIG vic;
            vic.AccCode = 0x80000008U;
            vic.AccMask = 0xFFFFFFFFU;
            vic.Filter = 1U;
            vic.Mode = 0U;
            baundRate = bundRate;
            switch (baundRate) {
            case 10:
                vic.Timing0 = 0x31U;
                vic.Timing1 = 0x1cU;
                break;
            case 20:
                vic.Timing0 = 0x18U;
                vic.Timing1 = 0x1cU;
                break;
            case 40:
                vic.Timing0 = 0x87U;
                vic.Timing1 = 0xffU;
                break;
            case 50:
                vic.Timing0 = 0x09U;
                vic.Timing1 = 0x1cU;
                break;
            case 80:
                vic.Timing0 = 0x83U;
                vic.Timing1 = 0xffU;
                break;
            case 100:
                vic.Timing0 = 0x04U;
                vic.Timing1 = 0x1cU;
                break;
            case 125:
                vic.Timing0 = 0x03U;
                vic.Timing1 = 0x1cU;
                break;
            case 200:
                vic.Timing0 = 0x81U;
                vic.Timing1 = 0xfaU;
                break;
            case 250:
                vic.Timing0 = 0x01U;
                vic.Timing1 = 0x1cU;
                break;
            case 400:
                vic.Timing0 = 0x80U;
                vic.Timing1 = 0xfaU;
                break;
            case 500:
                vic.Timing0 = 0x00U;
                vic.Timing1 = 0x1cU;
                break;
            case 666:
                vic.Timing0 = 0x80U;
                vic.Timing1 = 0xb6U;
                break;
            case 800:
                vic.Timing0 = 0x00U;
                vic.Timing1 = 0x16U;
                break;
            case 1000:
                vic.Timing0 = 0x00U;
                vic.Timing1 = 0x14U;
                break;
            case 33:
                vic.Timing0 = 0x09U;
                vic.Timing1 = 0x6fU;
                break;
            case 66:
                vic.Timing0 = 0x04U;
                vic.Timing1 = 0x6fU;
                break;
            case 83:
                vic.Timing0 = 0x03U;
                vic.Timing1 = 0x6fU;
                break;
            default:
                vic.Timing0 = 0x03U;
                vic.Timing1 = 0x6fU;
                break;
            }
            dwRel = VCI_InitCAN(deviceType, j, i, &vic);
            if (dwRel != 1U)
            {
                QLOG_WARN() << "init fail:" << QString::number(i) << " channel";
                return false;
            }
       

           /* dwRel = VCI_ReadBoardInfo((DWORD)nDeviceType, (DWORD)nDeviceInd, &vbi);
            if (dwRel != 1U)
            {
                qDebug() << "get dev message fail:";
                return false;
            }
            else
            {
                qDebug() << "CAN通道数：" << vbi.can_Num;
                qDebug() << "硬件版本号:" << vbi.hw_Version;
                qDebug() << "接口库版本号：" << vbi.in_Version;
                qDebug() << "中断号" << vbi.irq_Num;
            }*/

            if (VCI_StartCAN(deviceType, j, i) != 1U)
            {
                QLOG_WARN() << "VCI_StartCAN fail";
                return false;
            }
       
        }
    stopped = false;
    this->start();
    return true;
   
}
void CANThread::closeCAN()
{
    for(int i=0;i<i_DEVICE_NUM;i++)
        VCI_CloseDevice((DWORD)VCI_USBCAN2A, (DWORD)i);
    QLOG_INFO() << "关闭CANayst";
}

void CANThread::sendData(UINT ID, uchar qbt[])
{
    DWORD dwRel;
    VCI_CAN_OBJ vco[1];

    for(int i = 0; i < 8; i++)
    {
        vco->Data[i] = qbt[i];
    }

    int count = 1;
    vco->ID = ID;
    vco->DataLen = 8;
    vco->RemoteFlag = 0U;
    vco->ExternFlag = 1U;
    for(int j=0;j<i_DEVICE_NUM;j++)
    for (int i = 0; i < i_CAN_NUM; i++)
    {
        dwRel = VCI_Transmit(deviceType, j, i, vco, count);
        if (dwRel <= 0U)
        {
            QLOG_INFO()<<"CANayst Send fail:"<< QString::number(dwRel);
        }
        msleep(5);
    }
   
}
void CANThread::sendData(UINT ID, uchar qbt[],bool bStandard)
{
    DWORD dwRel;
    VCI_CAN_OBJ vco[1];

    for (int i = 0; i < 8; i++)
    {
        vco->Data[i] = qbt[i];
    }

    int count = 1;
    vco->ID = ID;
    vco->DataLen = 8;
    vco->RemoteFlag = 0U;
    vco->ExternFlag = (!bStandard);
    for (int j = 0; j < i_DEVICE_NUM; j++)
        for (int i = 0; i < i_CAN_NUM; i++)
        {
            dwRel = VCI_Transmit(deviceType, j, i, vco, count);
            if (dwRel <= 0U)
            {
                QLOG_INFO() << "CANayst Send fail:" << QString::number(dwRel);
            }
            msleep(5);
        }

}

void CANThread::run()
{
    while(!stopped)
    {
        int dwRel;
        int Common = 0;
        VCI_CAN_OBJ vco[2500];
        for(int j=0;j<i_DEVICE_NUM;j++)
            for (int ch = 0; ch < i_CAN_NUM; ch++)
            {
                memset(vco, 0, sizeof(VCI_CAN_OBJ) * 2500);
                dwRel = VCI_Receive(deviceType, j, ch, vco,2500,0);
                if(dwRel > 0)
                {
                    //for(int i = 0; i < dwRel; i++)
                    {
                        QByteArray data;
                        for (int i = 0; i < vco->DataLen; i++)
                        {
                            data.append(vco->Data[i]);
                        }
                        emit getProtocolData(i_DEVICE_NUM*j+ch, vco->ID,data);
                    }
                }
                else if(dwRel == -1)
                {
                    qDebug()<<"设备不存在或USB掉线";
                    stop();
                    closeCAN();
                }
                msleep(5);
            }
        msleep(30);
        
        
    }
    stopped = false;
}





void CANThread::sleep(unsigned int msec)
{
   QTime dieTime = QTime::currentTime().addMSecs(msec);
   while( QTime::currentTime() < dieTime )
       QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
QStringList CANThread::DetectDevice()
{
    VCI_BOARD_INFO pInfo[50];
    int num = VCI_FindUsbDevice2(pInfo);
    i_DEVICE_NUM = num;
    QStringList canlist;
    for (int i = 0; i < num; i++)
    {
        for(int n=0;n<pInfo[i].can_Num;n++)
            canlist.append(QString::number(i_DEVICE_NUM*i+n));
    }
    
    return canlist;
}