/*
 * CANThread.h - header file of CANThread class
 *
 * Copyright (C) 2021-2022 lyb <liyangbinbin@foxmail.com>
 *
 * This file is part of the QtCanPlatform.
 *
 * QtCanPlatform is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * QtCanPlatform is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libQtShadowsocks; see the file LICENSE. If not, see
 * <http://www.gnu.org/licenses/>.
 */
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
    void sendData(UINT ID, uchar qbt[],bool bStandard);

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
