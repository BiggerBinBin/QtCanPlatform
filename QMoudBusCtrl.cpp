#include "QMoudBusCtrl.h"
#include <QModbusRtuSerialMaster>
#include <qvariant.h>
#include "QsLog.h"
#include <qeventloop.h>
QMoudBusCtrl::QMoudBusCtrl(QObject *parent)
	: QObject(parent)
{
	modbusDevice = new QModbusRtuSerialMaster(this);
	getSerialInfo();
}

QMoudBusCtrl::~QMoudBusCtrl()
{
	if (modbusDevice) { delete modbusDevice; modbusDevice = nullptr; }
	if (modbusReply) { delete modbusReply; modbusReply = nullptr; }
}

bool QMoudBusCtrl::connectToDevice(QString seriportname, uint paity, uint budrate, uint stopbit, uint databit)
{
	if (modbusDevice->state() != QModbusDevice::ConnectedState)
	{
		modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, seriportname);
		modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, budrate);
		modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, databit);
		modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter, paity);
		State = modbusDevice->connectDevice();
		return State;
	}
	
	return true;
}

bool QMoudBusCtrl::connectToDevice(QString ipaddress, uint port)
{
	return false;
}

bool QMoudBusCtrl::disConnectDevice()
{
	State = false;
	if (!modbusDevice)
		return false;
	modbusDevice->disconnectDevice();
	return true;
}

QList<QSerialPortInfo> QMoudBusCtrl::getSerialInfo()
{
	SerialInfo = QSerialPortInfo::availablePorts(); 
	return SerialInfo;
}

bool QMoudBusCtrl::sendWriteMdu(QModbusDataUnit mdu,uint serverAdd)
{
	if(!modbusDevice)
		return false;
	isEndSend = false;
	auto *reply = modbusDevice->sendWriteRequest(mdu, serverAdd);
	if (reply)
	{
		if (!reply->isFinished())
		{
			QEventLoop loop;
			connect(reply, &QModbusReply::finished, &loop, &QEventLoop::quit);
			loop.exec();
		}
		isEndSend = true;

		if (reply->error() == QModbusDevice::ProtocolError)
		{
			QLOG_INFO() << "ModbusDevice error:" << reply->errorString() << ";" << reply->rawResult().exceptionCode();
			reply->deleteLater();
			return false;
		}
		else if (reply->error() != QModbusDevice::NoError)
		{
			QLOG_INFO() << "ModbusDevice error:" << reply->errorString();
			reply->deleteLater();
			return false;
		}
		else
		{

			reply->deleteLater();
			return true;
		}		
	}
	else
	{
		isEndSend = true;
		QLOG_INFO()<<"Send Error:"<<modbusDevice->errorString();
	}
}

QModbusDataUnit QMoudBusCtrl::sendReadMdu(QModbusDataUnit mdu, uint serverAdd)
{
	if (!modbusDevice)
		return QModbusDataUnit();
	isEndSend = false;
	auto* reply = modbusDevice->sendReadRequest(mdu, serverAdd);
	if (reply)
	{
		if (!reply->isFinished())
		{
			QEventLoop loop;
			connect(reply, &QModbusReply::finished, &loop, &QEventLoop::quit);
			loop.exec();
			if (!modbusDevice)
				return QModbusDataUnit();
		}
		isEndSend = true;

		if (reply->error() == QModbusDevice::ProtocolError)
		{
			QLOG_INFO() << "ModbusDevice error:" << reply->errorString() << ";" << reply->rawResult().exceptionCode();
			reply->deleteLater();
			return QModbusDataUnit();
		}
		else if (reply->error() != QModbusDevice::NoError)
		{
			QLOG_INFO() << "ModbusDevice error:" << reply->errorString();
			reply->deleteLater();
			return QModbusDataUnit();
		}
		else
		{
			return reply->result();
		}
	}
	else
	{
		isEndSend = true;
		QLOG_INFO() << "Send Error:" << modbusDevice->errorString();
	}
	return QModbusDataUnit();
}
