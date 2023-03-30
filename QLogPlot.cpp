#include "QLogPlot.h"
#include <qfiledialog.h>
#include "qGboleData.h"
#include <qmessagebox.h>
#include <qthread.h>
#include <QtConcurrent>
#include <QTime>
#include <QtGlobal>
#include "QsLog.h"
# pragma execution_character_set("utf-8")
QLogPlot::QLogPlot(int index,QWidget *parent)
	: m_iModel(index), QWidget(parent)
{
	//this->setWindowFlag(Qt::Tool);
	ui.setupUi(this);
	this->ui.pbPlay->setEnabled(false);
	this->ui.pbPause->setEnabled(false);
	this->ui.pbStop->setEnabled(false);
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
}
void QLogPlot::resizeEvent(QResizeEvent* event)
{
	plot->setFixedSize(ui.lable_show->size());
}
void QLogPlot::on_pbOpenFile_clicked()
{
	QString filepath = QFileDialog::getOpenFileName(NULL, "打开日志文件", QApplication::applicationDirPath(), QString("blf(*.blf)"));
	if (filepath.isEmpty()) return;
	if (!BLfLog)
	{
		BLfLog = new QBLFManage(this);
		connect(BLfLog, &QBLFManage::sigStatus, this, &QLogPlot::on_GetBLFStaus);
	}
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
	BLfLog->getBLFLog(filepath, fifter, LogData);
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
void QLogPlot::on_pbPause_clicked()
{
	m_bRunSend = 2;
}
void QLogPlot::on_pbStop_clicked()
{
	m_bRunSend = 0;
	LogData->iIndex = 0;
	this->ui.pbPlay->setEnabled(true);
}
void QLogPlot::on_GetBLFStaus(int n)
{
	if (n == 0)
	{
		this->ui.pbPlay->setEnabled(true);
		this->ui.pbPause->setEnabled(true);
		this->ui.pbStop->setEnabled(true);
		QMessageBox::information(this, QString("读取完成"), QString("读取日志文件完成"));
	}
	else if (n == -1)
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
	//x = (double)(QDateTime::currentMSecsSinceEpoch()) / 1000.0;//当前时间
	grap->addData(x, y);
	rect->axis(QCPAxis::atBottom)->rescale();
	grap->rescaleValueAxis(false, true);
	rect->axis(QCPAxis::atBottom)->setRange(rect->axis(QCPAxis::atBottom)->range().upper, 1000, Qt::AlignRight);
	grap->data()->removeBefore(x - 1000);

	plot->replot(QCustomPlot::rpQueuedReplot);
}
void QLogPlot::proLogData(uint id, QByteArray data,int x)
{
	int index = 0;
	//获取协议
	ushort protol = pModelMes.agreement;
	for (int i = 0; i < pModelMes.cItem.size(); i++)
	{
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
				if (0 == protol)
				{
					if (pModelMes.cItem.at(i).isSend)
					{
						value = parseMesItem_intel(pModelMes.cItem.at(i).pItem.at(j), data, j, true);
					}
					else
					{
						value = parseMesItem_intel(pModelMes.cItem.at(i).pItem.at(j), data, j, false);
					}
				}
				//motorola MSB 协议
				else if(1==protol)
				{
					if (pModelMes.cItem.at(i).isSend)
					{
						value = parseMesItem_moto_msb(pModelMes.cItem.at(i).pItem.at(j), data, j, true);
					}
					else
					{
						value = parseMesItem_moto_msb(pModelMes.cItem.at(i).pItem.at(j), data, j, false);
					}
				}
				
				on_AddData(index, x, value);
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
	qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
	uint i = 0;
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
		d_x += time;
		emit sigNewMessage(LogData->vData[LogData->iIndex++].Id, data);
		emit sinNewMessageInline(LogData->vData[LogData->iIndex-1].Id, data, d_x);
		QThread::msleep(time* times);
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
			
			QCPAxisRect* cpa = new QCPAxisRect(plot, true);
			cpa->axis(QCPAxis::atLeft)->setLabel(QString(pModelMes.cItem.at(i).pItem.at(j).bitName));
			QColor cc = QColor(qAbs(qrand() % 255), qAbs(qrand() % 255), qAbs(qrand() % 255));
			LableLineColor.append(cc);
			cpa->axis(QCPAxis::atLeft)->setLabelColor(cc);
			cpa->axis(QCPAxis::atLeft)->setLabelFont(QFont("Yahei"));
			plot->plotLayout()->addElement(plot->axisRectCount(), 0, cpa);
			aixsRect.append(cpa);
		}
	}
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
*/
float QLogPlot::parseMesItem_intel(const protoItem& pItem, const QByteArray& data, int index,bool isSend)
{
	//防止越界
	if (pItem.startByte > 7)
		return 0.0;
	QStringList binaryStr = QByteToBinary(data);

	int startByte = pItem.startByte;
	int startBit = pItem.startBit;
	int startLenght = pItem.bitLeng;
	float precision = pItem.precision;
	int offset = pItem.offset;
	bool octHex = pItem.octhex;
	parseData pd;
	float temp = 0;
	QString datafrom = pItem.dataFrom;


	//这个字段的数据来源其它字段的乘或除
	if (datafrom.contains("*") || datafrom.contains("/"))
	{
		/*QStringList splt = datafrom.split("*");
		if (splt.size() > 1)
		{
			dFromStru ddf;
			ddf.index = m;
			ddf.f1 = splt.at(0).toInt();
			ddf.f2 = splt.at(1).toInt();
			ddf.showCount = -1;
			ddFF.push_back(ddf);
		}*/
	}
	//CRC校验的异或
	else if (datafrom.contains("XOR"))
	{
		/*(uint8_t)data[m];
		uchar cData[8];
		for (int d = 0; d < 8; d++)
		{
			cData[d] = (uchar)data[d];
		}
		uchar res = checksumXOR(cData);
		uchar csum = data[m];
		if (res == csum)
		{
			temp = 0;
		}
		else
		{
			temp = 1;
		}*/
	}
	else
	{
		//判断是否跨字节，起止位模8，得出是当前字节的起止位，再加个长度
		int len = startBit % 8 + startLenght;
		if (len <= 8)
		{   //不跨字节，这个就比较简单了
			//15 14 13 12 11 10 9 8   7 6 5 4 3 2 1 0
			//^高位在前，低位在后
			if (octHex)
			{
				int nn = binaryStr[startByte].mid(8 - (startLenght + (startBit % 8)), startLenght).toInt(NULL, 2);
				QString ss = QString("%1").arg(nn, 0, 16, QLatin1Char('0'));
				if(isSend)
					temp = ss.toInt() / precision - offset;
				else
					temp = ss.toInt() * precision + offset;
			}
			else
			{
				if (isSend)
					temp = binaryStr[startByte].mid(8 - (startLenght + (startBit % 8)), startLenght).toInt(NULL, 2) / precision - offset;
				else
					temp = binaryStr[startByte].mid(8 - (startLenght + (startBit % 8)), startLenght).toInt(NULL, 2) * precision + offset;
			}

		}
		else if (len <= 16)
		{
			if (isSend)
				temp = (binaryStr[startByte + 1].mid(8 - (startLenght - (8 - startBit % 8)), startLenght - (8 - startBit % 8)) + binaryStr[startByte].mid(0, 8 - (startBit % 8))).toInt(NULL, 2) / precision - offset;
			else
				temp = (binaryStr[startByte + 1].mid(8 - (startLenght - (8 - startBit % 8)), startLenght - (8 - startBit % 8)) + binaryStr[startByte].mid(0, 8 - (startBit % 8))).toInt(NULL, 2) * precision + offset;
		}
		{
			//跨三个字节的，应该没有
		}
	}


	return temp;
}

float QLogPlot::parseMesItem_moto_msb(const protoItem& pItem, const QByteArray& data, int index, bool isSend)
{
	//防止越界
	if (pItem.startByte > 7)
		return 0.0;
	QStringList binaryStr = QByteToBinary(data);

	int startByte = pItem.startByte;
	int startBit = pItem.startBit;
	int startLenght = pItem.bitLeng;
	float precision = pItem.precision;
	int offset = pItem.offset;
	bool octHex = pItem.octhex;
	parseData pd;
	float temp = 0;
	QString datafrom = pItem.dataFrom;


	//这个字段的数据来源其它字段的乘或除
	if (datafrom.contains("*") || datafrom.contains("/"))
	{
		QStringList splt = datafrom.split("*");
		if (splt.size() > 1)
		{
			/*dFromStru ddf;
			ddf.index = m;
			ddf.f1 = splt.at(0).toInt();
			ddf.f2 = splt.at(1).toInt();
			ddFF.push_back(ddf);*/
		}
	}
	else if (datafrom.contains("XOR"))
	{
		/*(uint8_t)data[m];
		uchar cData[8];
		for (int d = 0; d < 8; d++)
		{
			cData[d] = (uchar)data[d];
		}
		uchar res = checksumXOR(cData);
		uchar csum = data[m];
		if (res == csum)
		{
			temp = 0;
		}
		else
		{
			temp = 1;
		}*/
	}
	else
	{
		int len = startBit % 8 + startLenght;
		if (len <= 8)
		{   //不跨字节，这个就比较简单了
			//15 14 13 12 11 10 9 8   7 6 5 4 3 2 1 0
			//^高位在前，低位在后

			if (octHex)
			{
				int nn = binaryStr[startByte].mid(8 - (startLenght + (startBit % 8)), startLenght).toInt(NULL, 2);
				QString ss = QString("%1").arg(nn, 0, 16, QLatin1Char('0'));
				if(isSend)
					temp = ss.toInt() / precision - offset;
				else
					temp = ss.toInt() * precision + offset;
			}
			else
			{
				if (isSend)
					temp = binaryStr[startByte].mid(startBit % 8, startLenght).toInt(NULL, 2) / precision - offset;
				else
					temp = binaryStr[startByte].mid(startBit % 8, startLenght).toInt(NULL, 2) * precision + offset;
			}

		}
		else if (len <= 16)
		{

			if (octHex)
			{
				int nn = (binaryStr[startByte].mid(0, 8 - (startBit % 8)) + binaryStr[startByte + 1].mid(startBit % 8, startLenght - (8 - startBit % 8))).toInt(NULL, 2);
				QString ss = QString("%1").arg(nn, 0, 16, QLatin1Char('0'));
				if (isSend)
					temp = ss.toInt() / precision - offset;
				else
					temp = ss.toInt() * precision + offset;
			}
			else
			{
				if (isSend)
					temp = (binaryStr[startByte].mid(0, 8 - (startBit % 8)) + binaryStr[startByte + 1].mid(startBit % 8, startLenght - (8 - startBit % 8))).toInt(NULL, 2) / precision - offset;
				else
					temp = (binaryStr[startByte].mid(0, 8 - (startBit % 8)) + binaryStr[startByte + 1].mid(startBit % 8, startLenght - (8 - startBit % 8))).toInt(NULL, 2) * precision + offset;
			}

		}
		{
			//跨三个字节的，应该没有
		}
	}
	return 0.0f;
}

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
