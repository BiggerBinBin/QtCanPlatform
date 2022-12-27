#include "kvaser.h"
//static void check(char* id, canStatus stat);
void show_channel_info(int channel_no);

// Find out how many channels that exists and loop over them
void list_channels(void)
{
  char driverName[256];
  int chanCount = 0;
  canStatus stat;
  int i;

  stat = canGetNumberOfChannels(&chanCount);
  //check("canGetNumberOfChannels", stat);

  printf("Found %d channel(s).\n", chanCount);
  for (i = 0; i < chanCount; i++) {
    show_channel_info(i);
  }
} // list_channels

// Print status message if stat contains an error code
//static void check(char* id, canStatus stat)
//{
//  if (stat != canOK) {
//    char buf[50];
//    buf[0] = '\0';
//    // Retreive informational text about the status code
//    canGetErrorText(stat, buf, sizeof(buf));
//    printf("%s: failed, stat=%d (%s)\n", id, (int)stat, buf);
//  }
//} // check

void show_channel_info(int channel_no)
{
  canStatus stat;
  char name[256];
  char driverName[256];
  char custChanName[40];
  unsigned int ean[2], fw[2], serial[2];
  unsigned int chan_no_on_card;


  memset(name, 0, sizeof(name));
  memset(custChanName, 0, sizeof(custChanName));
  memset(ean, 0, sizeof(ean));
  memset(fw, 0, sizeof(fw));
  memset(serial, 0, sizeof(serial));

  stat = canGetChannelData(channel_no, canCHANNELDATA_DEVDESCR_ASCII, &name, sizeof(name));
  //check("canGetChannelData: DEVDESCR_ASCII", stat);

  stat = canGetChannelData(channel_no, canCHANNELDATA_CARD_UPC_NO, &ean, sizeof(ean));
  //check("canGetChannelData: CARD_UPC_NO", stat);

  stat = canGetChannelData(channel_no, canCHANNELDATA_CARD_SERIAL_NO, &serial, sizeof(serial));
  //check("canGetChannelData: CARD_SERIAL_NO", stat);

  stat = canGetChannelData(channel_no, canCHANNELDATA_CARD_FIRMWARE_REV, &fw, sizeof(fw));
  //check("canGetChannelData: CARD_FIRMWARE_REV", stat);

  stat = canGetChannelData(channel_no, canCHANNELDATA_CHAN_NO_ON_CARD,
                           &chan_no_on_card, sizeof(chan_no_on_card));
  //check("canGetChannelData: canCHANNELDATA_CHAN_NO_ON_CARD", stat);

  canGetChannelData(channel_no, canCHANNELDATA_CUST_CHANNEL_NAME,
                    custChanName, sizeof(custChanName));

  printf("ch %2.1d: %-22s\t%x-%05x-%05x-%x, s/n %u, v%u.%u.%u :%d \"%s\"\n",
         channel_no, name,
         (ean[1] >> 12), ((ean[1] & 0xfff) << 8) | ((ean[0] >> 24) & 0xff),
         (ean[0] >> 4) & 0xfffff, (ean[0] & 0x0f),
         serial[0],
         fw[1] >> 16, fw[1] & 0xffff, fw[0] & 0xffff,
         chan_no_on_card, custChanName);
} // list_channel

kvaser::kvaser()
{
    stop = true;
    canInitializeLibrary();
    //list_channels();
}

bool kvaser::openCan(int can_chanel, int bus_speed)
{
    h = canOpenChannel(can_chanel,canOPEN_OVERRIDE_EXCLUSIVE);

    if(h >= canOK)
    {
        switch (bus_speed) {
        case 1000:
            canSetBusParams(h,BAUD_1M,0,0,0,0,0);
            break;
        case 500:
            canSetBusParams(h,BAUD_500K,0,0,0,0,0);
            break;
        case 250:
            canSetBusParams(h,BAUD_250K,0,0,0,0,0);
            break;
        case 125:
            canSetBusParams(h,BAUD_125K,0,0,0,0,0);
            break;
        case 100:
            canSetBusParams(h,BAUD_100K,0,0,0,0,0);
            break;
        default:
            canSetBusParams(h,BAUD_500K,0,0,0,0,0);
            break;
        }
        canSetBusOutputControl(h,canDRIVER_NORMAL);
        canBusOn(h);

        stop = false;
        qDebug()<<"open channel"<<can_chanel<<"ok";

        return true;
    }
    else{
        qDebug()<<"open channel"<<can_chanel<<"failed!";
        return false;
    }
}

int *kvaser::openCanAll(int bitRate,QString &msg)
{
        chanCount =0;
        getCanCount();
      //一拖4，加上两个虚拟的，就是6个，把后面两个虚拟的去掉
      if(chanCount>4)
          chanCount=4;
      for(int i=0;i<chanCount;i++)
      {
          kHandle[i]=canOpenChannel(i,canOPEN_OVERRIDE_EXCLUSIVE);
          //返回来的Handle，有可能是0，反正是非负的
          if(kHandle[i]<0)
          {
              qDebug()<<"Channel "<<i<<" open failure!";
              msg.append("Channel "+QString::number(i)+" open failure!");
          }
          else
          {
              switch (bitRate) {
              case 1000:
                  canSetBusParams(kHandle[i],BAUD_1M,0,0,0,0,0);
                  break;
              case 500:
                  canSetBusParams(kHandle[i],BAUD_500K,0,0,0,0,0);
                  break;
              case 250:
                  canSetBusParams(kHandle[i],BAUD_250K,0,0,0,0,0);
                  break;
              case 125:
                  canSetBusParams(kHandle[i],BAUD_125K,0,0,0,0,0);
                  break;
              case 100:
                  canSetBusParams(kHandle[i],BAUD_100K,0,0,0,0,0);
                  break;
              default:
                  canSetBusParams(kHandle[i],BAUD_500K,0,0,0,0,0);
                  break;
              }
              canSetBusOutputControl(kHandle[i],canDRIVER_NORMAL);
              canBusOn(kHandle[i]);
          }
      }
      if(chanCount>0)
      {
          stop=false;
          this->start();
      }

      return kHandle;
}

void kvaser::closeCAN()
{
    stop = true;
    for(int i=0;i<chanCount;i++)
        canClose(kHandle[i]);
}

int kvaser::getCanCount()
{
    canStatus stat;
    stat = canGetNumberOfChannels(&chanCount);
    if(stat!=canOK)
    {
        qDebug()<<"canGetNumbe failure";
        return -1;
    }
    if(chanCount>4)
        chanCount = 4;
    return chanCount;
}

void kvaser::canSend(int frame_id, unsigned int msg[])
{
    qDebug()<<"send";
    unsigned char msg_char[8];
    for(int i=0;i<8;i++)
        msg_char[i] = msg[i];
    qDebug()<<"Kvaser发送数据："<< QString::number(msg[0],16)<<" "<<QString::number(msg[1],16)<<" "<<
                                 QString::number(msg[2],16)<<" "<<QString::number(msg[3],16)<<" "<<
                                 QString::number(msg[4],16)<<" "<<QString::number(msg[5],16)<<" "<<
                                 QString::number(msg[6],16)<<" "<<QString::number(msg[7],16);
    canWrite(h,frame_id,msg_char,8,canMSG_EXT);
}

void kvaser::canSendAll(uint fream_id, uchar data[])
{
    for(int i=0;i<chanCount;i++)
    {
        if(kHandle[i]<0)
            continue;
        canStatus sta =  canWrite(kHandle[i],fream_id,data,8,canMSG_EXT);
        msleep(2);
        if(sta!=canOK)
        {
            qDebug()<<"Channel "<<i<<" send failure";
        }
    }
}
void kvaser::canSendAll(uint fream_id, uchar data[],bool bStandard)
{
    for (int i = 0; i < chanCount; i++)
    {
        if (kHandle[i] < 0)
            continue;
        canStatus sta;
        if (bStandard)
        {
            sta=canWrite(kHandle[i], fream_id, data, 8, canMSG_STD);
        }
        else
        {
            sta=canWrite(kHandle[i], fream_id, data, 8, canMSG_EXT);
        }
            
        //canStatus sta = canWrite(kHandle[i], fream_id, data, 8, canMSG_EXT);
        msleep(2);
        if (sta != canOK)
        {
            qDebug() << "Channel " << i << " send failure";
        }
    }
}

void kvaser::run()
{
    while(!stop)
    {
//        qDebug()<<"receive Thread ID:"<<QString::number(quintptr(QThread::currentThreadId()))<<stop;
        canStatus status = canERR_NOMSG;
        long id;
        unsigned char msg[8] = {};
        unsigned int dlc;
        unsigned int flag;
        unsigned long time = 0;
        
        for(int k=0;k<chanCount;++k)
        {
            QByteArray data;
            if(kHandle[k]<0)
                continue;
            status = canRead(kHandle[k],&id,&msg,&dlc,&flag,&time);
            if(status == canOK && id != 0x00)
            {
                for(int i = 0; i < 8; i++)
                {
                    data.append(msg[i]);
                }
                //emit getProtocolData(id,data);
                emit getProtocolData(k,id,data);
            }
        }
        msleep(5);

    }
}
//void kvaser::run()
//{
//    while(!stop)
//    {
////        qDebug()<<"receive Thread ID:"<<QString::number(quintptr(QThread::currentThreadId()))<<stop;
//        canStatus status = canERR_NOMSG;
//        long id;
//        unsigned char msg[8] = {};
//        unsigned int dlc;
//        unsigned int flag;
//        unsigned long time = 0;
//        QByteArray data;

//        status = canRead(h,&id,&msg,&dlc,&flag,&time);
//        if(status == canOK && id != 0x00)
//        {
//            for(int i = 0; i < 8; i++)
//            {
//                data.append(msg[i]);
//            }
//            //emit getProtocolData(data,msg[0]+0x00,id);
//            emit getProtocolData(id,data);
//        }
//        msleep(2);

//    }
//}
