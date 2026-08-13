
#include "zlgcan.h"
#include <QDebug>
#include <QTime>
#include <QtConcurrent>
zlgCAN::zlgCAN(QObject* parent)
{

}

zlgCAN::~zlgCAN()
{
	b_open_ = false;
	disConnectDev();
}

bool zlgCAN::openDevice(const QJsonObject& param)
{
	if (param.size() < 3)
	{
		return false;
	}
	m_channel = param["ch"].toInt();
	int bundrate = param["bundRate"].toInt();
	int devType = param["devModel"].toInt();

	if (devType == 3)
	{
		device_handle_ = ZCAN_OpenDevice(ZCAN_USBCAN2, 0, 0);
	}
	else if (devType == 2)
	{
		device_handle_ = ZCAN_OpenDevice(ZCAN_USBCAN_2E_U, 0, 0);
	}
	if (INVALID_DEVICE_HANDLE == device_handle_)
	{
		return false;
	}
	uint bundr;
	switch (bundrate)
	{
	case 1:
		bundr = 250; break;
	case 2:
		bundr = 500; break;
	case 0:
		bundr = 125; break;
	default:
		bundr = 250; break;
		break;
	}
	QString path = QString::number(m_channel) + "/baud_rate";
	QString value = QString::number(bundr * 1000);
	int res = ZCAN_SetValue(device_handle_, path.toStdString().c_str(), value.toStdString().c_str());
	if (1 != res)
	{
		disConnectDev();
		return false;
	}

	ZCAN_CHANNEL_INIT_CONFIG config;
	memset(&config, 0, sizeof(config));
	config.can_type = TYPE_CAN;
	config.can.mode = 0;                    //0是正常模式，1是只听模式
	config.can.filter = 1;
	config.can.acc_code = 0x00000000;
	config.can.acc_mask = 0xFFFFFFFF;
	channel_handle_ = ZCAN_InitCAN(device_handle_, m_channel, &config);

	if (INVALID_CHANNEL_HANDLE == channel_handle_)
	{
		disConnectDev();
		return false;
	}

	if (ZCAN_StartCAN(channel_handle_) != STATUS_OK)
	{
		disConnectDev();
		return false;
	}
	ZCAN_ClearBuffer(channel_handle_);
	b_open_ = true;
	QtConcurrent::run(this, &zlgCAN::getNewMessage);
	return true;
}

void zlgCAN::sendData(const ushort ch, const uint fream_id, uchar data[8], bool standard, int len, Dirction d)
{
	tagZCAN_Transmit_Data send;
	memcpy(send.frame.data, data, sizeof(uchar) * 8);
	send.frame.can_dlc = 8;
	send.frame.can_id = fream_id + 0x80000000;

	//
	send.transmit_type = 0;
	ZCAN_Transmit(channel_handle_, &send, 1);
}

QStringList zlgCAN::enumDevice()
{
	QStringList list;
	list.append("CAN1");
	list.append("CAN2");
	return list;
}

void zlgCAN::disConnectDev()
{
	b_open_ = false;
	ZCAN_CloseDevice(device_handle_);
}

void zlgCAN::getNewMessage()
{
	ZCAN_Receive_Data can_data[100];
	ZCAN_ReceiveFD_Data canfd_data[100];
	UINT len;
	QByteArray data;
	QString temp;
	ZCAN_ClearBuffer(channel_handle_);
	while (b_open_)
	{
		//temp.clear();
		if (len = ZCAN_GetReceiveNum(channel_handle_, TYPE_CAN))
		{
			len = ZCAN_Receive(channel_handle_, can_data, 100, 50);
			//QByteArray data;
			//temp.append("Rx ID: "+QString::number(can_data[0].frame.can_id-0x80000000,16));
			for (int mk = 0; mk < len; mk++)
			{
				for (int m = 0; m < can_data[mk].frame.can_dlc; m++)
				{
					data.append(can_data[mk].frame.data[m]);
					//temp.append("  0x"+QString::number(can_data[0].frame.data[m],16));
				}
				//emit getProtocolData2(data, 0, can_data[0].frame.can_id - 0x80000000);
				emit sigNewMessage(m_channel, can_data[0].frame.can_id - 0x80000000, data);
				data.clear();
				// qDebug()<<QTime::currentTime().toString("hh:mm:ss:zzz")+"  "<<temp;
			}
		}

		//避免无数据时变成While(1),会占用大量的CPU
		QThread::msleep(5);
	}
	ZCAN_CloseDevice(device_handle_);
}
