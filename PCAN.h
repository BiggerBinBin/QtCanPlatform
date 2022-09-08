/*
 * QtCanPlatform.h - header file of QtCanPlatform class
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
#pragma once
#include <QObject>
#include <QtSerialBus>
#include <QMetaType>
#include <QThread>

class PCAN  : public QThread
{
	Q_OBJECT

public:
	PCAN(QObject *parent);
	~PCAN();
    QStringList DetectDevice();
    bool ConnectDevice(int usb_index, int bitrate);
    void Receive();
    void SendFrame(int FrameID, uchar Data[]);
    void CloseCan();
    bool isReceive;
    QList<QCanBusDeviceInfo> m_interfaces;
    QCanBusDevice* m_canDevice = nullptr;
    const QString plugin = "peakcan";
    QString errorString;
private:
    void run();
signals:
    void getProtocolData(uint frame_id, QByteArray data);
};
