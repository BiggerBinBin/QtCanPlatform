#pragma once

#include <QObject>
#include <QString>
#include  <QVariant>
struct ParamMsg {

};
Q_DECLARE_METATYPE(ParamMsg)
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
	enum PState
	{
		unconnect=0,
		connected=1
	};
private:
	PState state;
Q_SIGNALS:
	void stateChanged(PState state);
};
