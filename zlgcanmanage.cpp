
#include "zlgcanmanage.h"
#include <QDebug>
#include <QTime>
#include <QtConcurrent>
zlgcanmanage::zlgcanmanage(QThread *parent):QThread(parent)
{

}

bool zlgcanmanage::openCAN(int devType,int chnanel,uint bundrate)
{
    if(devType==4)
    {
        device_handle_ = ZCAN_OpenDevice(ZCAN_USBCAN2,0,0);
    }
    else if(devType==3)
    {
        device_handle_ = ZCAN_OpenDevice(ZCAN_USBCAN_2E_U,0,0);
    }
    if (INVALID_DEVICE_HANDLE == device_handle_)
    {
        return false;
    }
    QString path=QString::number(chnanel)+"/baud_rate";
    QString value=QString::number(bundrate*1000);
    int res = ZCAN_SetValue(device_handle_, path.toStdString().c_str(), value.toStdString().c_str());
    if(1 != res)
    {
        closeCAN();
        return false;
    }

    ZCAN_CHANNEL_INIT_CONFIG config;
    memset(&config, 0, sizeof(config));
    config.can_type = TYPE_CAN;
    config.can.mode = 0;                    //0是正常模式，1是只听模式
    config.can.filter = 1;
    config.can.acc_code = 0x00000000;
    config.can.acc_mask = 0xFFFFFFFF;
    channel_handle_ = ZCAN_InitCAN(device_handle_, chnanel, &config);

    if(INVALID_CHANNEL_HANDLE == channel_handle_)
    {
        closeCAN();
        return false;
    }

    if (ZCAN_StartCAN(channel_handle_) != STATUS_OK)
    {
        closeCAN();
        return false;
    }
    ZCAN_ClearBuffer(channel_handle_);
    start_ = true;
    QtConcurrent::run(this,&zlgcanmanage::my_run);
    return true;

}
void zlgcanmanage::closeCAN()
{
    start_ = false;
    this->quit();
    while(this->isRunning()) QThread::usleep(10);
    ZCAN_CloseDevice(device_handle_);
}
void zlgcanmanage::sendData(UINT ID,uchar qbt[])
{

    tagZCAN_Transmit_Data send;
    memcpy(send.frame.data,qbt,sizeof(uchar)*8);
    send.frame.can_dlc = 8;
    send.frame.can_id = ID+0x80000000;
//    if(ID>0x7ff)
//        send.transmit_type = 1;
//    else
    send.transmit_type = 0;
    ZCAN_Transmit(channel_handle_,&send,1);

    /*QString temp;
    temp.append("Tx Id:"+QString::number(ID,16));
    temp.append(" 0x"+QString::number(qbt[0],16));
    temp.append(" 0x"+QString::number(qbt[1],16));
    temp.append(" 0x"+QString::number(qbt[2],16));
    temp.append(" 0x"+QString::number(qbt[3],16));
    temp.append(" 0x"+QString::number(qbt[4],16));
    temp.append(" 0x"+QString::number(qbt[5],16));
    temp.append(" 0x"+QString::number(qbt[6],16));
    temp.append(" 0x"+QString::number(qbt[7],16));

    qDebug()<<QTime::currentTime().toString("hh:mm:ss:zzz")+"  "<<temp;*/
}

void zlgcanmanage::my_run()
{
    ZCAN_Receive_Data can_data[100];
    ZCAN_ReceiveFD_Data canfd_data[100];
    UINT len;
    QByteArray data;
    QString temp;
    ZCAN_ClearBuffer(channel_handle_);
    while(start_)
    {
        //temp.clear();
        if (len = ZCAN_GetReceiveNum(channel_handle_, TYPE_CAN))
        {
            len = ZCAN_Receive(channel_handle_, can_data, 100, 50);
            QByteArray data;
            //temp.append("Rx ID: "+QString::number(can_data[0].frame.can_id-0x80000000,16));
            for(int mk=0;mk<len;mk++)
            {
                for(int m=0;m<can_data[mk].frame.can_dlc;m++)
                {
                    data.append(can_data[mk].frame.data[m]);
                    //temp.append("  0x"+QString::number(can_data[0].frame.data[m],16));
                }
                emit getProtocolData2(data,0,can_data[0].frame.can_id-0x80000000);
               // qDebug()<<QTime::currentTime().toString("hh:mm:ss:zzz")+"  "<<temp;
            }
        }

        //避免无数据时变成While(1),会占用大量的CPU
        this->msleep(5);
    }
}
