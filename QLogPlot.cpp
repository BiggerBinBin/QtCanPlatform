#include "QLogPlot.h"
#include <qfiledialog.h>
#include "qGboleData.h"
#include <qmessagebox.h>
#include <qthread.h>
#include <QtConcurrent>
#include <QTime>
#include <QtGlobal>
#include "QsLog.h"
#include "./unit/MsgParser.h"
#include "LogAsc.h"
#include "AlgorithmSet.h"
#pragma execution_character_set("utf-8")
QLogPlot::QLogPlot(int index,QWidget *parent)
	: m_iModel(index), QWidget(parent)
{
	//this->setWindowFlag(Qt::Tool);
	ui.setupUi(this);
	this->ui.pbPlay->setEnabled(false);
	this->ui.pbPause->setEnabled(false);
	this->ui.pbStop->setEnabled(false);
	m_dataSave.reset(new DataSave(this));
	//this->setWindowFlags(this->windowFlags());
	InitGraphUI();
	
}

QLogPlot::~QLogPlot()
{
	m_bRunSend = 0;
	if (LogData)
	{
		delete LogData;
		LogData = nullptr;
	}
	if (signalMan)
	{
		delete signalMan;
		signalMan = nullptr;

	}
	
	if (plot)
	{
		removeRect();
		delete plot;
		plot = nullptr;
	}
}
void QLogPlot::resizeEvent(QResizeEvent* event)
{
	plot->setFixedSize(ui.lable_show->size());
}
void QLogPlot::closeEvent(QCloseEvent* event)
{
	event->accept();
	emit sigCloseWindow();
}
void QLogPlot::on_pbOpenFile_clicked()
{
	QString filepath = QFileDialog::getOpenFileName(NULL, "打开日志文件", QApplication::applicationDirPath(), QString("log文件(*.blf *.trc *.asc)"));
	if (filepath.isEmpty()) return;
	QFileInfo info(filepath);

	//判断文件类型，根据后缀名来调用不同的解析派生类
	if (info.suffix().toLower() == "blf")
	{
		m_pLogSource.reset(new QBLFManage()); 
	}
	else if (info.suffix().toLower() == "trc")
	{
		m_pLogSource.reset(new LogTrc());
		
	}
	else if (info.suffix().toLower() == "asc")
	{
		m_pLogSource.reset(new LogAsc());
	}
	connect(m_pLogSource.data(), &LogMessage::LogBase::sigLoadState, this, &QLogPlot::on_GetBLFStaus);
	qGboleData* qGb = qGboleData::getInstance();
	if (!qGb)
	{
		return;
	}
	if (m_iModel > qGb->pGboleData.size())
		return;

	QStringList fifter;
	for (int n = 0; n < qGb->pGboleData.at(m_iModel).cItem.size(); n++)
	{
		fifter << qGb->pGboleData.at(m_iModel).cItem.at(n).strCanId;
	}
	if (!LogData) LogData = new messageMange();
	LogData->vData.clear();
	LogData->iLen = 0;
	LogData->iIndex = 0;
	//BLfLog->getBLFLog(filepath, fifter, LogData);
	m_pLogSource->loadLogFile(filepath, fifter, LogData);
}
void QLogPlot::on_pbSignal_clicked()
{
	if (!signalMan)
	{
		signalMan = new QSignalManage(&pModelMes);
		connect(signalMan, &QSignalManage::QSiganlClose, this, &QLogPlot::on_showWidget);
	}
	signalMan->setCurrentModel(m_iModel);
	signalMan->show();
	this->setHidden(true);
}
void QLogPlot::on_pbPlay_clicked()
{
	int circle = this->ui.lineEdit_SetCircle->text().toInt();
	if (m_bRunSend == 2)
	{
		m_bRunSend = 1;
		return;
	}
	this->ui.pbPlay->setEnabled(false);
	plotItem();
	m_bRunSend = 1;
	//启动子线程发送数据
	QtConcurrent::run(this, &QLogPlot::runSendMes, circle);

}
void QLogPlot::on_pbPause_clicked(bool Checked)
{
	if(Checked)
		m_bRunSend = 2;
	else
		m_bRunSend = 1;
}
void QLogPlot::on_pbStop_clicked()
{
	if (this->ui.pbPause->isChecked())
		this->ui.pbPause->setChecked(false);
	if (!LogData)
		return;
	m_bRunSend = 0;
	LogData->iIndex = 0;
	this->ui.pbPlay->setEnabled(true);
}
void QLogPlot::on_GetBLFStaus(int n)
{
	if (n == 0 && LogData->iLen>0)
	{
		this->ui.pbPlay->setEnabled(true);
		this->ui.pbPause->setEnabled(true);
		this->ui.pbStop->setEnabled(true);
		ui.horizontalSlider->setValue(0);
		ui.horizontalSlider->setMaximum(LogData->iLen);
		ui.label_end->setText(QString::number(LogData->iLen));
		QMessageBox::information(this, QString("读取完成"), QString("读取日志文件完成"));
	}
	else if (n == -1)
	{
		QMessageBox::information(this, QString("读取失败"), QString("读取日志文件失败"));
	}
	else
	{
		QMessageBox::information(this, QString("读取失败"), QString("读取日志文件失败"));
	}

}
void QLogPlot::sendMessage()
{
	if (LogData->vData.size() <= LogData->iIndex)
	{
		if(m_tSendMes)
			m_tSendMes->stop();
		return;
	}
	QByteArray data;

	for (int i = 0; i < 8; i++)
	{
		data.append(LogData->vData[LogData->iIndex].Data[i]);
	}
	emit sigNewMessage(LogData->vData[LogData->iIndex++].Id, data);
}
void QLogPlot::on_showWidget()
{
	this->setHidden(false);
	this->show();
	plotItem();
}
void QLogPlot::on_AddData(ushort index, double x, double y)
{
	if (index >= aixsRect.count())
	{
		QLOG_WARN() << "index >= aixsRect.count()";
		return;
	}
	//	
	QCPAxisRect* rect = aixsRect.at(index);
	QCPGraph* grap = nullptr;
	QList<QCPGraph*> rlist = rect->graphs();
	if (rlist.size() <= 0)
	{
		grap = plot->addGraph(rect->axis(QCPAxis::atBottom), rect->axis(QCPAxis::atLeft));
		QPen graphPen;
		if(index< LableLineColor.count())
			graphPen.setColor(LableLineColor.at(index));
		else
			graphPen.setColor(QColor(qAbs(qrand()%255), qAbs(qrand() % 255), qAbs(qrand() % 255)));
		graphPen.setWidthF(3);
		grap->setPen(graphPen);
		grap->setLineStyle(QCPGraph::lsLine);
		
	}
	else
	{
		grap = rect->graphs().at(0);
	}
	if (!grap)
	{
		QLOG_WARN() << "if (!grap)";
		return;
	}
	x = (double)(QDateTime::currentMSecsSinceEpoch()) / 1000.0;//当前时间
	grap->addData(x, y);

	if (rect->axis(QCPAxis::atLeft)->range().minRange == y)
	{
		QCPRange ran(y - 5, rect->axis(QCPAxis::atLeft)->range().maxRange);
		rect->axis(QCPAxis::atLeft)->setRange(ran);

	}
	rect->axis(QCPAxis::atLeft)->rescale();
	rect->axis(QCPAxis::atBottom)->rescale();
	
	grap->rescaleValueAxis(false, true);
	rect->axis(QCPAxis::atBottom)->setRange(rect->axis(QCPAxis::atBottom)->range().upper, 100, Qt::AlignRight);
	
	
	
	plot->replot(QCustomPlot::rpQueuedReplot);
}
void QLogPlot::proLogData(uint id, QByteArray data,const int &x)
{
	int index = 0;
	//获取协议
	ushort protol = pModelMes.agreement;
	for (int i = 0; i < pModelMes.cItem.size(); i++)
	{
		d_x += x;
		for (int j = 0; j < pModelMes.cItem.at(i).pItem.size(); j++)
		{
			if (pModelMes.cItem.at(i).pItem.at(j).isRoll)
			{
				if (id != pModelMes.cItem.at(i).strCanId.toInt(nullptr, 16))
				{
					index++;
					continue;
				}
				float value = 0;
				//intel 协议
				//if (0 == protol)
				//{
				//	if (pModelMes.cItem.at(i).isSend)
				//	{
				//		value = parseMesItem_intel(pModelMes.cItem.at(i).pItem.at(j), data, j, true);
				//	}
				//	else
				//	{
				//		value = parseMesItem_intel(pModelMes.cItem.at(i).pItem.at(j), data, j, false);
				//	}

				//}
				////motorola MSB 协议
				//else if(1==protol)
				//{
				//	if (pModelMes.cItem.at(i).isSend)
				//	{
				//		value = parseMesItem_moto_msb(pModelMes.cItem.at(i).pItem.at(j), data, j, true);
				//	}
				//	else
				//	{
				//		value = parseMesItem_moto_msb(pModelMes.cItem.at(i).pItem.at(j), data, j, false);
				//	}
				//}
				if (0 == protol)
				{
					value = MsgParser::intel_Parser(pModelMes.cItem.at(i).pItem.at(j), data, pModelMes.cItem.at(i).isSend);
				}
				else if (1 == protol)
				{
					value = MsgParser::moto_Msb_Parser(pModelMes.cItem.at(i).pItem.at(j), data, pModelMes.cItem.at(i).isSend);
				}
				else if (2 == protol)
				{
					value = MsgParser::moto_Lsb_Parser(pModelMes.cItem.at(i).pItem.at(j), data, pModelMes.cItem.at(i).isSend);
				}
				on_AddData(index, d_x, value);
				index++;
			}
		}
		
	}
}
/*****************************************
* @brief：多线程往外发送读到的数据
* @param：
* float times,时间因子，假设消息A在0秒收到，
*消息了在2秒收到，那么在发送完消息A之后，再
*隔times*2秒之后才往外发消息B
* @return：void
******************************************/
void QLogPlot::runSendMes(float times)
{
	//qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
	//uint i = 0;
	/*while (m_bRunSend)
	{
		on_AddData(0, i++, qrand() % 10);
		Sleep(500);
		on_AddData(1, i++, qrand() % 10);
		Sleep(1500);
	}
	return;*/
	times = (times <= 0) ? 1 : times;
	d_x = 0;
	LogData->iIndex = 0;
	emit sigDataIndexChange(0);
	while (m_bRunSend)
	{
		if (LogData->vData.size() <= LogData->iIndex)
			return;
		QByteArray data;

		for (int i = 0; i < 8; i++)
		{
			data.append(LogData->vData[LogData->iIndex].Data[i]);
		}
		//获取当前时间戳，就是延时
		float time = LogData->vData[LogData->iIndex].TimeStemp;
		float tempt = time;
		//防止数值大，图形曲线刷新太快
		if (time >= 1000)
		{
			time = (time / 1000);
		}
		else if (time >= 100)
		{
			time = (time / 100);
		}
		else if (time >= 10)
		{
			time = (time / 10);
		}
		
		emit sigNewMessage(LogData->vData[LogData->iIndex++].Id, data);
		emit sinNewMessageInline(LogData->vData[LogData->iIndex-1].Id, data, time);
		emit sigDataIndexChange(LogData->iIndex);
		QThread::msleep(100);
		//2是暂停，意思是卡在这里不发送消息
		while (m_bRunSend == 2)
		{
			QThread::msleep(1);
		}
		
	}
	on_pbStop_clicked();
}

void QLogPlot::plotItem()
{
	if (!plot)return;
	LableLineColor.clear();
	removeRect();
	
	for (int i = 0; i < pModelMes.cItem.size(); i++)
	{
		for (int j = 0; j < pModelMes.cItem.at(i).pItem.size(); j++)
		{
			if (!pModelMes.cItem.at(i).pItem.at(j).isRoll)
			{//No show signal
				continue;
			}
			QSharedPointer<QCPAxisTickerDateTime>timer(new QCPAxisTickerDateTime);
			timer->setDateTimeFormat("h:m:s.z");
			timer->setTickCount(10);
			//添加一个坐标轴外框架
			QCPAxisRect* cpa = new QCPAxisRect(plot, true);
			//添加标签名
			cpa->axis(QCPAxis::atBottom)->setTicker(timer);
			cpa->axis(QCPAxis::atBottom)->setTickLabelRotation(-15);
			cpa->axis(QCPAxis::atLeft)->setLabel(QString(pModelMes.cItem.at(i).pItem.at(j).bitName));
			cpa->axis(QCPAxis::atBottom)->setBasePen(QPen(QColor(0, 0, 0, 60)));
			QColor cc = QColor(qAbs(qrand() % 255), qAbs(qrand() % 255), qAbs(qrand() % 255));
			LableLineColor.append(cc);
			//设置字体颜色和字体
			cpa->axis(QCPAxis::atLeft)->setLabelColor(cc);
			cpa->axis(QCPAxis::atLeft)->setLabelFont(QFont("Yahei"));
			cpa->axis(QCPAxis::atLeft)->rescale();
			//添加到布局里面
			plot->plotLayout()->addElement(plot->axisRectCount(), 0, cpa);
			//用一个List管理坐标轴外框
			aixsRect.append(cpa);
		}
	}
	plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	plot->replot();
}

void QLogPlot::InitGraphUI()
{
	//把label_show这个label作为画图的载体
	plot = new QCustomPlot(ui.lable_show);
	qsrand(QDateTime::currentMSecsSinceEpoch());
	//清除所有的画图
	plot->clearGraphs();
	//清除所有的布局
	plot->plotLayout()->clear();
	//重绘
	plot->replot();
	//plot->legend->setVisible(true);
	connect(this, &QLogPlot::sinNewMessageInline, this, &QLogPlot::proLogData);
	connect(this, &QLogPlot::sigDataIndexChange, this, &QLogPlot::on_setSlider);
}

void QLogPlot::removeRect()
{
	for (int i = 0; i < aixsRect.size(); i++)
	{
		auto aixs = aixsRect.at(i);
		if (!aixs)
			continue;
		auto grap = aixs->graphs().at(0);
		if (!grap)
			continue;
		/*grap->data()->clear();*/
		plot->removeGraph(grap);
		grap = nullptr;
		
	}
	plot->plotLayout()->clear();
	aixsRect.clear();
	plot->replot();
}
/*
* @brief：Messages item parse for intel protol
* @
//*/
//float QLogPlot::parseMesItem_intel(const protoItem& pItem, const QByteArray& data, int index,bool isSend)
//{
//	//防止越界
//	if (pItem.startByte > 7)
//		return 0.0;
//	QStringList binaryStr = QByteToBinary(data);
//
//	int startByte = pItem.startByte;
//	int startBit = pItem.startBit;
//	int startLenght = pItem.bitLeng;
//	float precision = pItem.precision;
//	int offset = pItem.offset;
//	bool octHex = pItem.octhex;
//	parseData pd;
//	float temp = 0;
//	QString datafrom = pItem.dataFrom;
//
//
//	//这个字段的数据来源其它字段的乘或除
//	if (datafrom.contains("*") || datafrom.contains("/"))
//	{
//		/*QStringList splt = datafrom.split("*");
//		if (splt.size() > 1)
//		{
//			dFromStru ddf;
//			ddf.index = m;
//			ddf.f1 = splt.at(0).toInt();
//			ddf.f2 = splt.at(1).toInt();
//			ddf.showCount = -1;
//			ddFF.push_back(ddf);
//		}*/
//	}
//	//CRC校验的异或
//	else if (datafrom.contains("XOR"))
//	{
//		/*(uint8_t)data[m];
//		uchar cData[8];
//		for (int d = 0; d < 8; d++)
//		{
//			cData[d] = (uchar)data[d];
//		}
//		uchar res = checksumXOR(cData);
//		uchar csum = data[m];
//		if (res == csum)
//		{
//			temp = 0;
//		}
//		else
//		{
//			temp = 1;
//		}*/
//	}
//	else
//	{
//		//判断是否跨字节，起止位模8，得出是当前字节的起止位，再加个长度
//		int len = startBit % 8 + startLenght;
//		if (len <= 8)
//		{   //不跨字节，这个就比较简单了
//			//15 14 13 12 11 10 9 8   7 6 5 4 3 2 1 0
//			//^高位在前，低位在后
//			if (octHex)
//			{
//				int nn = binaryStr[startByte].mid(8 - (startLenght + (startBit % 8)), startLenght).toInt(NULL, 2);
//				QString ss = QString("%1").arg(nn, 0, 16, QLatin1Char('0'));
//				if(isSend)
//					temp = ss.toInt() / precision - offset;
//				else
//					temp = ss.toInt() * precision + offset;
//			}
//			else
//			{
//				if (isSend)
//					temp = binaryStr[startByte].mid(8 - (startLenght + (startBit % 8)), startLenght).toInt(NULL, 2) / precision - offset;
//				else
//					temp = binaryStr[startByte].mid(8 - (startLenght + (startBit % 8)), startLenght).toInt(NULL, 2) * precision + offset;
//			}
//
//		}
//		else if (len <= 16)
//		{
//			if (isSend)
//				temp = (binaryStr[startByte + 1].mid(8 - (startLenght - (8 - startBit % 8)), startLenght - (8 - startBit % 8)) + binaryStr[startByte].mid(0, 8 - (startBit % 8))).toInt(NULL, 2) / precision - offset;
//			else
//				temp = (binaryStr[startByte + 1].mid(8 - (startLenght - (8 - startBit % 8)), startLenght - (8 - startBit % 8)) + binaryStr[startByte].mid(0, 8 - (startBit % 8))).toInt(NULL, 2) * precision + offset;
//		}
//		{
//			//跨三个字节的，应该没有
//		}
//	}
//
//
//	return temp;
//}
//
//float QLogPlot::parseMesItem_moto_msb(const protoItem& pItem, const QByteArray& data, int index, bool isSend)
//{
//	//防止越界
//	if (pItem.startByte > 7)
//		return 0.0;
//	QStringList binaryStr = QByteToBinary(data);
//
//	int startByte = pItem.startByte;
//	int startBit = pItem.startBit;
//	int startLenght = pItem.bitLeng;
//	float precision = pItem.precision;
//	int offset = pItem.offset;
//	bool octHex = pItem.octhex;
//	parseData pd;
//	float temp = 0;
//	QString datafrom = pItem.dataFrom;
//
//
//	//这个字段的数据来源其它字段的乘或除
//	if (datafrom.contains("*") || datafrom.contains("/"))
//	{
//		QStringList splt = datafrom.split("*");
//		if (splt.size() > 1)
//		{
//			/*dFromStru ddf;
//			ddf.index = m;
//			ddf.f1 = splt.at(0).toInt();
//			ddf.f2 = splt.at(1).toInt();
//			ddFF.push_back(ddf);*/
//		}
//	}
//	else if (datafrom.contains("XOR"))
//	{
//		/*(uint8_t)data[m];
//		uchar cData[8];
//		for (int d = 0; d < 8; d++)
//		{
//			cData[d] = (uchar)data[d];
//		}
//		uchar res = checksumXOR(cData);
//		uchar csum = data[m];
//		if (res == csum)
//		{
//			temp = 0;
//		}
//		else
//		{
//			temp = 1;
//		}*/
//	}
//	else
//	{
//		int len = startBit % 8 + startLenght;
//		if (len <= 8)
//		{   //不跨字节，这个就比较简单了
//			//15 14 13 12 11 10 9 8   7 6 5 4 3 2 1 0
//			//^高位在前，低位在后
//
//			if (octHex)
//			{
//				int nn = binaryStr[startByte].mid(8 - (startLenght + (startBit % 8)), startLenght).toInt(NULL, 2);
//				QString ss = QString("%1").arg(nn, 0, 16, QLatin1Char('0'));
//				if(isSend)
//					temp = ss.toInt() / precision - offset;
//				else
//					temp = ss.toInt() * precision + offset;
//			}
//			else
//			{
//				if (isSend)
//					temp = binaryStr[startByte].mid(startBit % 8, startLenght).toInt(NULL, 2) / precision - offset;
//				else
//					temp = binaryStr[startByte].mid(startBit % 8, startLenght).toInt(NULL, 2) * precision + offset;
//			}
//
//		}
//		else if (len <= 16)
//		{
//
//			if (octHex)
//			{
//				int nn = (binaryStr[startByte].mid(0, 8 - (startBit % 8)) + binaryStr[startByte + 1].mid(startBit % 8, startLenght - (8 - startBit % 8))).toInt(NULL, 2);
//				QString ss = QString("%1").arg(nn, 0, 16, QLatin1Char('0'));
//				if (isSend)
//					temp = ss.toInt() / precision - offset;
//				else
//					temp = ss.toInt() * precision + offset;
//			}
//			else
//			{
//				if (isSend)
//					temp = (binaryStr[startByte].mid(0, 8 - (startBit % 8)) + binaryStr[startByte + 1].mid(startBit % 8, startLenght - (8 - startBit % 8))).toInt(NULL, 2) / precision - offset;
//				else
//					temp = (binaryStr[startByte].mid(0, 8 - (startBit % 8)) + binaryStr[startByte + 1].mid(startBit % 8, startLenght - (8 - startBit % 8))).toInt(NULL, 2) * precision + offset;
//			}
//
//		}
//		{
//			//跨三个字节的，应该没有
//		}
//	}
//	return temp;
//}

QStringList QLogPlot::QByteToBinary(const QByteArray& data)
{
	QStringList binaryStr;
	QString hex;
	for (int k = 0; k < data.size(); ++k)
	{
		QString str = QString("%1").arg((uint8_t)data[k], 8, 2, QLatin1Char('0'));
		binaryStr.append(str);

	}
	return binaryStr;
}
void QLogPlot::runExportMsg(QString filepath)
{
	
	if (!LogData || LogData->vData.size() <= 0)
	{
		return;
	}
	QString excelTitle="序号,offset-time,";
	for (int i = 0; i < pModelMes.cItem.size(); i++)
	{
		for (int j = 0; j < pModelMes.cItem.at(i).pItem.size(); j++)
		{
			if (!pModelMes.cItem.at(i).pItem.at(j).isRoll)
			{//No show signal
				continue;
			}
			excelTitle += pModelMes.cItem.at(i).pItem.at(j).bitName + ",";
		}
	}
	excelTitle.remove(excelTitle.size() - 1,1);
	ushort protol = pModelMes.agreement;
	QStringList strData;
	//QString strtemp;
	int count = 0;

	for (int m = 0; m < LogData->vData.size(); m++)
	{
		uint id = LogData->vData[m].Id;
		uchar dTemp[8];
		double timestemp = LogData->vData[m].TimeStemp;
		memcpy(dTemp, LogData->vData[m].Data, 8);
		QByteArray data;
		for (int k = 0; k < 8; k++)
		{
			data.append(dTemp[k]);
		}
		
		for (int i = 0; i < pModelMes.cItem.size(); i++)
		{
			if (id != pModelMes.cItem.at(i).strCanId.toInt(nullptr, 16))
			{
				continue;
			}
			std::vector<parseData>parseArr;
			for (int j = 0; j < pModelMes.cItem.at(i).pItem.size(); j++)
			{
				if (!pModelMes.cItem.at(i).pItem.at(j).isRoll)
				{//No show signal
					continue;
				}
				
				float value = 0;
				if (0 == protol)
				{
					
					value = MsgParser::intel_Parser(pModelMes.cItem.at(i).pItem.at(j), data, pModelMes.cItem.at(i).isSend);
					
				}
				else if (1 == protol)
				{
					value = MsgParser::moto_Msb_Parser(pModelMes.cItem.at(i).pItem.at(j), data, pModelMes.cItem.at(i).isSend);
				}
				else if (2 == protol)
				{
					value = MsgParser::moto_Lsb_Parser(pModelMes.cItem.at(i).pItem.at(j), data, pModelMes.cItem.at(i).isSend);
				}
				parseData pd;
				pd.name = pModelMes.cItem.at(i).pItem.at(j).bitName;
				pd.value = value;
				pd.toWord = QString::number(value);
				pd.color.r = 255;
				pd.color.g = 255;
				pd.color.b = 255;
				std::vector<cellProperty>& ss = pModelMes.cItem.at(i).pItem.at(j).stl_itemProperty;
				//解析要显示到单元格的颜色
				int stdddd = 0;
				
				
				for (int i = 0; i < ss.size(); i++)
				{
					if (ss.at(i).isStand)
					{
						stdddd = ss.at(i).value.toInt();
					}
					if (ss.at(i).value.toInt() == value)
					{
						pd.color.r = ss.at(i).r;
						pd.color.g = ss.at(i).g;
						pd.color.b = ss.at(i).b;
						pd.toWord = ss.at(i).toWord;
						break;
					}
				}
				parseArr.push_back(pd);

				int i_index = YB::idNameInVector(showTableVec, QString::number(id));
				if (i_index >= 0)
				{
					showTableVec.at(i_index).Pdata = parseArr;
				}
				else
				{
					showTableData temp;
					temp.IdName = QString::number(id);
					temp.Pdata = parseArr;
					showTableVec.push_back(temp);
				}

				//strtemp+=QString::number(value)+",";
			}
		}
		std::vector<showTableData>::iterator iBeginV = showTableVec.begin();
		std::vector<showTableData>::iterator iEndV = showTableVec.end();
		QString strdTemp;
		while (iBeginV != iEndV)
		{
			int num = iBeginV->Pdata.size();
			for (int j = 0; j < num; j++)
			{
				strdTemp += (iBeginV->Pdata.at(j).toWord + ",");
			}
			iBeginV++;
		}

		if (m < LogData->vData.size() - 2 && LogData->vData[m+1].TimeStemp < 50)
		{
			continue;
		}
		if (strdTemp.isEmpty())
			continue;
		strdTemp.remove(strdTemp.size() - 1, 1);
		QString a = QString::number(count++) + "," + QString::number(timestemp) + "," + strdTemp;
		strData.append(a);
		strdTemp = "";
		strdTemp.clear();
		//showTableVec.clear();
	}
	
	m_dataSave->setTitle(excelTitle);
	m_dataSave->SaveData(strData, strData.size(), filepath);

}
void QLogPlot::on_setSlider(int value)
{
	ui.horizontalSlider->setValue(value);
}

void QLogPlot::on_pbExportSignal_clicked()
{
	if (!LogData || LogData->vData.size()<=0)
	{
		return;
	}
	QString filename = QFileDialog::getSaveFileName(this, "导出excel文件", "./", "csv(*.csv *.xls *.xlsx)");
	if (filename.isEmpty())
	{
		QMessageBox::warning(this, "文件名为空", "文件名不能为空");
		return;
	}
	QtConcurrent::run(this, &QLogPlot::runExportMsg, filename);

}
