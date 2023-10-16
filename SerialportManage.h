#pragma once

#include <QObject>
#include <qserialport.h>
#include <qserialportinfo.h>
#include <qbytearray.h>
struct _SERIAL_PARM
{
	QString serial_name;
	uint baudrate;
};
class SerialportManage  : public QObject
{
	Q_OBJECT

public:
	SerialportManage(QObject *parent);
	~SerialportManage();
	bool openSerialPort(const _SERIAL_PARM &parm);
	bool senddata(QByteArray data);
private:
	QSerialPort* serialp = nullptr;
private slots:
	void receiveData();
Q_SIGNALS:
	void newMessage(QByteArray data);
};
