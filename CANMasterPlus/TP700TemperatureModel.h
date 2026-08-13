#pragma once

#include <QWidget>
#include <QDialog>
#include <QTimer>
#include <QByteArray>
#include <QVector>
#include <qcombobox.h>
#include "ui_TP700TemperatureModel.h"
#include "MsgQueue.h"
class QMoudBusCtrl;

class TP700TemperatureModel : public QDialog
{
	Q_OBJECT

public:
	TP700TemperatureModel(QWidget *parent = nullptr);
	~TP700TemperatureModel();
	QVector<float> getTemperatur();
	float getTemperatur(QString ch);
	bool getIsUpdate() {
		return bDataUp;
	}
	QString getAliasName() { return aliasName; }
private:
	Ui::TP700TemperatureModelClass ui;
	QMoudBusCtrl* serialport_tp700 = nullptr; //TP700采温仪
	int serviceAddr = 0x1;
	int bitrate;
	QString portname;
	bool isSerialOK = false;
	QTimer timer;
	//通道名称
	QString aliasName;
	//要开启的通道
	QStringList channel;

	int indexData[32];
	//用于16进制转浮点数的
	union valReg
	{
		unsigned char data[4];
		float fval;
	};
	valReg vData[32];
	//True为可以读
	bool bDataUp = false;
	//寄存器数量
	
	bool m_debug = 0;
	QComboBox* cb = nullptr;
	QMap<QString, MsgQueue<float>>*m_temp=nullptr;
private:
	bool openSerial();
private Q_SLOTS:
	void on_pbStart_clicked(bool b);
	void on_timesend();
	void on_receive(QVector<uint16_t> data);
	void on_comboboxDevice_indexChanged(int index);
};
