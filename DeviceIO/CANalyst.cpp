#include <QtConcurrent>
#include "../DeviceIO/3rd/QsLog/include/QsLog.h"
#include "CANalyst.h"
#include "../3rd/CANalyst/include/ControlCAN.h"
VCI_BOARD_INFO vbi;
CANalyst::CANalyst(QObject* parent) : DeviceIO(parent)
{
	m_bConnect = false;
	deviceType = 0x04;
	deviceIndex = 0x00;
	deviceChannel = 0x0;
	i_DEVICE_NUM = 0;
	//CAN设备
	dev_type = this->CAN;
}

CANalyst::~CANalyst()
{
	disConnectDev();
}

bool CANalyst::openDevice(const QJsonObject& param)
{
	if (param.size() < 2)
	{
		return false;
	}
	int ch = param["ch"].toInt();
	int bundRate = param["bundRate"].toInt();
	deviceChannel = ch;
	DWORD nDeviceType = (DWORD)deviceType; /* USBCAN-2A或USBCAN-2C或CANalyst-II */
	DWORD nDeviceInd = (DWORD)deviceIndex; /* 第几个个设备 */
	DWORD nCANInd = (DWORD)deviceChannel; /* 通道 */
	DWORD dwRel;

	uint bundr;
	switch (bundRate)
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


	dwRel = VCI_OpenDevice(nDeviceType, nDeviceInd, nCANInd);
	if (dwRel != 1U)
	{
		qDebug() << "open fail:";
		return false;
	}
	else
	{
		qDebug() << "open success";
	}
	dwRel = VCI_ClearBuffer(nDeviceType, nDeviceInd, nCANInd);
	VCI_INIT_CONFIG vic;
	vic.AccCode = 0x80000008U;
	vic.AccMask = 0xFFFFFFFFU;
	vic.Filter = 1U;
	vic.Mode = 0U;
	switch (bundr) {
	case 10:
		vic.Timing0 = 0x31U;
		vic.Timing1 = 0x1cU;
		break;
	case 20:
		vic.Timing0 = 0x18U;
		vic.Timing1 = 0x1cU;
		break;
	case 40:
		vic.Timing0 = 0x87U;
		vic.Timing1 = 0xffU;
		break;
	case 50:
		vic.Timing0 = 0x09U;
		vic.Timing1 = 0x1cU;
		break;
	case 80:
		vic.Timing0 = 0x83U;
		vic.Timing1 = 0xffU;
		break;
	case 100:
		vic.Timing0 = 0x04U;
		vic.Timing1 = 0x1cU;
		break;
	case 125:
		vic.Timing0 = 0x03U;
		vic.Timing1 = 0x1cU;
		break;
	case 200:
		vic.Timing0 = 0x81U;
		vic.Timing1 = 0xfaU;
		break;
	case 250:
		vic.Timing0 = 0x01U;
		vic.Timing1 = 0x1cU;
		break;
	case 400:
		vic.Timing0 = 0x80U;
		vic.Timing1 = 0xfaU;
		break;
	case 500:
		vic.Timing0 = 0x00U;
		vic.Timing1 = 0x1cU;
		break;
	case 666:
		vic.Timing0 = 0x80U;
		vic.Timing1 = 0xb6U;
		break;
	case 800:
		vic.Timing0 = 0x00U;
		vic.Timing1 = 0x16U;
		break;
	case 1000:
		vic.Timing0 = 0x00U;
		vic.Timing1 = 0x14U;
		break;
	case 33:
		vic.Timing0 = 0x09U;
		vic.Timing1 = 0x6fU;
		break;
	case 66:
		vic.Timing0 = 0x04U;
		vic.Timing1 = 0x6fU;
		break;
	case 83:
		vic.Timing0 = 0x03U;
		vic.Timing1 = 0x6fU;
		break;
	default:
		vic.Timing0 = 0x03U;
		vic.Timing1 = 0x6fU;
		break;
	}
	dwRel = VCI_InitCAN(nDeviceType, nDeviceInd, nCANInd, &vic);
	if (dwRel != 1U)
	{
		QLOG_WARN() << "Init fail";
		return false;
	}


	dwRel = VCI_ReadBoardInfo((DWORD)nDeviceType, (DWORD)nDeviceInd, &vbi);
	if (dwRel != 1U)
	{
		QLOG_INFO() << "Get dev message fail:";
		return false;
	}
	else
	{
		/*QLOG_INFO() << "CAN通道数：" << vbi.can_Num;
		QLOG_INFO() << "硬件版本号:" << vbi.hw_Version;
		QLOG_INFO() << "接口库版本号：" << vbi.in_Version;
		QLOG_INFO() << "中断号" << vbi.irq_Num;*/
	}

	if (VCI_StartCAN(nDeviceType, nDeviceInd, nCANInd) != 1U)
	{
		QLOG_INFO() << "Start" << nCANInd << "fail";
		return false;
	}
	else
	{
		QLOG_INFO() << "Start" << nCANInd << "success";
	}
	m_bConnect = true;
	QtConcurrent::run(this, &CANalyst::getNewMessage);
	return true;
}

void CANalyst::sendData(const ushort ch, const uint fream_id, uchar data[8], bool standard, int len, Dirction d)
{
	DWORD dwRel;
	VCI_CAN_OBJ vco[1];

	for (int i = 0; i < 8; i++)
	{
		vco->Data[i] = data[i];
	}
	int count = 1;
	vco->ID = fream_id;
	vco->DataLen = 8;
	vco->RemoteFlag = 0U;
	vco->SendType = 0;
	if (standard)
		vco->ExternFlag = 0;
	else
		vco->ExternFlag = 1;

	dwRel = VCI_Transmit(deviceType, deviceIndex, deviceChannel, vco, count);
	/*if (dwRel <= 0U)
	{
		QLOG_INFO() << "CANayst Send fail:" << QString::number(dwRel);
	}*/


}

QStringList CANalyst::enumDevice()
{

	VCI_BOARD_INFO pInfo[50];
	int num = VCI_FindUsbDevice2(pInfo);
	i_DEVICE_NUM = num;
	QStringList canlist;
	for (int i = 0; i < num; i++)
	{
		for (int n = 0; n < pInfo[i].can_Num; n++)
			canlist.append("CAN-" + QString::number(i_DEVICE_NUM * i + n));
	}

	return canlist;
}

void CANalyst::disConnectDev()
{
	m_bConnect = false;
	QThread::msleep(20);
	for (int i = 0; i < i_DEVICE_NUM; i++)
		VCI_CloseDevice((DWORD)VCI_USBCAN2A, (DWORD)i);
	QLOG_INFO() << "关闭CANayst";
}

void CANalyst::getNewMessage()
{
	VCI_CAN_OBJ vco[2500];
	while (m_bConnect)
	{
		int dwRel;

		memset(vco, 0, sizeof(VCI_CAN_OBJ) * 2500);
		dwRel = VCI_Receive(deviceType, deviceIndex, deviceChannel, vco, 2500, 0);
		if (dwRel > 0)
		{
			for (int i = 0; i < dwRel; i++)
			{
				QByteArray data;
				for (int i = 0; i < vco->DataLen; i++)
				{
					data.append(vco->Data[i]);
				}
				emit sigNewMessage(deviceChannel, vco->ID, data);
			}
		}
		else if (dwRel == -1)
		{


		}

		QThread::msleep(5);


	}

}