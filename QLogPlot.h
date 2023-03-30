#pragma once

#include <QWidget>
#include "ui_QLogPlot.h"
#include "QBLFManage.h"
#include <QTimer>
#include "QSignalManage.h"
#include "qcustomplot.h"
class QLogPlot : public QWidget
{
	Q_OBJECT

public:
	QLogPlot(int index,QWidget *parent = nullptr);
	~QLogPlot();
	void setModelIndex(int index){ m_iModel = index; }
	virtual void resizeEvent(QResizeEvent* event) override;
private:
	Ui::QLogPlotClass ui;
	int m_iModel=-1;
	QBLFManage* BLfLog = nullptr;
	messageMange* LogData = nullptr;
	QTimer* m_tSendMes = nullptr;
	short int m_bRunSend = 1;
	QSignalManage* signalMan = nullptr;
	protoData pModelMes;
	QCustomPlot* plot = nullptr;
	QList<QCPAxisRect*>aixsRect;
	uint dataLimite = 50;
	UINT64 d_x = 0;
	QList<QColor>LableLineColor;
private:
	void runSendMes(float times);
	void plotItem();
	void InitGraphUI();
	void removeRect();
	float parseMesItem_intel(const protoItem& pItem,const QByteArray &data,int index,bool isSend);
	float parseMesItem_moto_msb(const protoItem& pItem,const QByteArray &data,int index,bool isSend);
	QStringList QByteToBinary(const QByteArray& data);
signals:
	void sigNewMessage(uint id, QByteArray data);
	void sinNewMessageInline(uint id, QByteArray data,int x);
private slots:
	void on_pbOpenFile_clicked();
	void on_pbSignal_clicked();
	void on_pbPlay_clicked();
	void on_pbPause_clicked();
	void on_pbStop_clicked();
	void on_GetBLFStaus(int n);
	void sendMessage();
	void on_showWidget();
	void on_AddData(ushort index, double x, double y);
	
public slots:
	void proLogData(uint id, QByteArray data, int x);
};
