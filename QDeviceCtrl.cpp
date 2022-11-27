#include "QDeviceCtrl.h"
#include "QsLog.h"
#include <thread>
#include <chrono>
#include <Windows.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QMessageBox>
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
	std::thread t2(&QDeviceCtrl::getPowerRun, this);
	t2.detach();

	moudBus = new QMoudBusCtrl(this);
	on_pbRefresh_clicked();
	timeSend = new QTimer();
	connect(timeSend, &QTimer::timeout, this, &QDeviceCtrl::on_delaySend);
	qRegisterMetaType<QModbusDataUnit>("QModbusDataUnit");
	//connect(this, &QDeviceCtrl::sigArealdSend, this, &QDeviceCtrl::on_sendMdu);
	timeGetPower = new  QTimer();
	connect(timeGetPower, &QTimer::timeout, this, &QDeviceCtrl::on_sendMdu);

	waterCan = new PCAN(this);
	connect(waterCan, &PCAN::getProtocolData, this, &QDeviceCtrl::onReceiveData);
	QStringList canStr = waterCan->DetectDevice();
	for (int i = 0; i < canStr.size(); ++i)
	{
		ui.dCanDevice->addItem(canStr.at(i));
	}
	ui.dCanBundRate->setCurrentIndex(2);
	//timeGetPower->start(500);
	/*ui.label_12->setVisible(false);
	ui.label_14->setVisible(false);
	ui.label_16->setVisible(false);
	ui.label_Volt->setVisible(false);
	ui.label_Current->setVisible(false);
	ui.label_Power->setVisible(false);*/
}

QDeviceCtrl::~QDeviceCtrl()
{
	closeSomething();
}
void QDeviceCtrl::closeSomething()
{
	bRun = false;
	if (tcp)
	{
		tcp->disconnectFromHost();
		delete tcp;
		tcp = nullptr;
	}
	if (moudBus)
	{
		moudBus->disConnectDevice();
		delete moudBus;
		moudBus = nullptr;
	}
	if (waterCan)
	{
		waterCan->CloseCan();
		delete waterCan;
		waterCan = nullptr;
	}
	
	
}
void QDeviceCtrl::setResInLabel(int ch, QString str,QColor color)
{
	if (0 == ch)
	{
		ui.label_1Res->setText(str);
		ui.label_1Res->setStyleSheet("color:#" + QString::number(color.red(),16) + QString::number(color.green(),16) + QString::number(color.blue(),16));
	}
	else if (1 == ch)
	{
		ui.label_2Res->setText(str);
		ui.label_2Res->setStyleSheet("color:#" + QString::number(color.red(),16) + QString::number(color.green(),16) + QString::number(color.blue(),16));
	}
	else if (2 == ch)
	{
		ui.label_3Res->setText(str);
		ui.label_3Res->setStyleSheet("color:#" + QString::number(color.red(),16) + QString::number(color.green(),16) + QString::number(color.blue(),16));
	}
}
QString QDeviceCtrl::getPhuCode(int ch)
{
	if (1 == ch)
		return ui.lineEdit_1ResCode->text();
	else if (2 == ch)
		return ui.lineEdit_2ResCode->text();
	else if (3 == ch)
		return ui.lineEdit_3ResCode->text();
	else
		return QString();
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
		deviceState &= 0x3; //3为011
	}
	else
	{
		if (tcp->state() == QAbstractSocket::ConnectedState)
		{
			tcp->disconnectFromHost();
			deviceState &= 0x3; //3为011
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
		deviceState |= 0x4; //3为100
		QLOG_INFO() << "PLC connected!";
		ui.leIpAdd->setEnabled(false);
		ui.lePort->setEnabled(false);
		ui.pbTcpConnect->setChecked(true);
	}
	else if (bb==QAbstractSocket::UnconnectedState)
	{
		if (!bRun)
			return;
		deviceState &= 0x3; //3为011
		QLOG_INFO() << "PLC connected failure";
		ui.leIpAdd->setEnabled(true);
		ui.lePort->setEnabled(true);
		ui.pbTcpConnect->setChecked(false);
	}
	else if (bb==QAbstractSocket::ClosingState)
	{
		if (!bRun)
			return;
		deviceState &= 0x3; //3为011
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
	//return;
	while (bRun)
	{
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(500ms);
		if (!bRun)
			return;
		if (!moudBus)
			continue;
		if (!moudBus->connectState())
			continue;
		auto mdu = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, 0x0, 6);
		if (!bRun)
			return;
		while (!moudBus->isEndSendFream())
		{
			if (!bRun)
				return;
			if (!moudBus)return;
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(5ms);
		}
		std::this_thread::sleep_for(100ms);
		emit sigArealdSend(mdu);
		if (!moudBus)return;
		//Sleep(500);
		
	}
	
}
void QDeviceCtrl::setOffPower()
{
	auto temp = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, 0x7, 5);
	temp.setValue(4, 0x0000);
	if(ui.cbSelectMachine->currentIndex()==0 || ui.cbSelectMachine->currentIndex() == 1)
		moudBus->sendWriteMdu(temp, 0x1);
	QTime tt = QTime::currentTime().addMSecs(100);
	while (QTime::currentTime() < tt)
	{
		QApplication::processEvents();
	}
	if (ui.cbSelectMachine->currentIndex() == 0 || ui.cbSelectMachine->currentIndex() == 2)
		moudBus->sendWriteMdu(temp, 0x2);
}
void QDeviceCtrl::getAnSetWaterRun()
{
}
void QDeviceCtrl::_readyRead()
{
	
}

//读取PLC传感器
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
/*
* @brief:   设置高压电源的电流电压
*			此高压电源有两路输出，使用同一个串口，A路的服务器地址是0x1，B路为0x2
*			所以同样的操作，只需要把服务器地址改一下即可
* @param:	void
* @return:	void
*/
void QDeviceCtrl::on_pbSetVoltAndCurr_clicked()
{
	timeGetPower->stop();
	QTime ttt = QTime::currentTime().addMSecs(1200);
	while (QTime::currentTime() < ttt)
	{
		QApplication::processEvents();
	}
	//初始化（数据类型，起止地址，数量），数据类型有线圈，寄存器等。
	auto mdu = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, 0x7, 5);
	auto mdub = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, 0x7, 5);
	//获取界面上的电压流，需要乘1000，以便传输小数。
	uint volt = ui.spinBoxVolt->text().toUInt() * 1000;
	uint current = ui.spinBoxCurrent->text().toDouble() * 1000;
	uint volt2 = ui.spinBoxVolt_2->text().toUInt() * 1000;
	uint current2 = ui.spinBoxCurrent_2->text().toDouble() * 1000;
	//数据要拆分成2字节+2字节，高位在前，低位在后
	uint low8bit;
	uint hight8bit;
	uint low8bit2;
	uint hight8bit2;

	uint low8bit_b;
	uint hight8bit_b;
	uint low8bit2_b;
	uint hight8bit2_b;

	//拆分，这里使用更Low的方法，更快的是: 低位：0xFF&volt，高位：0xFF&(volt>>16)
	QString volt_hex = QString("%1").arg(volt, 8, 16, QLatin1Char('0'));
	low8bit = volt_hex.right(4).toInt(nullptr, 16);
	hight8bit = volt_hex.left(4).toInt(nullptr, 16);
	QString currl_hex = QString("%1").arg(current, 8, 16, QLatin1Char('0'));
	low8bit2 = currl_hex.right(4).toInt(nullptr, 16);
	hight8bit2 = currl_hex.left(4).toInt(nullptr, 16);

	low8bit2 = 0;
	hight8bit2 = 0;

	QString volt_hex_b = QString("%1").arg(volt2, 8, 16, QLatin1Char('0'));
	low8bit_b = volt_hex_b.right(4).toInt(nullptr, 16);
	hight8bit_b = volt_hex_b.left(4).toInt(nullptr, 16);
	QString currl_hex_b = QString("%1").arg(current2, 8, 16, QLatin1Char('0'));
	low8bit2_b = currl_hex_b.right(4).toInt(nullptr, 16);
	hight8bit2_b = currl_hex_b.left(4).toInt(nullptr, 16);

	low8bit2_b = 0;
	hight8bit2_b = 0;
	/*mdu.setValue(0, qint16(0));
	mdu.setValue(1, qint16(0));
	mdu.setValue(2, qint16(0));
	mdu.setValue(3, qint16(0));
	mdu.setValue(4, qint16(0x0000));*/
	/*if (0 == ui.cbSelectMachine->currentIndex())
	{
		QTime tt = QTime::currentTime().addMSecs(100);
		moudBus->sendWriteMdu(mdu, 0x1);
		while (QTime::currentTime() < tt)
		{
			QApplication::processEvents();
		}
		moudBus->sendWriteMdu(mdu, 0x2);

	}
	else if (1 == ui.cbSelectMachine->currentIndex())
	{
		moudBus->sendWriteMdu(mdu, 0x1);
	}
	else if (2 == ui.cbSelectMachine->currentIndex())
	{
		moudBus->sendWriteMdu(mdu, 0x2);
	}*/
		
	mdu.setValue(0, qint16(hight8bit));
	mdu.setValue(1, qint16(low8bit));
	mdu.setValue(2, qint16(hight8bit2));
	mdu.setValue(3, qint16(low8bit2));
	mdu.setValue(4, qint16(0xFF00));

	mdub.setValue(0, qint16(hight8bit_b));
	mdub.setValue(1, qint16(low8bit_b));
	mdub.setValue(2, qint16(hight8bit2_b));
	mdub.setValue(3, qint16(low8bit2_b));
	mdub.setValue(4, qint16(0xFF00));
	m_mdu = mdu;
	m_mdu2 = mdub;
	//timeSend->start(500);
	on_delaySend();
}
/*
* @brief: timeSend定时器响应槽函数，这是为了防止过快发送，导致线路堵塞
*/
void QDeviceCtrl::on_delaySend()
{



	if (0 == ui.cbSelectMachine->currentIndex())
	{
		QTime tt = QTime::currentTime().addMSecs(200);
		moudBus->sendWriteMdu(m_mdu, 0x1);
		while (QTime::currentTime() < tt)
		{
			QApplication::processEvents();
		}
		moudBus->sendWriteMdu(m_mdu2, 0x2);

	}
	else if (1 == ui.cbSelectMachine->currentIndex())
	{
		moudBus->sendWriteMdu(m_mdu, 0x1);
	}
	else if (2 == ui.cbSelectMachine->currentIndex())
	{
		moudBus->sendWriteMdu(m_mdu2, 0x2);
	}
	//moudBus->sendWriteMdu(m_mdu, 0x1);
	timeSend->stop();
	timeGetPower->start(1000);
}

void QDeviceCtrl::on_pbConnectRTU_clicked(bool isCheck)
{
	if (isCheck)
	{
		if (ui.cbSerialPort->currentIndex() < 0)
		{
			QLOG_INFO() << "No Device!";
			deviceState &= 0x6; //6为110
			return;
		}
		QString serialName = ui.cbSerialPort->currentText();
		bool b = moudBus->connectToDevice(serialName, ui.cbPaity->currentText().toInt(), ui.cbBudRate->currentText().toUInt(), ui.cbStopBit->currentText().toUInt(), ui.cbDataBit->currentText().toUInt());
		if (!b)
		{
			deviceState &= 0x6; //6为110
			QLOG_INFO() << "connect to  Device error!";
			return;
		}
		timeGetPower->start(1000);
		deviceState |= 0x1; //
	}
	else
	{
		timeGetPower->stop();
		deviceState &= 0x6; //6为110
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
	ui.cbSerialPort->clear();
	for (int i = 0; i < moudBus->getSerialInfo().size(); i++)
	{
		ui.cbSerialPort->addItem(moudBus->getSerialInfo().at(i).portName());
	}
}
void QDeviceCtrl::onReceiveData(unsigned int fream_id, QByteArray data)
{
	if (5==fream_id)
	{
		
		/*第一帧数据为：
		  字节	定义
			0	出口温度高字节
			1	出口温度低字节
			2	回口温度高字节
			3	回口温度低字节
			4	出口压力高字节
			5	出口压力低字节
			6	回口压力高字节
			7	回口压力低字节
		*/
		float temp_o;
		float temp_b;

		//负数的传输方法是：原数据+0xFFFF
		if ((uchar)data[0] > 0xF)
		{
			QString str = QString("%1%2").arg((uchar)data[0], 2, 16, QLatin1Char('0')).arg((uchar)data[1], 2, 16, QLatin1Char('0'));
			int hex = 0xFFFF - str.toInt(nullptr, 16);
			temp_o = -1*(hex / 10.0);
		}
		else
		{
			temp_o = ((float)(data[0]) * 16.0 * 16.0 + (float)(data[1])) / 10.0;		//冷水机出水温度,拼接高字节与低字节
		}

		if ((uchar)data[2] > 0xF)
		{
			QString str = QString("%1%2").arg((uchar)data[2], 2, 16, QLatin1Char('0')).arg((uchar)data[3], 2, 16, QLatin1Char('0'));
			int hex = 0xFFFF - str.toInt(nullptr, 16);
			temp_b = -1*(hex / 10.0);
		}
		else
		{
			temp_b = ((float)(data[2]) * 16.0 * 16.0 + (float)(data[3])) / 10.0;		//冷水机回水温度,拼接高字节与低字节
		}
		ui.label_dInTempture->setText(QString::number(temp_o, (char)103, 2)+ " °C");
		ui.label_dOutTempture->setText(QString::number(temp_b, (char)103, 2)+" °C");
	}
	else if (6 == fream_id)
	{
		float temp_o = ((float)(data[0]) * 16.0 * 16.0 + (float)(data[1])) / 10.0;		//冷水机流量,拼接高字节与低字节
		ui.label_dFollow->setText(QString::number(temp_o, (char)103, 2)+" L/min");
	}
	else if (7 == fream_id)
	{
		QString str=QString("%1%2").arg((uchar)data[0], 8, 2, QLatin1Char('0'));
		inCircle = str.mid(7, 1).toInt(nullptr, 2);
		outCircle = str.mid(6, 1).toInt(nullptr, 2);
		int warm = str.mid(3, 1).toInt(nullptr, 2);
		
		QString inback = inCircle ? strGreen : strRed;
		ui.label_inCircle->setStyleSheet(inback);

		QString outback = outCircle ? strGreen : strRed;
		ui.label_outCircle->setStyleSheet(outback);

		QString warmback = warm ? strRed : strGreen;
		ui.label_warm->setStyleSheet(warmback);

	}
}
void QDeviceCtrl::on_pbStartInCricle_clicked(bool isCheck)
{
	ui.pbStartInCricle->setChecked(isCheck);
	uchar data[8];
	memset(data, 0, 8 * sizeof(uchar));
	bitInCircle = isCheck ? 1 : 0;
	//第6个字节
	uchar bOnorOff = bitInCircle + bitOutCircle;
	data[6] = bOnorOff;
	int temp = ui.lineEdit_inTemp->text().toInt();
	int var = 0;
	if (temp < 0)
		var = 0xFFFF + temp * 10;
	else
		var = temp * 10;
	uchar l8bit = 0xFF & var;
	uchar h8bit = var >> 8;
	data[1] = h8bit;
	data[0] = l8bit;

	int flow = ui.lineEdit_inFollow->text().toInt() * 10;
	l8bit = 0xFF & flow;
	h8bit = flow >> 8;
	data[3] = h8bit;
	data[2] = l8bit;
	waterCan->SendFrame(0x1, data);
}
void QDeviceCtrl::on_pbStartOutCricle_clicked(bool isCheck)
{
	ui.pbStartOutCricle->setChecked(isCheck);
	uchar data[8];
	memset(data, 0, 8*sizeof(uchar));
	//外循环开关在第6个字节的第1位（从0开始）
	bitOutCircle = isCheck ? 2 : 0;
	//第6个字节，第0位+第1位
	uchar bOnorOff = bitInCircle + bitOutCircle;
	data[6] = bOnorOff;
	int temp = ui.lineEdit_inTemp->text().toInt();
	int var = 0;

	//若温度小于0，则用FFFF+温度
	if (temp < 0)
		var = 0xFFFF + temp * 10;	//要乘10，这样才能有小数
	else
		var = temp * 10;
	//低8位
	uchar l8bit = 0xFF & var;
	//高8位
	uchar h8bit = var >> 8;
	data[1] = h8bit;
	data[0] = l8bit;

	int flow = ui.lineEdit_inFollow->text().toInt()*10;
	l8bit = 0xFF & flow;
	h8bit = flow >> 8;
	data[3] = h8bit;
	data[2] = l8bit;
	waterCan->SendFrame(0x1, data);
}
void QDeviceCtrl::on_pbBlowWater_4_clicked(bool isCheck)
{
	
	uchar data[8];
	memset(data, 0, 8 * sizeof(uchar));
	if (isCheck)
	{
		data[6] = 0x4;
	}
	else
		data[6] = 0x0;
	waterCan->SendFrame(0x1, data);
}
void QDeviceCtrl::on_dOpenCan_clicked(bool isCheck)
{
	if (isCheck)
	{
		if (ui.dCanDevice->count() <= 0)
			return;
		int curindex = ui.dCanBundRate->currentIndex();
		int bitRate = 250;
		switch (curindex)
		{
		case 0:
			bitRate = 200; break;
		case 1:
			bitRate = 250; break;
		case 2:
			bitRate = 500; break;
		case 3:
			bitRate = 800; break;
		default:
			bitRate = 250;
			break;
		}
		bool b = waterCan->ConnectDevice(ui.dCanDevice->currentText(), bitRate);
		
		if (!b)
		{
			deviceState &= 0x5; //5为101
			QMessageBox::warning(NULL, tr("错误"), tr("打开CAN失败,请检测设备是否被占用或者已经连接？"));
			return;
		}
		deviceState |= 0x2;//2为 010
		ui.dOpenCan->setText(tr("关闭"));
		ui.dCanBundRate->setEnabled(false);
		ui.dCanDevice->setEnabled(false);
	}
	else
	{
		deviceState &= 0x5; //5为101
		waterCan->CloseCan();
		ui.dOpenCan->setText(tr("打开"));
		ui.dCanBundRate->setEnabled(true);
		ui.dCanDevice->setEnabled(true);
	}
	emit sigCanChanged();
	/*pbSend->setEnabled(true);
	pcanIsOpen = true;*/
}
void QDeviceCtrl::on_dCanRefresh_clicked()
{
	ui.dCanDevice->clear();
	QStringList canStr = waterCan->DetectDevice();
	for (int i = 0; i < canStr.size(); ++i)
	{
		ui.dCanDevice->addItem(canStr.at(i));
	}
}

void QDeviceCtrl::on_pbProcessSet_clicked(bool isCheck)
{
	ui.pbProcessSet->setChecked(false);
	emit sigWorkRun(isCheck);
	return;
	if (!qProcessB)
	{
		qProcessB = new QProcessBuild();
	}
	qProcessB->show();
}
void QDeviceCtrl::on_setProcessSetState(bool b)
{
	ui.pbProcessSet->setChecked(b);
}
/*
* @brief：响应多线程发出的信号，读电压电流寄存器
* @param：无
* @return：void
*/
void QDeviceCtrl::on_sendMdu()
{
	if (!moudBus->connectState())
		return;
	auto mdu = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, 0x0, 6);
	if (0 == ui.cbSelectMachine->currentIndex() || 1 == ui.cbSelectMachine->currentIndex())
	{
		QModbusDataUnit tmdu = moudBus->sendReadMdu(mdu, 0x1);
		if (tmdu.registerType() != QModbusDataUnit::HoldingRegisters || tmdu.valueCount() < 6)
		{
			ui.label_Volt->setText("0 V");
			ui.label_Current->setText("0 A");
			ui.label_Power->setText("0 kW");
			reamHV_1 = 0.0;
			return;
		}
		float volt = (tmdu.value(0) * 65535.0 + tmdu.value(1)) / 1000.0;
		reamHV_1 = volt;
		float current = (tmdu.value(2) * 65535.0 + tmdu.value(3)) / 1000.0;
		float powerState = (tmdu.value(4) * 65535.0 + tmdu.value(5)) / 1000.0;
		ui.label_Volt->setText(QString::number(volt, 'f', 1) + "V");
		ui.label_Current->setText(QString::number(current, 'f', 1) + "A");
		ui.label_Power->setText(QString::number(powerState, 'f', 1) + "kW");
	}
	if (0 == ui.cbSelectMachine->currentIndex() || 2 == ui.cbSelectMachine->currentIndex())
	{
		QTime tt = QTime::currentTime().addMSecs(100);
		while (QTime::currentTime() < tt)
		{
			QApplication::processEvents();
		}
		QModbusDataUnit tmdu = moudBus->sendReadMdu(mdu, 0x2);
		if (tmdu.registerType() != QModbusDataUnit::HoldingRegisters || tmdu.valueCount() < 6)
		{
			ui.label_Volt_2->setText("0 V");
			ui.label_Current_2->setText("0 A");
			ui.label_Power_2->setText("0 kW");
			reamHV_2 = 0.0;
			return;
		}
		float volt = (tmdu.value(0) * 65535.0 + tmdu.value(1)) / 1000.0;
		reamHV_2 = volt;
		float current = (tmdu.value(2) * 65535.0 + tmdu.value(3)) / 1000.0;
		float powerState = (tmdu.value(4) * 65535.0 + tmdu.value(5)) / 1000.0;
		ui.label_Volt_2->setText(QString::number(volt, 'f', 1) + "V");
		ui.label_Current_2->setText(QString::number(current, 'f', 1) + "A");
		ui.label_Power_2->setText(QString::number(powerState, 'f', 1) + "kW");
	}
	
		

}
void QDeviceCtrl::on_dCbProcess1_stateChanged(int state)
{
	cbProcess1Check = state == 2 ? true : false;
}
void QDeviceCtrl::on_dCbProcess2_stateChanged(int state)
{
	cbProcess2Check = state == 2 ? true : false;
}
void QDeviceCtrl::on_dCbProcess3_stateChanged(int state)
{
	cbProcess3Check = state == 2 ? true : false;
}
void QDeviceCtrl::on_linedSetp_editingFinished()
{
	int  setp = ui.linedSetp->text().toInt();
	if (setp < 1)
		setp = 1;
	ui.spinBoxVolt->setSingleStep(setp);
	ui.spinBoxVolt_2->setSingleStep(setp);
}
bool QDeviceCtrl::getProcess1State()
{
	return cbProcess1Check;
}
bool QDeviceCtrl::getProcess2State()
{
	return cbProcess2Check;
}
bool QDeviceCtrl::getProcess3State()
{
	return cbProcess3Check;
}
void QDeviceCtrl::setWorkButton(int n)
{
	if (0 == n)
	{
		ui.pbProcessSet->setChecked(false);
		
	}
}
void QDeviceCtrl::setHV_1(int hv, float current)
{
	ui.spinBoxVolt->setValue(hv);
	ui.spinBoxCurrent->setValue(current);
	on_pbSetVoltAndCurr_clicked();
}
void QDeviceCtrl::setHV_2(int hv, float current)
{
	ui.spinBoxVolt_2->setValue(hv);
	ui.spinBoxCurrent_2->setValue(current);
	on_pbSetVoltAndCurr_clicked();
}