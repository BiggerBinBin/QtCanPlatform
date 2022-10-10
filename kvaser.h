#ifndef KVASER_H
#define KVASER_H


#include <QThread>
#include <QDebug>
#include "kvaser_INC/canlib.h"

class kvaser:public QThread
{
    Q_OBJECT

public:
    kvaser();
    bool openCan(int can_chanel, int bus_speed);
    int* openCanAll(int bitRate,QString &msg);
    void canSend(int frame_id, unsigned int msg[]);
    void canSendAll(uint fream_id,uchar data[8]);
    void closeCAN();
    int getCanCount();

    CanHandle h;
    bool stop;
    int kHandle[4];
    int chanCount = 0;
protected:
    void run();
private:
    //int chanCount = 0;
signals:
    //void getProtocolData(QByteArray data,int common,quint32 ID);
    //void getProtocolData(quint32 ID,QByteArray data);
    void getProtocolData(int channel,quint32 ID,QByteArray data);
};

#endif // KVASER_H
