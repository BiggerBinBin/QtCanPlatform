#pragma once

#include <QObject>

class AbstractPower  : public QObject
{
	Q_OBJECT

public:
	AbstractPower(QObject *parent);
	~AbstractPower();
	virtual bool openDevice(QVariant param)=0;
	virtual bool outPut(QVariant param)=0;
	virtual bool stopOutPut(QVariant param)=0;
	virtual bool setOutParam(QVariant param)=0;
};
