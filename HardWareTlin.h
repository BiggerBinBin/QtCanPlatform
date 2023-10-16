#pragma once

//#include <QObject>
#include <windows.h>
#include "HardWareBase.h"
#include <vector>
#include <qmutex.h>
#include <qwaitcondition.h>
class HardWareTlin  : public HardWareBase
{
	Q_OBJECT

public:
	HardWareTlin(QObject *parent);
	~HardWareTlin();
	virtual bool OpenHardWare(QString dev, int bundrate, int attach) override;
	virtual bool SendMessage(uint id, uchar data[], int resever[]) override;
	//virtual bool ReadMessage(uint id, uchar data[], int resever) override;
	bool ReadMessage();
	virtual QStringList GetHardWare() override;
    virtual bool CloseHardWare() override;
	void setRespondID(const std::vector<int> vec_id);
	void setCycle(int msc) { m_cycle = msc; }
private:
    
	int m_hardHandle[10];
	int m_hardNum;
	int m_curHandle;
	int m_curIndex;
	int m_cycle;
	std::vector<int>m_respondID;
	QMutex qmutex;
	QWaitCondition condition;
    // Constant value that indicate the mask of the client filter (64bit)
    static const unsigned __int64 FRAME_FILTER_MASK = 0xFFFFFFFFFFFFFFFF;
    bool bRunRead = false;
};
