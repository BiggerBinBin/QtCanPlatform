#pragma once

#include <QWidget>
#include "ui_AutoDeviceManage.h"
#include <qtcpsocket.h>
#include "canthread.h"
#include <QSharedPointer>
#include <QScopedPointer>
#include "mHttp.h"
#include <qmutex.h>
class AutoDeviceManage : public QWidget
{
	Q_OBJECT

public:
	AutoDeviceManage(QWidget *parent = nullptr);
	~AutoDeviceManage();
	//获取设备状态，0：All ok，1：冷水机不OK，2：电源不OK，-1：所有不OK
	int getDeviceState();
	bool initializeDev();
	void requireHttpMes(QString url, QStringList dList);
	void requireTcpPower(QString data);
	void requireTcpMes(QString data);
	void requireTcpPLC(QString data);
	void requireCanWater(uint id, uchar data[8]);
	bool setCoolantTemp(int temp, int flow,bool cooling, bool out);
	bool setCoolantBlow() { on_pbBlowWater_clicked(true); return true; }
	bool isMesInit() { return m_bMesMacInit; }
	bool isAvailableMes() { return m_bIsRecMES; }
	QString isReadMes() { return m_strMesData; }
	float getFlow() { return m_realflow; }
private:
	Ui::AutoDeviceManageClass ui;
protected:
	bool m_bCoolMacInit;
	bool m_bPowerMacInit;
	bool m_bMesMacInit;
	bool m_bPLCMacInit;

	//冷水机内循环状态，0关闭，1开启
	uint bitInCircle = 0;
	//冷水机外循环状态，0关闭，1开启
	uint bitOutCircle = 0;
	int inCircle = 1;
	int outCircle = 1;
	int blowState = 0;
	float m_realflow;
	QScopedPointer<QTcpSocket> m_pPowerTCP;
	QScopedPointer<QTcpSocket> m_pMesUpTCP;
	QScopedPointer<QTcpSocket> m_pPLCCtrTCP;
	QScopedPointer<CANThread> m_pWaterCAN;
	QScopedPointer<mHttp> m_pHttpMES;
	std::atomic_bool m_bIsRecMES;
	QString m_strMesData;
	QMutex mut1;
	QMutex mut2;
	QString strGreen = "background-color:green";
	QString strRed = "background-color:red";

	//
	/*****************************************************
* 控制IO_OUT,共有18个IO输出点
* 格式为Head+Name+On/Off
* 字符串为须为utf-8
******************************************************/
	QString Head = "500000FF03FF00001800041402000101M*0000";
	QString IO_UpDown_1 = "10";		//1#上下
	QString IO_Grasp_1 = "11";		//1#夹紧
	QString IO_Cover_1 = "12";		//1#密封圈
	QString IO_Cold_1 = "13";		//1#冷水阀
	QString IO_Pure_1 = "14";		//1#纯水阀
	QString IO_Blow_1 = "15";		//1#吹水阀

	QString IO_UpDown_2 = "16";		//2#上下
	QString IO_Grasp_2 = "17";		//2#夹紧
	QString IO_Cover_2 = "18";		//2#密封圈
	QString IO_Cold_2 = "19";		//2#冷水阀
	QString IO_Pure_2 = "20";		//2#纯水阀
	QString IO_Blow_2 = "21";		//2#吹水阀

	QString IO_UpDown_3 = "22";		//3#上下
	QString IO_Grasp_3 = "23";		//3#夹紧
	QString IO_Cover_3 = "24";		//3#密封圈
	QString IO_Cold_3 = "25";		//3#冷水阀
	QString IO_Pure_3 = "26";		//3#纯水阀
	QString IO_Blow_3 = "27";		//3#吹水阀
	QString IOON = "01";		//IO输出ON
	QString IOOFF = "00";		//IO输出OFF

	/*****************************************************
	* 读取IO输入点,共有21个IO输出点
	* 格式为Head+Name+On/Off
	* 字符串为须为utf-8
	******************************************************/
	QString InHead = "500000FF03FF000018000404010001X*0000";
	QString InSatrtBit_00 = "00";
	QString InSatrtBit_10 = "10";
	QString InSatrtBit_20 = "20";
	//QString InNum = "0007";
	QString InNum = "001E";
signals:
	void sigMesNewData(QString str);
	void sigPowerNewData(QString str);
	void sigFlowCool(float flow);
private slots:
	void on_pbConnectPower_clicked(bool b);
	void on_pbConnectMes_clicked(bool b);
	void on_pbConnectPLC_clicked(bool b);
	void on_pbSetVoltage_clicked();
	void on_pbSetCurrent_clicked();
	void on_pbPowerOut_clicked(bool b);
	void on_powerTCP_stateChanged(QAbstractSocket::SocketState state);
	void on_powerTCP_readlyRecvied();
	void on_PLCCtrTCP_readlyRecvied();
	void on_pbOpenCanayst_clicked(bool b);
	void on_pbCanRefresh_clicked();
	void on_waterCAN_readlyRecived(int, quint32, QByteArray);
public slots:
	//冷水机
	void on_pbBlowWater_clicked(bool isCheck);		//吹水
	void on_pbStartOutCricle_clicked(bool isCheck);		//外循环（开阀门）
	void on_pbStartInCricle_clicked(bool isCheck);		//内循环（制冷）
	void onReceiveData(int ch , quint32 fream_id, QByteArray data);

	//PLC
	//1#工位
	void on_pbGrasp_clicked(bool isClicked);
	void on_pbUpDown_clicked(bool isClicked);
	void on_pbCover_clicked(bool isClicked);
	void on_pbColdWater_clicked(bool isClicked);
	void on_pbPureWater_clicked(bool isClicked);
	void on_pbBlowWaterPLC_clicked(bool isClicked);

	//2#工位
	void on_pbGrasp_2_clicked(bool isClicked);
	void on_pbUpDown_2_clicked(bool isClicked);
	void on_pbCover_2_clicked(bool isClicked);
	void on_pbColdWater_2_clicked(bool isClicked);
	void on_pbPureWater_2_clicked(bool isClicked);
	void on_pbBlowWater_2_clicked(bool isClicked);

	//3#工位
	void on_pbGrasp_3_clicked(bool isClicked);
	void on_pbUpDown_3_clicked(bool isClicked);
	void on_pbCover_3_clicked(bool isClicked);
	void on_pbColdWater_3_clicked(bool isClicked);
	void on_pbPureWater_3_clicked(bool isClicked);
	void on_pbBlowWater_3_clicked(bool isClicked);

	void on_timeToSend();


};
