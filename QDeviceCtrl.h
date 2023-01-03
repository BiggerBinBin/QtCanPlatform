#pragma once

#include <QWidget>
#include "ui_QDeviceCtrl.h"
#include <qtcpsocket.h>
#include <qcheckbox.h>
#include "QMoudBusCtrl.h"
#include <QTimer>
#include <QModbusDataUnit>
#include "PCAN.h"
#include "QProcessBuild.h"
#include "QAutoTestIniData.h"
#include "mHttp.h"
#pragma execution_character_set("utf-8")  
class QDeviceCtrl : public QWidget
{
	Q_OBJECT

public:
	QDeviceCtrl(QWidget *parent = nullptr);
	~QDeviceCtrl();

private:
	Ui::QDeviceCtrlClass ui;
	QTcpSocket* tcp = nullptr;
	PCAN* waterCan = nullptr;
	QAutoTestIniData* autoTestWidget = nullptr;
	QMoudBusCtrl* moudBus = nullptr;
	//PLC 地址和端口号
	QString ipAddress = "192.168.200.10";
	uint16_t port = 1001;
	/*bool bInState_One[7] = {false};
	bool bInState_Two[7] = { false };
	bool bInState_Three[7] = { false };*/
	QString bInName_One[7] = {"急停","复位","报警清除","启动","1#升降-上","1#升降-下","1#夹原位"};
	QString bInName_Two[7] = {"2#升降-上","2#升降-下","2#夹原位","NONE","3#升降-上","3#升降-下","3#夹原位"};
	QString bInName_Three[7] = {"1#夹按钮","2#夹按钮","3#夹按钮","光栅","1#确料光纤","2#确料光纤","3#确料光纤"};
	QCheckBox* checkOne[7] = { nullptr };
	QCheckBox* checkTwo[7] = { nullptr };
	QCheckBox* checkThree[7] = { nullptr };

	QTimer* timeSend = nullptr;
	QTimer* timeGetPower = nullptr;
	QModbusDataUnit m_mdu;
	QModbusDataUnit m_mdu2;

	QProcessBuild* qProcessB = nullptr;
	//冷水机内循环状态，0关闭，1开启
	uint bitInCircle = 0;
	//冷水机外循环状态，0关闭，1开启
	uint bitOutCircle = 0;
	int inCircle = 1;
	int outCircle = 1;

	bool cbProcess1Check = false;
	bool cbProcess2Check = false;
	bool cbProcess3Check = false;

	mHttp* mp = nullptr;;

	float reamHV_1 = 0;
	float reamHV_2 = 0;

	//连接状态,7代表3个设备都连接了，二进制：111
	//PLC为bit[2],冷水机为bit[1]，高压电源为bit[0]
	unsigned short int deviceState = 0;
	//多线程函数，读取PLC中的输入点信号
	void getInStateRun();
	//多线程函数，读取高压电源的电压电流功率
	void getPowerRun();
	void setOffPower();
	void getAnSetWaterRun();
public:
	unsigned short int getDeviceState() { return deviceState; }
	bool getProcess1State();
	bool getProcess2State();
	bool getProcess3State();
	float getHV_1() { return reamHV_1; }
	float getHV_2() { return reamHV_2; }
	void setWorkButton(int n);

	void setHV_1(int hv, float current);
	void setHV_2(int hv, float current);

	bool bInState_One[7] = { false };
	bool bInState_Two[7] = { false };
	bool bInState_Three[7] = { false };
	void closeSomething();
	bool outCycleState() { return outCircle; }
	bool inCycleState() { return inCircle; }
	void setResInLabel(int ch, QString str, QColor color);
	QString getPhuCode(int ch);
signals:
	void timeToSend(QString str,int num);
	void sigArealdSend(QModbusDataUnit mdu);
	void sigWorkRun(bool isCheck);
	void sigCanChanged();
public slots:

	//1#工位
	void on_pbGrasp_clicked(bool isClicked);
	void on_pbUpDown_clicked(bool isClicked);
	void on_pbCover_clicked(bool isClicked);
	void on_pbColdWater_clicked(bool isClicked);
	void on_pbPureWater_clicked(bool isClicked);
	void on_pbBlowWater_clicked(bool isClicked);

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
	void on_SocketStateChanged(QAbstractSocket::SocketState);
	void on_pbTcpConnect_clicked(bool isClicked);

	void _readyRead();
	void on_timeToSend(QString str,int num);

	//设置电流电压
	void on_pbSetVoltAndCurr_clicked();
	void on_delaySend();
	//连接串口
	void on_pbConnectRTU_clicked(bool isCheck);

	//两个spinBox，电流电压
	void on_spinBoxVolt_editingFinished();
	void on_spinBoxCurrent_editingFinished();
	//下高压
	void on_pbOffVolt_clicked();
	//刷新串口
	void on_pbRefresh_clicked();

	//接收冷水机报文
	void onReceiveData(unsigned int fream_id, QByteArray data);
	void on_pbStartInCricle_clicked(bool isCheck);
	void on_pbStartOutCricle_clicked(bool isCheck);
	void on_pbBlowWater_4_clicked(bool isCheck);

	//
	void on_dOpenCan_clicked(bool isCheck);
	void on_dCanRefresh_clicked();

	//打开流程设置
	void on_pbProcessSet_clicked(bool isCheck);
	void on_setProcessSetState(bool b);

	//工位的选择
	void on_dCbProcess1_stateChanged(int state);
	void on_dCbProcess2_stateChanged(int state);
	void on_dCbProcess3_stateChanged(int state);

	void on_linedSetp_editingFinished();
	void on_pbTestParamSet_clicked();
private slots:
	void on_lineEdit_1ResCode_editingFinished();
	void on_lineEdit_2ResCode_editingFinished();
	void on_lineEdit_3ResCode_editingFinished();
public slots:
	void on_sendMdu();
	
};
