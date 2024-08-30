#include <qmessagebox.h>
#include <QSerialPort>
#include <QComboBox>
#include "TP700TemperatureModel.h"
#include "QMoudBusCtrl.h"
#include "QsLog.h"
#pragma execution_character_set("utf-8")  
static QByteArray requestArray_1 = QByteArrayLiteral("\x01\x03\x00\x00\x00\x10\x44\x06");
#define _DATA_NUM_ 37
#define _CHANNEL_NUM 32
static ushort m_register_ = 16;
TP700TemperatureModel::TP700TemperatureModel(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	
	ui.tableWidget->setColumnWidth(0, 200);
	cb = new QComboBox(this);
	cb->addItem("TP700",32);
	cb->addItem("DAM-4501",8);
	ui.tableWidget->setCellWidget(0, 1, cb);
	connect(cb, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TP700TemperatureModel::on_comboboxDevice_indexChanged);
	m_register_= 32;
}

TP700TemperatureModel::~TP700TemperatureModel()
{

}

QVector<float> TP700TemperatureModel::getTemperatur()
{
	QVector<float> vec;
	for (int i = 0; i < m_register_; i++)
	{
		if (indexData[i] == 1)
		{
			vec.append(vData[i].fval);
		}
	}
	return vec;
}

bool TP700TemperatureModel::openSerial()
{
	if (!serialport_tp700)
		serialport_tp700 = new QMoudBusCtrl(this);
	//if(b)
	bitrate = this->ui.tableWidget->item(2, 1)->text().toInt();
	portname = this->ui.tableWidget->item(1, 1)->text();
	
	isSerialOK = serialport_tp700->connectToDevice(portname, QSerialPort::NoParity, bitrate, QSerialPort::OneStop, QSerialPort::Data8);
	return isSerialOK;
}
void TP700TemperatureModel::on_timesend()
{
	QModbusDataUnit mdu;
	mdu.setRegisterType(QModbusDataUnit::HoldingRegisters);
	mdu.setStartAddress(0);
	mdu.setValueCount(m_register_*2);
	QModbusDataUnit reply = serialport_tp700->sendReadMdu(mdu, serviceAddr);
	if (reply.valueCount() < m_register_*2)
	{
		bDataUp = false;
		QLOG_WARN() << "Loss data";
		return;
	}
	bDataUp = true;
	on_receive(reply.values());
}
void TP700TemperatureModel::on_receive(QVector<uint16_t> data)
{
	
	{
		if(m_register_==32)
		{
			for (int i = 0; i < m_register_*2 && i< data.size(); i += 2)
			{
				//uData[i] = (uchar)data.at(i + 3);
				vData[i / 2].data[0] = data.at(i + 1) & 0xFF;
				vData[i / 2].data[1] = (data.at(i + 1) >> 8) & 0xFF;
				vData[i / 2].data[2] = data.at(i) & 0xFF;
				vData[i / 2].data[3] = (data.at(i) >> 8) & 0xFF;

				if (indexData[i / 2] == 1 && m_debug)
				{
					QLOG_INFO() <<"Ch"<<i/2<<": " << vData[i / 2].fval << "°C";
				}
			}
		}
		else if (m_register_ == 8)
		{
			for (int i = 0; i < m_register_*2 && i< data.size(); i++)
			{
				vData[i].fval = data.at(i) / 10.0;
				if (indexData[i] == 1 && m_debug)
				{
					QLOG_INFO() << "Ch" << i<< ": " << vData[i].fval<<"°C";
				}
			}
			
		}
		
	}
}
void TP700TemperatureModel::on_comboboxDevice_indexChanged(int index)
{
	QComboBox* cb1 = dynamic_cast<QComboBox*>(sender());
	if (!cb1)return;
	m_register_ = cb1->currentData().toInt();
}
void TP700TemperatureModel::on_pbStart_clicked(bool b)
{
	if (b)
	{
		if (!openSerial())
		{
			QMessageBox::warning(this, QString("Error"), QString("串口打开失败，请检查串口设备"));
			return;
		}
		memset(indexData, 0, sizeof(uchar) * 32);
		if (!isSerialOK)
		{
			QMessageBox::warning(this, QString("Error"), QString("串口未打开，不能启动"));
			return;
		}
		aliasName = this->ui.tableWidget->item(4, 1)->text();
		channel = this->ui.tableWidget->item(3, 1)->text().split(";");
		m_debug = this->ui.tableWidget->item(5, 1)->text().toInt();
		if (cb)
			m_register_ = cb->currentData().toInt();
		else
			m_register_ = 8;
		if (channel.size() <= 0)
		{
			QMessageBox::warning(this, QString("Error"), QString("通道参数有错"));
			return;
		}
		for (int x = 0; x < channel.size(); x++)
		{
			int index = channel.at(x).toInt();
			if (index < 0 || index>m_register_)
			{
				QMessageBox::warning(this, QString("Error"), QString("通道参数有错"));
				return;
			}
			indexData[index] = 1;
		}
		this->ui.pbStart->setText("Stop");
		connect(&timer, &QTimer::timeout, this, &TP700TemperatureModel::on_timesend);
		//connect(serialport_tp700, &SerialportManage::newMessage, this, &TP700TemperatureModel::on_receive);
		timer.start(1000);
	}
	else
	{
		timer.stop();
		disconnect(&timer, &QTimer::timeout, this, &TP700TemperatureModel::on_timesend);
		this->ui.pbStart->setText("Start");
		bDataUp = false;
		serialport_tp700->disConnectDevice();
	}
}
