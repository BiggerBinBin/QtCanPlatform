#pragma once

#include <QObject>
#include <qstring.h>
#include <qstringlist.h>
class HardWareBase  : public QObject
{
	Q_OBJECT

public:
	HardWareBase(QObject *parent);
	~HardWareBase();

	//virtual bool Init()=0;
	virtual bool OpenHardWare(QString dev,int bundrate,int attach);
	virtual bool SendMessage(uint id, uchar data[], int resever[]);
	//virtual bool ReadMessage(uint id, uchar data[], int resever);
	virtual QStringList GetHardWare();
	virtual bool CloseHardWare();
signals:
	void newMessage(uint id, QByteArray data, int resever);
};
