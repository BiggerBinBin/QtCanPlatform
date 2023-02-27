#pragma once

//#include <QObject>
#include <windows.h>
#include "HardWareBase.h"
#include "PLinApi.h"
class HardWarePlin  : public HardWareBase
{
	Q_OBJECT

public:
	HardWarePlin(QObject *parent);
	~HardWarePlin();
	virtual bool OpenHardWare(QString dev, int bundrate, int attach) override;
	virtual bool SendMessage(uint id, uchar data[], int resever[]) override;
	//virtual bool ReadMessage(uint id, uchar data[], int resever) override;
	bool ReadMessage();
	virtual QStringList GetHardWare() override;
    virtual bool CloseHardWare() override;
private:
    // LIN API object
    // Client handle
    HLINCLIENT m_hClient;
    // Hardware handle
    HLINHW m_hHw;
    // LIN Hardware Modus (Master/Slave)
    TLINHardwareMode m_HwMode;
    // Client filter mask
    unsigned __int64 m_lMask;
    // Baudrate Index of Hardware
    WORD m_wBaudrate;
    // Last LIN error
    TLINError m_LastLINErr;
    // Constant value that indicate the mask of the client filter (64bit)
    static const unsigned __int64 FRAME_FILTER_MASK = 0xFFFFFFFFFFFFFFFF;
    bool bRunRead = false;
};
