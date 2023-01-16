/*
 * kvaser.h - header file of kvaser class
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
    void canSendAll(uint fream_id,uchar data[8],bool bStandard);
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
