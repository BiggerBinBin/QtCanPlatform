#include "SerialportManage.h"

SerialportManage::SerialportManage(QObject *parent)
	: QObject(parent)
{}

SerialportManage::~SerialportManage()
{
	serialp->close();
	/*delete serialp;
	serialp = nullptr;*/
}
bool SerialportManage::openSerialPort(const _SERIAL_PARM& parm)
{
	if (!serialp)
	{
		serialp = new QSerialPort(this);
		connect(serialp, &QSerialPort::readyRead, this, &SerialportManage::receiveData);
	}
	serialp->setBaudRate(parm.baudrate);
	serialp->setPortName(parm.serial_name);
	serialp->setStopBits(QSerialPort::OneStop);
	serialp->setParity(QSerialPort::NoParity);
	serialp->setDataBits(QSerialPort::Data8);
	bool ret = serialp->open(QIODevice::OpenMode::enum_type::ReadWrite);
	return ret;
}
bool SerialportManage::senddata(QByteArray data)
{
	if (!serialp)
		return false;
	if (!serialp->isOpen())
		return false;
	serialp->write(data);
	serialp->flush();
}
void SerialportManage::receiveData()
{
	QByteArray data = serialp->readAll();
	emit newMessage(data);
}