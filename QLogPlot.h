#pragma once

#include <QWidget>
#include "ui_QLogPlot.h"
#include "QBLFManage.h"
#include <QTimer>
#include "QSignalManage.h"
#include "LogTrc.h"
#include "qcustomplot.h"
#include <qscopedpointer.h>
#include "DataSave.h"
class QLogPlot : public QWidget
{
	Q_OBJECT

public:
	QLogPlot(int index,QWidget *parent = nullptr);
	~QLogPlot();
	void setModelIndex(int index){ m_iModel = index; }
	virtual void resizeEvent(QResizeEvent* event) override;
	virtual void closeEvent(QCloseEvent* event) override;
private:
	Ui::QLogPlotClass ui;
	int m_iModel=-1;
	//QBLFManage* BLfLog = nullptr;
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
	QScopedPointer<LogMessage::LogBase>m_pLogSource;
	QScopedPointer<DataSave>m_dataSave;
private:
	void runSendMes(float times);
	void plotItem();
	void InitGraphUI();
	void removeRect();
	/*float parseMesItem_intel(const protoItem& pItem,const QByteArray &data,int index,bool isSend);
	float parseMesItem_moto_msb(const protoItem& pItem,const QByteArray &data,int index,bool isSend);*/
	QStringList QByteToBinary(const QByteArray& data);
	void runExportMsg(QString filepath);
Q_SIGNALS:
	void sigNewMessage(uint id, QByteArray data);
	void sinNewMessageInline(uint id, QByteArray data,const int& x);
	void sigDataIndexChange(int value);
	void sigCloseWindow();
private Q_SLOTS:
	void on_pbOpenFile_clicked();
	void on_pbSignal_clicked();
	void on_pbPlay_clicked();
	void on_pbPause_clicked(bool Checked);
	void on_pbStop_clicked();
	void on_GetBLFStaus(int n);
	void sendMessage();
	void on_showWidget();
	void on_AddData(ushort index, double x, double y);
	void on_setSlider(int value);
	void on_pbExportSignal_clicked();
	
	
public Q_SLOTS:
	void proLogData(uint id, QByteArray data,const int& x);
};
