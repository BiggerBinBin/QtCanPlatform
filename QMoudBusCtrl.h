#pragma once

#include <QObject>
#include <QModbusDataUnit>
#include <qserialportinfo.h>
class QModbusClient;
class QModbusReply;
class QMoudBusCtrl  : public QObject
{
	Q_OBJECT

public:
	QMoudBusCtrl(QObject *parent);
	~QMoudBusCtrl();
	bool connectToDevice(QString seriport,uint paity,uint budrate,uint stopbit,uint databit);
	bool connectToDevice(QString ipaddress,uint port);
	bool disConnectDevice();
	bool connectState() { return State; }
	bool isEndSendFream() { return isEndSend; }
	QList<QSerialPortInfo> getSerialInfo();
	bool sendWriteMdu(QModbusDataUnit mdu,uint serverAdd);
	QModbusDataUnit sendReadMdu(QModbusDataUnit mdu,uint serverAdd);
private:
	QModbusClient* modbusDevice = nullptr;
	QModbusReply* modbusReply = nullptr;
	QList<QSerialPortInfo>SerialInfo;
	bool State = false;
	bool isEndSend = true;
signals:
	void resultReadly(QModbusDataUnit mdu);

};
