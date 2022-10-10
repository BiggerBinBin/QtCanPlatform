#include "QDeviceCtrl.h"
#include "QsLog.h"
#include <thread>
#include <chrono>
#include <Windows.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
/*****************************************************
* 控制IO_OUT,共有18个IO输出点
* 格式为Head+Name+On/Off
* 字符串为须为utf-8
******************************************************/
QString Head = "500000FF03FF00001800041402000101M*0000";
QString IO_UpDown_1 = "10";		//1#上下
QString IO_Grasp_1	= "11";		//1#夹紧
QString IO_Cover_1	= "12";		//1#密封圈
QString IO_Cold_1	= "13";		//1#冷水阀
QString IO_Pure_1	= "14";		//1#纯水阀
QString IO_Blow_1	= "15";		//1#吹水阀

QString IO_UpDown_2 = "16";		//2#上下
QString IO_Grasp_2	= "17";		//2#夹紧
QString IO_Cover_2	= "18";		//2#密封圈
QString IO_Cold_2	= "19";		//2#冷水阀
QString IO_Pure_2	= "20";		//2#纯水阀
QString IO_Blow_2	= "21";		//2#吹水阀

QString IO_UpDown_3 = "22";		//3#上下
QString IO_Grasp_3	= "23";		//3#夹紧
QString IO_Cover_3	= "24";		//3#密封圈
QString IO_Cold_3	= "25";		//3#冷水阀
QString IO_Pure_3	= "26";		//3#纯水阀
QString IO_Blow_3	= "27";		//3#吹水阀
QString IOON		= "01";		//IO输出ON
QString IOOFF		= "00";		//IO输出OFF

/*****************************************************
* 读取IO输入点,共有21个IO输出点
* 格式为Head+Name+On/Off
* 字符串为须为utf-8
******************************************************/
QString InHead = "500000FF03FF000018000404010001X*0000";
QString InSatrtBit_00 = "00";
QString InSatrtBit_10 = "10";
QString InSatrtBit_20 = "20";
QString InNum		  = "0007";

QString strGreen = "background-color:green";
QString strRed = "background-color:red";
bool bRun = true;
QDeviceCtrl::QDeviceCtrl(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->setWindowTitle(tr("设备控制"));

	QVBoxLayout* vLaout1 = new QVBoxLayout();
	QVBoxLayout* vLaout2 = new QVBoxLayout();
	QVBoxLayout* vLaout3 = new QVBoxLayout();
	QHBoxLayout* hLaout = new QHBoxLayout();
	for (int i = 0; i < 7; i++)
	{
		QCheckBox* c1 = new QCheckBox(bInName_One[i]);
		QCheckBox* c2 = new QCheckBox(bInName_Two[i]);
		QCheckBox* c3 = new QCheckBox(bInName_Three[i]);
		vLaout1->addWidget(c1);
		vLaout2->addWidget(c2);
		vLaout3->addWidget(c3);
		c1->setStyleSheet(strGreen);
		c2->setStyleSheet(strGreen);
		c3->setStyleSheet(strGreen);
		checkOne[i] = c1;
		checkTwo[i] = c2;
		checkThree[i] = c3;
	}
	hLaout->addLayout(vLaout1);
	hLaout->addLayout(vLaout2);
	hLaout->addLayout(vLaout3);
	ui.tab_2->setLayout(hLaout);
	tcp = new QTcpSocket(this);
	connect(tcp, &QTcpSocket::stateChanged, this, &QDeviceCtrl::on_SocketStateChanged);
	//connect(tcp, &QTcpSocket::readyRead, this, &QDeviceCtrl::_readyRead);
	connect(this, &QDeviceCtrl::timeToSend, this, &QDeviceCtrl::on_timeToSend);
	tcp->connectToHost(ipAddress, port);
	std::thread t(&QDeviceCtrl::getInStateRun, this);
	t.detach();
	/*std::thread t2(&QDeviceCtrl::getPowerRun, this);
	t2.detach();*/

	moudBus = new QMoudBusCtrl(this);
	on_pbRefresh_clicked();
	timeSend = new QTimer();
	connect(timeSend, &QTimer::timeout, this, &QDeviceCtrl::on_delaySend);
	qRegisterMetaType<QModbusDataUnit>("QModbusDataUnit");
	connect(this, &QDeviceCtrl::sigArealdSend, this, &QDeviceCtrl::on_sendMdu);
	timeGetPower = new  QTimer();
	connect(timeGetPower, &QTimer::timeout, this, &QDeviceCtrl::on_sendMdu);
	//timeGetPower->start(500);
	ui.label_12->setVisible(false);
	ui.label_14->setVisible(false);
	ui.label_16->setVisible(false);
	ui.label_Volt->setVisible(false);
	ui.label_Current->setVisible(false);
	ui.label_Power->setVisible(false);
}

QDeviceCtrl::~QDeviceCtrl()
{
	bRun = false;
	if (tcp)
	{
		if (tcp->state() == QAbstractSocket::ConnectedState)
			tcp->disconnectFromHost();
		//tcp->deleteLater();
	}
}
void QDeviceCtrl::on_pbGrasp_clicked(bool isClicked)
{
	if (tcp->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_Grasp_1 + IOON;
	else
		sendStr = Head + IO_Grasp_1 + IOOFF;
	tcp->write(sendStr.toStdString().data());
		
}
void QDeviceCtrl::on_pbUpDown_clicked(bool isClicked)
{
	if (tcp->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head+ IO_UpDown_1+ IOON;
	else
		sendStr = Head + IO_UpDown_1 + IOOFF;
	tcp->write(sendStr.toUtf8().data());
}
void QDeviceCtrl::on_pbCover_clicked(bool isClicked)
{
	if (tcp->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_Cover_1 + IOON;
	else
		sendStr = Head + IO_Cover_1 + IOOFF;
	tcp->write(sendStr.toUtf8().data());
}
void QDeviceCtrl::on_pbColdWater_clicked(bool isClicked)
{
	if (tcp->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_Cold_1 + IOON;
	else
		sendStr = Head + IO_Cold_1 + IOOFF;
	tcp->write(sendStr.toUtf8().data());
}
void QDeviceCtrl::on_pbPureWater_clicked(bool isClicked)
{
	if (tcp->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_Pure_1 + IOON;
	else
		sendStr = Head + IO_Pure_1 + IOOFF;
	tcp->write(sendStr.toUtf8().data());
}
void QDeviceCtrl::on_pbBlowWater_clicked(bool isClicked)
{
	if (tcp->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_Blow_1 + IOON;
	else
		sendStr = Head + IO_Blow_1 + IOOFF;
	tcp->write(sendStr.toUtf8().data());
}

/****************************************************
* 2#工位
******************************************************/
void QDeviceCtrl::on_pbGrasp_2_clicked(bool isClicked)
{
	if (tcp->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_Grasp_2 + IOON;
	else
		sendStr = Head + IO_Grasp_2 + IOOFF;
	tcp->write(sendStr.toStdString().data());

}
void QDeviceCtrl::on_pbUpDown_2_clicked(bool isClicked)
{
	if (tcp->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_UpDown_2 + IOON;
	else
		sendStr = Head + IO_UpDown_2 + IOOFF;
	tcp->write(sendStr.toUtf8().data());
}
void QDeviceCtrl::on_pbCover_2_clicked(bool isClicked)
{
	if (tcp->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_Cover_2 + IOON;
	else
		sendStr = Head + IO_Cover_2 + IOOFF;
	tcp->write(sendStr.toUtf8().data());
}
void QDeviceCtrl::on_pbColdWater_2_clicked(bool isClicked)
{
	if (tcp->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_Cold_2 + IOON;
	else
		sendStr = Head + IO_Cold_2 + IOOFF;
	tcp->write(sendStr.toUtf8().data());
}
void QDeviceCtrl::on_pbPureWater_2_clicked(bool isClicked)
{
	if (tcp->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_Pure_2 + IOON;
	else
		sendStr = Head + IO_Pure_2 + IOOFF;
	tcp->write(sendStr.toUtf8().data());
}
void QDeviceCtrl::on_pbBlowWater_2_clicked(bool isClicked)
{
	if (tcp->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_Blow_2 + IOON;
	else
		sendStr = Head + IO_Blow_2 + IOOFF;
	tcp->write(sendStr.toUtf8().data());
}

/****************************************************
* 3#工位
******************************************************/
void QDeviceCtrl::on_pbGrasp_3_clicked(bool isClicked)
{
	if (tcp->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_Grasp_3 + IOON;
	else
		sendStr = Head + IO_Grasp_3 + IOOFF;
	tcp->write(sendStr.toStdString().data());

}
void QDeviceCtrl::on_pbUpDown_3_clicked(bool isClicked)
{
	if (tcp->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_UpDown_3 + IOON;
	else
		sendStr = Head + IO_UpDown_3 + IOOFF;
	tcp->write(sendStr.toUtf8().data());
}
void QDeviceCtrl::on_pbCover_3_clicked(bool isClicked)
{
	if (tcp->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_Cover_3 + IOON;
	else
		sendStr = Head + IO_Cover_3 + IOOFF;
	tcp->write(sendStr.toUtf8().data());
}
void QDeviceCtrl::on_pbColdWater_3_clicked(bool isClicked)
{
	if (tcp->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_Cold_3 + IOON;
	else
		sendStr = Head + IO_Cold_3 + IOOFF;
	tcp->write(sendStr.toUtf8().data());
}
void QDeviceCtrl::on_pbPureWater_3_clicked(bool isClicked)
{
	if (tcp->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_Pure_3 + IOON;
	else
		sendStr = Head + IO_Pure_3 + IOOFF;
	tcp->write(sendStr.toUtf8().data());
}
void QDeviceCtrl::on_pbBlowWater_3_clicked(bool isClicked)
{
	if (tcp->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_Blow_3 + IOON;
	else
		sendStr = Head + IO_Blow_3 + IOOFF;
	tcp->write(sendStr.toUtf8().data());
}
void QDeviceCtrl::on_pbTcpConnect_clicked(bool isClicked)
{
	if (!tcp)
		return;
	if (!isClicked)
	{
		if (tcp->state() == QAbstractSocket::ConnectedState)
		{
			tcp->disconnectFromHost();
		}
	}
	else
	{
		if (tcp->state() == QAbstractSocket::ConnectedState)
		{
			tcp->disconnectFromHost();
		}
		QString ipadd = ui.leIpAdd->text().trimmed();
		uint16_t iport = ui.lePort->text().toUShort();
		if (ipadd.isEmpty())
			ipadd = ipAddress;
		if (iport == 0)
			iport = port;
		tcp->connectToHost(ipadd, iport);
	}
	

}
void QDeviceCtrl::on_SocketStateChanged(QAbstractSocket::SocketState bb)
{
	if (bb == QAbstractSocket::ConnectedState)
	{
		if (!bRun)
			return;
		QLOG_INFO() << "PLC connected!";
		ui.leIpAdd->setEnabled(false);
		ui.lePort->setEnabled(false);
		ui.pbTcpConnect->setChecked(true);
	}
	else if (bb==QAbstractSocket::UnconnectedState)
	{
		if (!bRun)
			return;
		QLOG_INFO() << "PLC connected failure";
		ui.leIpAdd->setEnabled(true);
		ui.lePort->setEnabled(true);
		ui.pbTcpConnect->setChecked(false);
	}
	else if (bb==QAbstractSocket::ClosingState)
	{
		if (!bRun)
			return;
		QLOG_INFO() << "PLC connected close";
		ui.leIpAdd->setEnabled(true);
		ui.lePort->setEnabled(true);
		ui.pbTcpConnect->setChecked(false);
	}
}
void QDeviceCtrl::getInStateRun()
{
	while (bRun)
	{
		using namespace std::chrono_literals;		//这个命名空间牛逼啊，加入了时间单位的后缀的字面量运算符
		std::this_thread::sleep_for(10ms);
		if (!bRun)
			return;
		if (!tcp)
			continue;
		if (tcp->state() != QAbstractSocket::ConnectedState)
			continue;
		QString str = InHead + InSatrtBit_00 + InNum;
		emit timeToSend(str,0);
		Sleep(100);
		str = InHead + InSatrtBit_10 + InNum;
		emit timeToSend(str, 1);
		Sleep(100);
		str = InHead + InSatrtBit_20 + InNum;
		emit timeToSend(str, 2);
		Sleep(100);
		//tcp->waitForReadyRead();
		
	}
}
void QDeviceCtrl::getPowerRun()
{
	return;
	while (bRun)
	{
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(500ms);

		if (!moudBus)
			continue;
		if (!moudBus->connectState())
			continue;
		auto mdu = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, 0x0, 6);
		while (!moudBus->isEndSendFream())
		{
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(5ms);
		}
		std::this_thread::sleep_for(100ms);
		emit sigArealdSend(mdu);
		//Sleep(500);
		
	}
	
}
void QDeviceCtrl::setOffPower()
{
	auto temp = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, 0x7, 5);
	temp.setValue(4, 0x0000);
	moudBus->sendWriteMdu(temp, 0x1);
}
void QDeviceCtrl::_readyRead()
{
	
}

void QDeviceCtrl::on_timeToSend(QString str,int num)
{
	tcp->write(str.toUtf8().data());
	tcp->waitForReadyRead(1000);
	QByteArray data = tcp->readAll();
	int lenght = data.size();
	if (lenght == 29)
	{
		QString dd = data;
		QString res = dd.mid(18, 4);
		if (res != "0000")
			return;
		QString inState = dd.mid(22, 7);
		if (num == 0)
		{
			for (int i = 0; i < 7; i++)
			{
				bInState_One[i] = QString(inState.at(i)).toInt();
				checkOne[i]->setChecked(bInState_One[i]);
				QString str = bInState_One[i] ? strRed : strGreen;
				checkOne[i]->setStyleSheet(str);
			}
		}
		else if (num == 1)
		{
			for (int i = 0; i < 7; i++)
			{
				bInState_Two[i] = QString(inState.at(i)).toInt();
				checkTwo[i]->setChecked(bInState_Two[i]);
				QString str = bInState_Two[i] ? strRed : strGreen;
				checkTwo[i]->setStyleSheet(str);
				
			}
		}
		else if (num == 2)
		{
			for (int i = 0; i < 7; i++)
			{
				bInState_Three[i] = QString(inState.at(i)).toInt();
				checkThree[i]->setChecked(bInState_Three[i]);
				QString str = bInState_Three[i] ? strRed : strGreen;
				checkThree[i]->setStyleSheet(str);
			}
		}

	}
}

void QDeviceCtrl::on_pbSetVoltAndCurr_clicked()
{
	//setOffPower();
	auto mdu = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, 0x7, 5);
	uint volt = ui.spinBoxVolt->text().toUInt() * 1000;
	uint low8bit;
	uint hight8bit;
	uint low8bit2;
	uint hight8bit2;
	uint current = ui.spinBoxCurrent->text().toDouble()*1000;
	QString volt_hex = QString("%1").arg(volt, 8, 16, QLatin1Char('0'));
	low8bit = volt_hex.right(4).toInt(nullptr, 16);
	hight8bit = volt_hex.left(4).toInt(nullptr, 16);
	QString currl_hex = QString("%1").arg(current, 8, 16, QLatin1Char('0'));
	low8bit2 = currl_hex.right(4).toInt(nullptr, 16);
	hight8bit2 = currl_hex.left(4).toInt(nullptr, 16);
	mdu.setValue(0, qint16(0));
	mdu.setValue(1, qint16(0));
	mdu.setValue(2, qint16(0));
	mdu.setValue(3, qint16(0));
	mdu.setValue(4, qint16(0x0000));
	
	moudBus->sendWriteMdu(mdu, 0x1);
	mdu.setValue(0, qint16(hight8bit));
	mdu.setValue(1, qint16(low8bit));
	mdu.setValue(2, qint16(hight8bit2));
	mdu.setValue(3, qint16(low8bit2));
	mdu.setValue(4, qint16(0xFF00));
	m_mdu = mdu;
	timeSend->start(1500);
}

void QDeviceCtrl::on_delaySend()
{
	
	moudBus->sendWriteMdu(m_mdu, 0x1);
	timeSend->stop();
}

void QDeviceCtrl::on_pbConnectRTU_clicked(bool isCheck)
{
	if (isCheck)
	{
		if (ui.cbSerialPort->currentIndex() < 0)
		{
			QLOG_INFO() << "No Device!";
			return;
		}
		QString serialName = ui.cbSerialPort->currentText();
		bool b = moudBus->connectToDevice(serialName, ui.cbPaity->currentText().toInt(), ui.cbBudRate->currentText().toUInt(), ui.cbStopBit->currentText().toUInt(), ui.cbDataBit->currentText().toUInt());
		if (!b)
		{
			QLOG_INFO() << "connect to  Device error!";
			return;
		}
	}
	else
	{
		moudBus->disConnectDevice();
	}
	

}

void QDeviceCtrl::on_spinBoxVolt_editingFinished()
{
	auto mdu = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, 0x7, 2);
	uint volt = ui.spinBoxVolt->text().toUInt() * 1000;
	uint low8bit;
	uint hight8bit;
	QString volt_hex = QString("%1").arg(volt, 8, 16, QLatin1Char('0'));
	low8bit = volt_hex.right(4).toInt(nullptr, 16);
	hight8bit = volt_hex.left(4).toInt(nullptr, 16);
	mdu.setValue(0, qint16(hight8bit));
	mdu.setValue(1, qint16(low8bit));
	/*moudBus->sendWriteMdu(mdu, 0x1);
	Sleep(500);*/
}
void QDeviceCtrl::on_spinBoxCurrent_editingFinished()
{
	auto mdu = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, 0x9, 2);
	uint volt = ui.spinBoxCurrent->text().toDouble() * 1000;
	uint low8bit;
	uint hight8bit;
	QString Current_hex = QString("%1").arg(volt, 8, 16, QLatin1Char('0'));
	low8bit = Current_hex.right(4).toInt(nullptr, 16);
	hight8bit = Current_hex.left(4).toInt(nullptr, 16);
	mdu.setValue(0, qint16(hight8bit));
	mdu.setValue(1, qint16(low8bit));
	/*moudBus->sendWriteMdu(mdu, 0x1);
	Sleep(500);*/
}

void QDeviceCtrl::on_pbOffVolt_clicked()
{
	setOffPower();
}

void QDeviceCtrl::on_pbRefresh_clicked()
{
	for (int i = 0; i < moudBus->getSerialInfo().size(); i++)
	{
		ui.cbSerialPort->addItem(moudBus->getSerialInfo().at(i).portName());
	}
}

void QDeviceCtrl::on_sendMdu()
{
	if (!moudBus->connectState())
		return;
	auto mdu = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, 0x0, 6);
	QModbusDataUnit tmdu = moudBus->sendReadMdu(mdu, 0x1);
	if (tmdu.registerType() != QModbusDataUnit::HoldingRegisters || tmdu.valueCount()<6)
	{
		ui.label_Volt->setText("0 V");
		ui.label_Current->setText("0 A");
		ui.label_Power->setText("0 kW");
		return;
	}
	int volt = (tmdu.value(0) * 65535 + tmdu.value(1)) / 1000;
	float current = (tmdu.value(2) * 65535 + tmdu.value(3)) / 1000.0;
	float powerState = (tmdu.value(4) * 65535 + tmdu.value(5)) / 1000.0;
	ui.label_Volt->setText(QString::number(volt)+" V");
	ui.label_Current->setText(QString::number(current) + " A");
	ui.label_Power->setText(QString::number(powerState)+" kW");
		

}
