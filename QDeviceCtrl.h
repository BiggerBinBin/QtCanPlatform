#pragma once

#include <QWidget>
#include "ui_QDeviceCtrl.h"
#include <qtcpsocket.h>
#include <qcheckbox.h>
#include "QMoudBusCtrl.h"
#include <QTimer>
#include <QModbusDataUnit>
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
	QMoudBusCtrl* moudBus = nullptr;
	//PLC ��ַ�Ͷ˿ں�
	QString ipAddress = "192.168.200.10";
	uint16_t port = 1001;
	bool bInState_One[7] = {false};
	bool bInState_Two[7] = { false };
	bool bInState_Three[7] = { false };
	QString bInName_One[7] = {"��ͣ","��λ","�������","����","1#����-��","1#����-��","1#��ԭλ"};
	QString bInName_Two[7] = {"2#����-��","2#����-��","2#��ԭλ","NONE","3#����-��","3#����-��","3#��ԭλ"};
	QString bInName_Three[7] = {"1#�а�ť","2#�а�ť","3#�а�ť","��դ","1#ȷ�Ϲ���","2#ȷ�Ϲ���","3#ȷ�Ϲ���"};
	QCheckBox* checkOne[7] = { nullptr };
	QCheckBox* checkTwo[7] = { nullptr };
	QCheckBox* checkThree[7] = { nullptr };

	QTimer* timeSend = nullptr;
	QTimer* timeGetPower = nullptr;
	QModbusDataUnit m_mdu;
	//���̺߳�������ȡPLC�е�������ź�
	void getInStateRun();
	//���̺߳�������ȡ��ѹ��Դ�ĵ�ѹ��������
	void getPowerRun();

	void setOffPower();
signals:
	void timeToSend(QString str,int num);
	void sigArealdSend(QModbusDataUnit mdu);
public slots:

	//1#��λ
	void on_pbGrasp_clicked(bool isClicked);
	void on_pbUpDown_clicked(bool isClicked);
	void on_pbCover_clicked(bool isClicked);
	void on_pbColdWater_clicked(bool isClicked);
	void on_pbPureWater_clicked(bool isClicked);
	void on_pbBlowWater_clicked(bool isClicked);

	//2#��λ
	void on_pbGrasp_2_clicked(bool isClicked);
	void on_pbUpDown_2_clicked(bool isClicked);
	void on_pbCover_2_clicked(bool isClicked);
	void on_pbColdWater_2_clicked(bool isClicked);
	void on_pbPureWater_2_clicked(bool isClicked);
	void on_pbBlowWater_2_clicked(bool isClicked);

	//3#��λ
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

	//���õ�����ѹ
	void on_pbSetVoltAndCurr_clicked();
	void on_delaySend();
	void on_pbConnectRTU_clicked(bool isCheck);

	//����spinBox��������ѹ
	void on_spinBoxVolt_editingFinished();
	void on_spinBoxCurrent_editingFinished();
	//�¸�ѹ
	void on_pbOffVolt_clicked();
	void on_pbRefresh_clicked();
private slots:
	void on_sendMdu();
	
};