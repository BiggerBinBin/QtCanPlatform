#ifndef CANTHREAD_H
#define CANTHREAD_H

#include <QThread>
#include <QDebug>


#include "config.h"

#include "ControlCAN.h"
#include "conversion_number.h"
#include "chex.h"
#include <qstringlist.h>
class CANThread:public QThread
{
    Q_OBJECT
public:
    CANThread();

    ~CANThread();

    void stop();
    QStringList DetectDevice();
    bool openCAN(ushort debicIndex, ushort deviceChannel);
    bool openCANAll(int bundRate);

    void closeCAN();

    void sendData(UINT ID, uchar qbt[]);

    //void sendData(int ID,QByteArray qbt,bool check);

    void DataManage();

    void SetStartAddr(int line);

    void CheckAPP(UINT ID,int Addr);

    int deviceType=4;
    int debicIndex=0;
    int baundRate=250;
    int debicCom=0;
    bool stopped;
    UINT Send_ID;
    //QByteArray *hex_data = new QByteArray();
    int hex_data_length;
    CHex chex;
//    int Counter_hex = 1;
    int Counter_hex = 0;
    int Counter_hex_data = 0;

    qint64 currentTime,preTime;
    int i_CAN_NUM = 2;
    int i_DEVICE_NUM = 1;
signals:
    void getProtocolData(VCI_CAN_OBJ *vci,DWORD dwRel,int common);
    void getProtocolData(int ch, quint32 ID ,QByteArray data);

private:
    char StartAddr;
    bool Reset = false;

   
    void run();
    void sleep(unsigned int msec);
    
};

#endif // CANTHREAD_H
