#include "HardWareTlin.h"
#include <minwindef.h>
#include "QsLog.h"
#include <QtConcurrent>
#include "usb2lin_ex.h"
#include "usb_device.h"
#include "lin_uds.h"

HardWareTlin::HardWareTlin(QObject *parent)
	: HardWareBase(parent)
{
	m_cycle = 100;
}

HardWareTlin::~HardWareTlin()
{
	bRunRead = false;
	condition.wakeAll();
	QThread::msleep(7);
	CloseHardWare();
}

bool HardWareTlin::OpenHardWare(QString dev, int bundrate, int attach)
{

	
	int index = dev.toInt();
	bool ret = USB_OpenDevice(m_hardHandle[index]);
	if (!ret)return false;
	ret = LIN_EX_Init(m_hardHandle[index], 0, bundrate, LIN_EX_MASTER);
	if (ret!= LIN_EX_SUCCESS)return false;
	bRunRead = true;
	m_curHandle = m_hardHandle[index];
	m_curIndex = index;
	QtConcurrent::run(this, &HardWareTlin::ReadMessage);
	return true;
}
bool HardWareTlin::CloseHardWare()
{
	bRunRead = false;
	return true;
}
bool HardWareTlin::SendMessage(uint id, uchar data[], int resever[])
{
	bool ret;
	qmutex.lock();
	if(0==resever[1])
	{
		ret = (LIN_EX_MasterWrite(m_curHandle, m_curIndex, id, data, resever[0], LIN_EX_CHECK_EXT) == 0);
		
	}
	qmutex.unlock();
	condition.wakeAll();
	return (ret==0);

}
void HardWareTlin::setRespondID(const std::vector<int> vec_id)
{
	m_respondID.clear();
	for (auto x : vec_id)
		m_respondID.push_back(x);
}
bool HardWareTlin::ReadMessage()
{
	while (bRunRead)
	{
		qmutex.lock();
		condition.wait(&qmutex);
		uchar data[8];
		for (auto x : m_respondID)
		{
			int len = LIN_EX_MasterRead(m_curHandle, m_curIndex, x, data);
			if (len>0)
			{
				QByteArray sdata;
				for (int m = 0; m < len; m++)
				{
					sdata.append(data[m]);
				}
				emit newMessage(x, sdata, 0);
			}
			QThread::msleep(5);
		}
		qmutex.unlock();
		QThread::msleep(m_cycle);
	}
	return true;
}

QStringList HardWareTlin::GetHardWare()
{
	//查找设备，并把找到的设备的句柄放到m_hardHandle数组中
	int num = USB_ScanDevice(m_hardHandle);
	m_hardNum = num;
	QStringList devList;

	for (int i = 0; i < num; i++)
	{
		{
			devList.append("LIN-"+ QString::number(i));
		}
	}

	return devList;
}
