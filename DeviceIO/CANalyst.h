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

#include <QThread>
#include <QDebug>
#include "deviceio.h"

class DEVICEIO_EXPORT CANalyst : public DeviceIO
{
    Q_OBJECT

public:
    explicit CANalyst(QObject* parent = nullptr);
    ~CANalyst();
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
    int deviceType;
    int deviceIndex;
    int deviceChannel;
    int i_DEVICE_NUM;

};
