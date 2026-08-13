#include "peakLIN.h"
#include <minwindef.h>
#include <QtConcurrent>
#include "../DeviceIO/3rd/QsLog/include/QsLog.h"


peakLIN::peakLIN(QObject* parent) : DeviceIO(parent)
{
	m_bConnect = false;
	m_hClient = 0;
	//LINÉè±¸
	dev_type = this->LIN;
	QByteArray strName = "KUS-PLIN";
	LPSTR lstr = strName.data();
	m_LastLINErr = LIN_RegisterClient(lstr, (DWORD)0, &m_hClient);
	bRunRead = true;
	if (m_LastLINErr != errOK)
	{
		QLOG_WARN() << "PLIN RegisterClient failed";
		bRunRead = false;
	}
}

peakLIN::~peakLIN()
{
	m_bConnect = false;
}

bool peakLIN::openDevice(const QJsonObject& param)
{
	if (param.size() < 2)
	{
		return false;
	}
	int ch = param["ch"].toInt();
	int bundRate = param["bundRate"].toInt();
	uint bundr;
	switch (bundRate)
	{
	case 3:
		bundr = 9600; break;
	case 4:
		bundr = 19200; break;
	default:
		bundr = 19200; break;
		break;
	}
	bool fRet;
	if (ch >= m_devList.size())
		return false;
	WORD lwHw = m_devList.at(ch).toInt(), lwBaud = bundr;
	int lnMode, lnCurrBaud;
	TLINHardwareMode lHwMode;
	lHwMode = modMaster;
	m_hHw = lwHw;
	m_LastLINErr = LIN_ConnectClient(m_hClient, lwHw);
	if (m_LastLINErr == errOK)
	{
		m_LastLINErr = LIN_InitializeHardware(m_hClient, m_hHw, lHwMode, lwBaud);
		if (m_LastLINErr != errOK)
		{
			QLOG_WARN() << "Open PLIN failed: " << m_LastLINErr;
			return false;
		}
		m_LastLINErr = LIN_SetClientFilter(m_hClient, m_hHw, FRAME_FILTER_MASK);
		if (m_LastLINErr != errOK)
		{
			QLOG_WARN() << "PLIN SetCLientFilter failed: " << m_LastLINErr;
			return false;
		}
		//QLOG_INFO() << "GOOD JOB";
		m_bConnect = true;
	}
	else
	{
		return false;
	}
	QtConcurrent::run(this, &peakLIN::getNewMessage);
	return true;
}

void peakLIN::sendData(const ushort ch, const uint fream_id, uchar data[8], bool standard, int len, Dirction d)
{
	TLINMsg pMsg;
	// Create a new LIN frame message and copy the data.
	BYTE idd = uchar(fream_id);
	LIN_GetPID(&idd);
	pMsg.FrameId = idd;
	if (d == Master)
		pMsg.Direction = dirPublisher;
	else
		pMsg.Direction = dirSubscriber;
	pMsg.ChecksumType = cstEnhanced;
	pMsg.Length = len;
	// Fill data array
	/*pMsg.Data[0] = 0x64;
	pMsg.Data[1] = 0x32;
	pMsg.Data[2] = 0;
	pMsg.Data[3] = 0;
	pMsg.Data[4] = 0;
	pMsg.Data[5] = 0;
	pMsg.Data[6] = 0;
	pMsg.Data[7] = 0;*/
	memcpy(pMsg.Data, data, len);
	// Check if the hardware is initialize as master
	//if (m_HwMode == modMaster)
	{
		// Calculate the checksum contained with the
		// checksum type that set some line before.
		LIN_CalculateChecksum(&pMsg);
		// Try to send the LIN frame message with LIN_Write.
		m_LastLINErr = LIN_Write(m_hClient, m_hHw, &pMsg);
	}

}

QStringList peakLIN::enumDevice()
{
	BYTE i;
	HLINHW lwHwHandles[8];
	WORD lwBuffSize;
	int lwCount, lnHwType, lnDevNo, lnChannel, lnMode;
	TLINError lLINErr;
	HLINHW lwHw;

	//lwHwHandles = new HLINHW[8];
	lwBuffSize = 8;
	lwCount = 0;
	lLINErr = LIN_GetAvailableHardware(lwHwHandles, lwBuffSize, &lwCount);
	QStringList devList;
	if (lLINErr != errOK)
	{
		return QStringList();
	}
	if (lwCount == 0)
		return QStringList();
	for (int i = 0; i < lwCount; i++)
	{
		lwHw = lwHwHandles[i];
		LIN_GetHardwareParam(lwHw, hwpType, &lnHwType, 0);
		LIN_GetHardwareParam(lwHw, hwpDeviceNumber, &lnDevNo, 0);
		LIN_GetHardwareParam(lwHw, hwpChannelNumber, &lnChannel, 0);
		LIN_GetHardwareParam(lwHw, hwpMode, &lnMode, 0);
		if (lnHwType == LIN_HW_TYPE_USB)
		{
			QString name = "LIN-" + QString::number(lnDevNo) + "-" + QString::number(lnChannel);
			devList.append(name);
		}
		else
		{
			devList.append("LIN-" + QString::number(lnChannel));
			m_devList.append(QString::number(lnChannel));
		}
	}

	return devList;

}

void peakLIN::disConnectDev()
{
	bRunRead = false;
	LIN_DisconnectClient(m_hClient, m_hHw);
}

void peakLIN::getNewMessage()
{
	TLINRcvMsg pMsg;
	while (bRunRead)
	{

		m_LastLINErr = LIN_Read(m_hClient, &pMsg);

		if (m_LastLINErr != errOK || pMsg.ErrorFlags != errOK || pMsg.Direction== dirPublisher)
		{
			QThread::msleep(5);
			continue;
		}

		{
			BYTE id = pMsg.FrameId & 0x3F;
			QByteArray data;
			for (int m = 0; m < pMsg.Length; m++)
			{
				data.append(pMsg.Data[m]);
			}
			emit sigNewMessage(0, id, data);
		}

	}
	return;
}
