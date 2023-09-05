#pragma once

#include <qobject.h>
#include <qthread.h>
#include "usb_device.h"
#include "usb2can.h"
class HardWareTcan:public QThread
{
	Q_OBJECT

public:
	HardWareTcan(QObject *parent);
	~HardWareTcan();
	//virtual bool Init()=0;
	bool OpenHardWare(QString dev, int bundrate, int attach);
	bool SendMessage(uint id, uchar data[], int resever[]);
	//virtual bool ReadMessage(uint id, uchar data[], int resever);
	QStringList GetHardWare();
	bool CloseHardWare();
protected:
	void run();
Q_SIGNALS:
	void getProtocolData(int ch, quint32 ID, QByteArray data);
private:
	int m_hardHandle[10];
	int m_hardNum;
	int m_curHandle;
	int m_curIndex;
	bool runGetMessage = false;
};
