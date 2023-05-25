#include "AutoDeviceManage.h"
#include <qmessagebox.h>
#include "QsLog.h"
#include <qdatetime.h>
#include <QTime>
#include <qeventloop.h>

AutoDeviceManage::AutoDeviceManage(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	m_bCoolMacInit = false;
	m_bPowerMacInit = false;
	m_pPowerTCP.reset(new QTcpSocket);
	m_pMesUpTCP.reset(new QTcpSocket);
	m_pPLCCtrTCP.reset(new QTcpSocket);
	connect(m_pPowerTCP.data(), &QTcpSocket::stateChanged, this, &AutoDeviceManage::on_powerTCP_stateChanged);
	connect(m_pMesUpTCP.data(), &QTcpSocket::stateChanged, this, &AutoDeviceManage::on_powerTCP_stateChanged);
	connect(m_pPLCCtrTCP.data(), &QTcpSocket::stateChanged, this, &AutoDeviceManage::on_powerTCP_stateChanged);
	connect(m_pPowerTCP.data(), &QTcpSocket::readyRead, this, &AutoDeviceManage::on_powerTCP_readlyRecvied);
	connect(m_pMesUpTCP.data(), &QTcpSocket::readyRead, this, &AutoDeviceManage::on_powerTCP_readlyRecvied);
	connect(m_pPLCCtrTCP.data(), &QTcpSocket::readyRead, this, &AutoDeviceManage::on_PLCCtrTCP_readlyRecvied);
	m_pWaterCAN.reset(new CANThread);
	connect(m_pWaterCAN.data(), SIGNAL(getProtocolData(int, quint32, QByteArray)), this, SLOT(onReceiveData(int, quint32, QByteArray)));
	m_pHttpMES.reset(new mHttp);
	ui.pbConnectPower->setCheckable(true);
	ui.pbOpenCanayst->setCheckable(true);
	m_bIsRecMES = false;
	initializeDev();
}

AutoDeviceManage::~AutoDeviceManage()
{
	if (!m_pPowerTCP.isNull())
	{
		m_pPowerTCP->disconnectFromHost();
	}
	if (!m_pMesUpTCP.isNull())
	{
		m_pMesUpTCP->disconnectFromHost();
	}
}
void AutoDeviceManage::on_pbConnectPower_clicked(bool b)
{
	if (m_pPowerTCP.isNull()) { QMessageBox::warning(this, "Tips", "指针为空,连接失败"); return; }
	if (b)
	{
		if (m_pPowerTCP->state() != QAbstractSocket::ConnectedState)
		{
			QString ipadd = ui.lineEdit_IPAddress->text();
			ushort port = ui.lineEdit_Port->text().toUShort();
			m_pPowerTCP->connectToHost(ipadd, port);
		}
	}
	else
	{
		m_pPowerTCP->disconnectFromHost();
	}
	
	
	
}
void AutoDeviceManage::on_pbConnectMes_clicked(bool b)
{
	if (m_pMesUpTCP.isNull()) { QMessageBox::warning(this, "Tips", "指针为空,连接失败"); return; }
	if (b)
	{
		if (m_pMesUpTCP->state() != QAbstractSocket::ConnectedState)
		{
			QString ipadd = ui.lineEdit_MesIPAdd->text();
			ushort port = ui.lineEdit_MesPort->text().toUShort();
			m_pMesUpTCP->connectToHost(ipadd, port);
		}
	}
	else
	{
		m_pMesUpTCP->disconnectFromHost();
	}
}
void AutoDeviceManage::on_pbConnectPLC_clicked(bool b)
{
	if(m_pPLCCtrTCP.isNull()) { QMessageBox::warning(this, "Tips", "指针为空,连接失败"); return; }
	if (b)
	{
		if (m_pPLCCtrTCP->state() != QAbstractSocket::ConnectedState)
		{
			QString ipadd = ui.lineEdit_PLCIPAdd->text();
			ushort port = ui.lineEdit_PLCPort->text().toUShort();
			m_pPLCCtrTCP->connectToHost(ipadd, port);
		}
	}
	else
	{
		m_bPLCMacInit = false;
		m_pPLCCtrTCP->disconnectFromHost();
	}
}
void AutoDeviceManage::on_pbSetVoltage_clicked()
{
	QString voltage = ui.lineEdit_Voltage->text().trimmed();
	voltage = "Voltage " + voltage + "\n";
	QByteArray arr;
	arr.append(voltage);
	m_pPowerTCP->write(arr);
}
void AutoDeviceManage::on_pbSetCurrent_clicked()
{
	QString current = ui.lineEdit_Current->text().trimmed();
	current = "Current:lim " + current + "\n";
	QByteArray arr;
	arr.append(current);
	m_pPowerTCP->write(arr);
}
void AutoDeviceManage::on_pbPowerOut_clicked(bool b)
{
	if (b)
	{
		QString voltage = ui.lineEdit_Voltage->text().trimmed();
		QString current = ui.lineEdit_Current->text().trimmed();
		QByteArray arr;
		current = "Current:lim " + current + "\n";
		arr.append(current);
		m_pPowerTCP->write(arr);
		QString output = "VOLTAGE "+voltage+";Output on\n";
		//QByteArray arr;
		QTime tt = QTime::currentTime().addMSecs(500);
		while (QTime::currentTime() < tt) {
			QApplication::processEvents();
		}
		arr.clear();
		arr.append(output);
		m_pPowerTCP->write(arr);
	}
	else
	{
		QString output = "Output off\n";
		QByteArray arr;
		arr.append(output);
		m_pPowerTCP->write(arr);
	}
	
}
void AutoDeviceManage::on_powerTCP_readlyRecvied()
{
	QTcpSocket* tcp = dynamic_cast<QTcpSocket*>(sender());
	if (tcp == m_pPowerTCP.data())
	{
		QByteArray res = m_pPowerTCP->readAll();
		QString data(res);
		emit sigPowerNewData(data);
	}
	else if (tcp == m_pMesUpTCP.data())
	{
		
		QByteArray res =  m_pMesUpTCP->readAll();
		QString data(res);
		m_bIsRecMES = true;
		m_strMesData = data;
		emit sigMesNewData(m_strMesData);
		
	}
}
void AutoDeviceManage::on_PLCCtrTCP_readlyRecvied()
{
	QByteArray bData =  m_pPLCCtrTCP->readAll();
	QLOG_INFO() <<"PLC Return size:" << bData.size();
	QLOG_INFO() << bData;
}
void AutoDeviceManage::on_powerTCP_stateChanged(QAbstractSocket::SocketState state)
{
	if (state == QAbstractSocket::UnconnectedState)
	{
		QTcpSocket* tcp = dynamic_cast<QTcpSocket*>(sender());
		if (tcp == m_pPowerTCP.data())
		{
			QLOG_WARN() << "Power supply not connected!";
			m_bPowerMacInit = false;
			ui.pbConnectPower->setChecked(false);
		}
		else if (tcp == m_pMesUpTCP.data())
		{
			QLOG_WARN() << "Mes System not connected!";
			ui.pbConnectMes->setChecked(false);
			m_bMesMacInit = false;
		}
		else if (tcp == m_pPLCCtrTCP.data())
		{
			ui.pbConnectPLC->setChecked(false);
			QLOG_WARN() << "PLC System not connected!";
			m_bPLCMacInit = false;
		}
		
	}
	else if (state == QAbstractSocket::ConnectedState)
	{
		QTcpSocket* tcp = dynamic_cast<QTcpSocket*>(sender());
		if (tcp == m_pPowerTCP.data())
		{
			QLOG_INFO() << "Power supply  connected!";
			ui.pbConnectPower->setChecked(true);
			m_bPowerMacInit = true;
		}
		else if (tcp == m_pMesUpTCP.data())
		{
			QLOG_WARN() << "Mes System connected!";
			ui.pbConnectMes->setChecked(true);
			m_bMesMacInit = true;
		}
		else if (tcp == m_pPLCCtrTCP.data())
		{
			ui.pbConnectPLC->setChecked(true);
			QLOG_WARN() << "PLC System  connected!";
			m_bPLCMacInit = true;
		}
	}
}
int AutoDeviceManage::getDeviceState()
{
	if (m_bPowerMacInit && m_bCoolMacInit && m_bPLCMacInit)
		return 0;
	else if (!(m_bCoolMacInit || m_bPowerMacInit))
		return -1;
	else if (!m_bCoolMacInit)
		return 1;
	else if (!m_bPowerMacInit)
		return 2;
	else if (!m_bPLCMacInit)
		return 3;

}
bool AutoDeviceManage::initializeDev()
{
	on_pbConnectMes_clicked(true);
	on_pbConnectPower_clicked(true);
	on_pbOpenCanayst_clicked(true);
	on_pbConnectPLC_clicked(true);
	return true;
}
void AutoDeviceManage::on_pbOpenCanayst_clicked(bool b)
{
	if (m_pWaterCAN.isNull())
	{
		m_bCoolMacInit = false;
		return;
	}
	if (b)
	{
		if (ui.dCanDevice_2->count() < 1)
		{
			on_pbCanRefresh_clicked();
		}
		if (ui.dCanDevice_2->count() < 1)
		{
			QLOG_WARN() << "No CANayst devices";
			ui.pbOpenCanayst->setChecked(false);
			m_bCoolMacInit = false;
			return;
		}
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
		bool b = m_pWaterCAN->openCAN(0, 0, bitRate);
		if (!b)
		{
			QLOG_WARN() << "Open CANayst failure";
			ui.pbOpenCanayst->setChecked(false);
			m_bCoolMacInit = false;
			return;
		}
		ui.pbOpenCanayst->setChecked(true);
		m_bCoolMacInit = true;
	}
	else
	{
		m_bCoolMacInit = false;
		m_pWaterCAN->closeCAN();
	}
	
}
void AutoDeviceManage::on_pbCanRefresh_clicked()
{
	QStringList n = m_pWaterCAN->DetectDevice();
	ui.dCanDevice_2->clear();
	for (int m = 0; m < n.size(); m++)
	{
		ui.dCanDevice_2->addItem("can" + n.at(m));
	}
}
void AutoDeviceManage::requireHttpMes(QString url,QStringList dList)
{
	
}
void AutoDeviceManage::requireTcpPower(QString data)
{
	QByteArray sendData;
	sendData.append(data);
	m_pPowerTCP->write(sendData);
}
void AutoDeviceManage::requireTcpMes(QString data)
{
	
	if (m_pMesUpTCP->state() != QAbstractSocket::ConnectedState)
	{
		on_pbConnectMes_clicked(true);	//if not connection,connect it
		QTime curt = QTime::currentTime().addMSecs(500);//get it delay 500msecs
		while (QTime::currentTime() < curt)
		{
			if (m_pMesUpTCP->state() == QAbstractSocket::ConnectedState)
				break;
		}
	}
	
	//no connected,return
	if (m_pMesUpTCP->state() != QAbstractSocket::ConnectedState)
	{
		return;
	}
	QByteArray send;
	send.append(data);
	m_pMesUpTCP->write(send);
	
}
void AutoDeviceManage::requireTcpPLC(QString data)
{
	if (m_pPLCCtrTCP->state() != QAbstractSocket::ConnectedState)
	{
		on_pbConnectMes_clicked(true);	//if not connection,connect it
		QTime curt = QTime::currentTime().addMSecs(500);//get it delay 500msecs
		while (QTime::currentTime() < curt)
		{
			if (m_pPLCCtrTCP->state() == QAbstractSocket::ConnectedState)
				break;
		}
	}
	//no connected,return
	if (m_pPLCCtrTCP->state() != QAbstractSocket::ConnectedState)
	{
		return;
	}
	QByteArray send;
	send.append(data);
	m_pPLCCtrTCP->write(send);
}
void AutoDeviceManage::requireCanWater(uint id, uchar data[8])
{
	m_pWaterCAN->sendData(id, data);
	//return QByteArray();
}
void AutoDeviceManage::on_waterCAN_readlyRecived(int ch, quint32 id, QByteArray data)
{

}
bool AutoDeviceManage::setCoolantTemp(int temp2, int flow2,bool cooling, bool out)
{
	
	uchar data[8];
	memset(data, 0, 8 * sizeof(uchar));
	//外循环开关在第6个字节的第1位（从0开始）
	
	//第6个字节，第0位+第1位
	uchar bOnorOff;
	if (cooling && out) bOnorOff = 3;
	else if (cooling) bOnorOff = 1;
	else bOnorOff = 2;
	data[6] = bOnorOff;
	int temp = temp2;
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

	int flow = flow2*10;
	l8bit = 0xFF & flow;
	h8bit = flow >> 8;
	data[3] = h8bit;
	data[2] = l8bit;
	m_pWaterCAN->sendData(0x01, data,false);
	return true;
}
void AutoDeviceManage::onReceiveData(int ch , quint32 fream_id, QByteArray data)
{
	if (5 == fream_id)
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
			temp_o = -1 * (hex / 10.0);
		}
		else
		{
			temp_o = ((float)(data[0]) * 16.0 * 16.0 + (float)(data[1])) / 10.0;		//冷水机出水温度,拼接高字节与低字节
		}

		if ((uchar)data[2] > 0xF)
		{
			QString str = QString("%1%2").arg((uchar)data[2], 2, 16, QLatin1Char('0')).arg((uchar)data[3], 2, 16, QLatin1Char('0'));
			int hex = 0xFFFF - str.toInt(nullptr, 16);
			temp_b = -1 * (hex / 10.0);
		}
		else
		{
			temp_b = ((float)(data[2]) * 16.0 * 16.0 + (float)(data[3])) / 10.0;		//冷水机回水温度,拼接高字节与低字节
		}
		ui.label_dInTempture->setText(QString::number(temp_o, (char)103, 2) + " °C");
		ui.label_dOutTempture->setText(QString::number(temp_b, (char)103, 2) + " °C");
	}
	else if (6 == fream_id)
	{
		float temp_o = ((float)(data[0]) * 16.0 * 16.0 + (float)(data[1])) / 10.0;		//冷水机流量,拼接高字节与低字节
		ui.label_dFollow->setText(QString::number(temp_o, (char)103, 2) + " L/min");
	}
	else if (7 == fream_id)
	{
		QString str = QString("%1%2").arg((uchar)data[0], 8, 2, QLatin1Char('0'));
		inCircle = str.mid(7, 1).toInt(nullptr, 2);
		outCircle = str.mid(6, 1).toInt(nullptr, 2);
		blowState = str.mid(2,1).toInt(nullptr, 2);
		int warm = str.mid(3, 1).toInt(nullptr, 2);

		QString inback = inCircle ? strGreen : strRed;
		ui.label_inCircle->setStyleSheet(inback);
		if (ui.pbStartInCricle->isChecked() != inCircle)
			ui.pbStartInCricle->setChecked(inCircle);
		QString outback = outCircle ? strGreen : strRed;
		ui.label_outCircle->setStyleSheet(outback);
		if (ui.pbStartOutCricle->isChecked() != outCircle)
			ui.pbStartOutCricle->setChecked(outCircle);
		QString warmback = warm ? strRed : strGreen;
		ui.label_warm->setStyleSheet(warmback);

		QString blowState2 = blowState ? strGreen : strRed;
		ui.label_blowState->setStyleSheet(blowState2);
		if (ui.pbBlowWater->isChecked() != blowState)
			ui.pbBlowWater->setChecked(blowState);

	}
}
void AutoDeviceManage::on_pbStartInCricle_clicked(bool isCheck)
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
	m_pWaterCAN.data()->sendData(0x01, data, false);
}
void AutoDeviceManage::on_pbStartOutCricle_clicked(bool isCheck)
{
	ui.pbStartOutCricle->setChecked(isCheck);
	uchar data[8];
	memset(data, 0, 8 * sizeof(uchar));
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
	uchar h8bit = var >> 8;
	data[1] = h8bit;
	data[0] = l8bit;

	int flow = ui.lineEdit_inFollow->text().toInt() * 10;
	l8bit = 0xFF & flow;
	h8bit = flow >> 8;
	data[3] = h8bit;
	data[2] = l8bit;
	m_pWaterCAN.data()->sendData(0x01, data,false);
}
void AutoDeviceManage::on_pbBlowWater_clicked(bool isCheck)
{

	uchar data[8];
	memset(data, 0, 8 * sizeof(uchar));

	int temp = -20;
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

	if (isCheck)
	{
		data[6] = 0x5;
	}
	else
		data[6] = 0x1;
	m_pWaterCAN.data()->sendData(0x01, data);
}
void AutoDeviceManage::on_pbGrasp_clicked(bool isClicked)
{
	if (m_pPLCCtrTCP->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_Grasp_1 + IOON;
	else
		sendStr = Head + IO_Grasp_1 + IOOFF;
	m_pPLCCtrTCP->write(sendStr.toStdString().data());

}
void AutoDeviceManage::on_pbUpDown_clicked(bool isClicked)
{
	if (m_pPLCCtrTCP->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_UpDown_1 + IOON;
	else
		sendStr = Head + IO_UpDown_1 + IOOFF;
	m_pPLCCtrTCP->write(sendStr.toUtf8().data());
}
void AutoDeviceManage::on_pbCover_clicked(bool isClicked)
{
	if (m_pPLCCtrTCP->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_Cover_1 + IOON;
	else
		sendStr = Head + IO_Cover_1 + IOOFF;
	m_pPLCCtrTCP->write(sendStr.toUtf8().data());
}
void AutoDeviceManage::on_pbColdWater_clicked(bool isClicked)
{
	if (m_pPLCCtrTCP->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_Cold_1 + IOON;
	else
		sendStr = Head + IO_Cold_1 + IOOFF;
	m_pPLCCtrTCP->write(sendStr.toUtf8().data());
}
void AutoDeviceManage::on_pbPureWater_clicked(bool isClicked)
{
	if (m_pPLCCtrTCP->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_Pure_1 + IOON;
	else
		sendStr = Head + IO_Pure_1 + IOOFF;
	m_pPLCCtrTCP->write(sendStr.toUtf8().data());
}
void AutoDeviceManage::on_pbBlowWaterPLC_clicked(bool isClicked)
{
	if (m_pPLCCtrTCP->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_Blow_1 + IOON;
	else
		sendStr = Head + IO_Blow_1 + IOOFF;
	m_pPLCCtrTCP->write(sendStr.toUtf8().data());
}

/****************************************************
* 2#工位
******************************************************/
void AutoDeviceManage::on_pbGrasp_2_clicked(bool isClicked)
{
	if (m_pPLCCtrTCP->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_Grasp_2 + IOON;
	else
		sendStr = Head + IO_Grasp_2 + IOOFF;
	m_pPLCCtrTCP->write(sendStr.toStdString().data());

}
void AutoDeviceManage::on_pbUpDown_2_clicked(bool isClicked)
{
	if (m_pPLCCtrTCP->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_UpDown_2 + IOON;
	else
		sendStr = Head + IO_UpDown_2 + IOOFF;
	m_pPLCCtrTCP->write(sendStr.toUtf8().data());
}
void AutoDeviceManage::on_pbCover_2_clicked(bool isClicked)
{
	if (m_pPLCCtrTCP->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_Cover_2 + IOON;
	else
		sendStr = Head + IO_Cover_2 + IOOFF;
	m_pPLCCtrTCP->write(sendStr.toUtf8().data());
}
void AutoDeviceManage::on_pbColdWater_2_clicked(bool isClicked)
{
	if (m_pPLCCtrTCP->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_Cold_2 + IOON;
	else
		sendStr = Head + IO_Cold_2 + IOOFF;
	m_pPLCCtrTCP->write(sendStr.toUtf8().data());
}
void AutoDeviceManage::on_pbPureWater_2_clicked(bool isClicked)
{
	if (m_pPLCCtrTCP->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_Pure_2 + IOON;
	else
		sendStr = Head + IO_Pure_2 + IOOFF;
	m_pPLCCtrTCP->write(sendStr.toUtf8().data());
}
void AutoDeviceManage::on_pbBlowWater_2_clicked(bool isClicked)
{
	if (m_pPLCCtrTCP->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_Blow_2 + IOON;
	else
		sendStr = Head + IO_Blow_2 + IOOFF;
	m_pPLCCtrTCP->write(sendStr.toUtf8().data());
}

/****************************************************
* 3#工位
******************************************************/
void AutoDeviceManage::on_pbGrasp_3_clicked(bool isClicked)
{
	if (m_pPLCCtrTCP->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_Grasp_3 + IOON;
	else
		sendStr = Head + IO_Grasp_3 + IOOFF;
	m_pPLCCtrTCP->write(sendStr.toStdString().data());

}
void AutoDeviceManage::on_pbUpDown_3_clicked(bool isClicked)
{
	if (m_pPLCCtrTCP->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_UpDown_3 + IOON;
	else
		sendStr = Head + IO_UpDown_3 + IOOFF;
	m_pPLCCtrTCP->write(sendStr.toUtf8().data());
}
void AutoDeviceManage::on_pbCover_3_clicked(bool isClicked)
{
	if (m_pPLCCtrTCP->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_Cover_3 + IOON;
	else
		sendStr = Head + IO_Cover_3 + IOOFF;
	m_pPLCCtrTCP->write(sendStr.toUtf8().data());
}
void AutoDeviceManage::on_pbColdWater_3_clicked(bool isClicked)
{
	if (m_pPLCCtrTCP->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_Cold_3 + IOON;
	else
		sendStr = Head + IO_Cold_3 + IOOFF;
	m_pPLCCtrTCP->write(sendStr.toUtf8().data());
}
void AutoDeviceManage::on_pbPureWater_3_clicked(bool isClicked)
{
	if (m_pPLCCtrTCP->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_Pure_3 + IOON;
	else
		sendStr = Head + IO_Pure_3 + IOOFF;
	m_pPLCCtrTCP->write(sendStr.toUtf8().data());
}
void AutoDeviceManage::on_pbBlowWater_3_clicked(bool isClicked)
{
	if (m_pPLCCtrTCP->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString sendStr;
	if (isClicked)
		sendStr = Head + IO_Blow_3 + IOON;
	else
		sendStr = Head + IO_Blow_3 + IOOFF;
	m_pPLCCtrTCP->write(sendStr.toUtf8().data());
}

void AutoDeviceManage::on_timeToSend()
{
	if (m_pPLCCtrTCP->state() != QAbstractSocket::ConnectedState)
	{
		QLOG_INFO() << "PLC未连接";
		return;
	}
	QString str = InHead + InSatrtBit_00 + InNum;
	m_pPLCCtrTCP->write(str.toUtf8().data());
}
