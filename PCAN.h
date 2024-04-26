/*
 * PCAN.h - header file of PCAN class
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
    Q_PROPERTY(bool isExtend READ readIsExtend WRITE setIsExtend)
public:
	PCAN(QObject *parent);
	~PCAN();
    QStringList DetectDevice();
    bool ConnectDevice(int usb_index, int bitrate);
    //bool ConnectDevice(QString usb_name, int bitrate);
    
    void SendFrame(int FrameID, uchar Data[]);
    void SendFrame(int FrameID, uchar Data[],bool bStandard);
    void CloseCan();
 
   
    QString errorString;
    bool IsOpen() { return m_bConnect; };
    bool readIsExtend() {
        return isExtendedFream
            ;
    }
    void setIsExtend(bool b) { isExtendedFream = b; }
    bool setDeviceList(const  std::vector<ushort>& list);
    std::vector<ushort> getDeviceList() {
        return m_device_io_list;
            
    }
private:
    void run();
    bool isExtendedFream = false;
    //当前连接状态
    bool m_bConnect;
    ushort m_hCurHandle;
    //可用的设备索引
    std::vector<ushort> m_device_io_list;
    //PCAN默认的设备句柄，这里初始化必须C++17及以上才能编译
    void disConnectDev();
signals:
    void getProtocolData(uint frame_id, QByteArray data);
};
