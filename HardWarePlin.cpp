#include "HardWarePlin.h"
#include <minwindef.h>
#include "QsLog.h"
#include <QtConcurrent>

HardWarePlin::HardWarePlin(QObject *parent)
	: HardWareBase(parent)
{
	m_hClient = 0;
	QByteArray strName = "KUS-PLIN";
	LPSTR lstr = strName.data();
	m_LastLINErr = LIN_RegisterClient(lstr, (DWORD)0, &m_hClient);
	bRunRead = true;
	if (m_LastLINErr != errOK)
	{
		QLOG_WARN() << "RegisterClient failed";
		bRunRead = false;
	}
}

HardWarePlin::~HardWarePlin()
{
	bRunRead = false;
	CloseHardWare();
}

bool HardWarePlin::OpenHardWare(QString dev, int bundrate, int attach)
{
	/*QByteArray strName = "KUS-PLIN";
	LPSTR lstr = strName.data();
	m_LastLINErr = LIN_RegisterClient(lstr, (DWORD)0, &m_hClient);
	if (m_LastLINErr != errOK)
	{
		return false;
	}*/

	bool fRet;
	WORD lwHw= dev.toInt(), lwBaud = bundrate;
	int lnMode, lnCurrBaud;
	TLINHardwareMode lHwMode;
	lHwMode = modMaster;
	m_hHw = lwHw;
	m_LastLINErr = LIN_ConnectClient(m_hClient, lwHw);
	if (m_LastLINErr == errOK)
	{
		m_LastLINErr =LIN_InitializeHardware(m_hClient,m_hHw, lHwMode, lwBaud);
		if (m_LastLINErr != errOK)
		{
			QLOG_WARN() << "Open LIN failed: "<< m_LastLINErr;
			return false;
		}
		m_LastLINErr = LIN_SetClientFilter(m_hClient, m_hHw, FRAME_FILTER_MASK);
		if (m_LastLINErr != errOK)
		{
			QLOG_WARN() << "SetCLientFilter failed: " << m_LastLINErr;
			return false;
		}
		QLOG_INFO() << "GOOD JOB";
	}
	else
	{
		return false;
	}
	QtConcurrent::run(this, &HardWarePlin::ReadMessage);
	return true;
}
bool HardWarePlin::CloseHardWare()
{
	return LIN_DisconnectClient(m_hClient, m_hHw);;
}
bool HardWarePlin::SendMessage(uint id, uchar data[], int resever[])
{
	TLINMsg pMsg;
	// Create a new LIN frame message and copy the data.
	BYTE idd = id;
	LIN_GetPID(&idd);
	pMsg.FrameId = idd;
	if(0==resever[1])
		pMsg.Direction = dirPublisher;
	else
		pMsg.Direction = dirSubscriber;
	pMsg.ChecksumType = cstEnhanced;
	pMsg.Length = resever[0];
	// Fill data array
	/*pMsg.Data[0] = 0x64;
	pMsg.Data[1] = 0x32;
	pMsg.Data[2] = 0;
	pMsg.Data[3] = 0;
	pMsg.Data[4] = 0;
	pMsg.Data[5] = 0;
	pMsg.Data[6] = 0;
	pMsg.Data[7] = 0;*/
	memcpy(pMsg.Data, data, resever[0]);
	// Check if the hardware is initialize as master
	//if (m_HwMode == modMaster)
	{
		// Calculate the checksum contained with the
		// checksum type that set some line before.
		LIN_CalculateChecksum(&pMsg);
		// Try to send the LIN frame message with LIN_Write.
		m_LastLINErr = LIN_Write(m_hClient, m_hHw, &pMsg);
	}
	return true;
}

bool HardWarePlin::ReadMessage()
{
	TLINRcvMsg pMsg;
	while (bRunRead)
	{

		m_LastLINErr = LIN_Read(m_hClient, &pMsg);
		
		if (m_LastLINErr != errOK || pMsg.ErrorFlags != errOK)
		{
			Sleep(5);
			continue;
		}
			
		{
			BYTE id = pMsg.FrameId & 0x3F;
			QByteArray data;
			for (int m = 0; m < 8; m++)
			{
				data.append(pMsg.Data[m]);
			}
			emit newMessage(id, data, 0);
		}
		
	}
	return true;
}

QStringList HardWarePlin::GetHardWare()
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
	if(lwCount==0)
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
			QString name = "USB_LIN-" + QString::number(lnDevNo) + "-" + QString::number(lnChannel);
			devList.append(name);
		}
		else
		{
			devList.append("LIN-"+ QString::number(i));
		}
	}

	return devList;
}
