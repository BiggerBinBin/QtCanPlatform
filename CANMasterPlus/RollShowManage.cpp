#pragma execution_character_set("utf-8")  
#include "RollShowManage.h"
#include <QTableWidget>
#include <QTabWidget>
#include <QWidget>
#include <QLabel>
#include <qheaderview.h>
#include <QtConcurrent>
#include <QElapsedTimer>
#include "../DeviceIO/3rd/QsLog/include/QsLog.h"
#include "../../CANMasterPlus/ModelDataManage/Unities/MsgParser.h"
#include "../../CANMasterPlus/ModelDataManage/Unities/AlgorithmSet.h"
#include "../ModelDataManage/Unities/qGboleData.h"

QString rollTitleStyle = "QHeaderView::section {background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #5A69B2, stop: 0.5 #5A69B2,stop: 0.6 #5A69B2, stop:1 #5A69B2);color: white;border:1px solid;border-color:white;font-weight: bold;}QHeaderView{background-color:#5A69B2}";
RollShowManage::RollShowManage(QObject *parent)
	: QObject(parent)
{
	initTable();
	m_currModelIndex = 0;
	on_currentModelIndex_Changed(m_currModelIndex);
    iniPath = QApplication::applicationDirPath() + "/Data/syssetting.ini";
    this->readIni(iniPath);
    this->m_TimerShow = new QTimer(this);
    connect(this->m_TimerShow, &QTimer::timeout, this, &RollShowManage::onNewRollMes);
    this->m_TimerLoss = new QTimer(this);
    connect(this->m_TimerLoss, &QTimer::timeout, this, &RollShowManage::onStopShow);

    connect(this, &RollShowManage::sigRollMes, this, &RollShowManage::doTimerOperator);
}

RollShowManage::~RollShowManage()
{
    if (this->m_TimerShow)
        this->m_TimerShow->stop();
    if (this->m_TimerLoss)
        this->m_TimerLoss->stop();
}

void RollShowManage::transferMsg(int ch, unsigned int fream_id, QByteArray data)
{
    auto it = m_msg_gap.find(fream_id);
    static int grp_t;
    grp_t = 100;
    if (it != m_msg_gap.end())
    {
        grp_t = QDateTime::currentMSecsSinceEpoch() - it.value();
        if (grp_t >= 50)
            m_msg_gap[fream_id] = QDateTime::currentMSecsSinceEpoch();
    }
    else
    {
        m_msg_gap[fream_id] = QDateTime::currentMSecsSinceEpoch();
    }
    if(grp_t>=50)
        QtConcurrent::run(this, &RollShowManage::msgParser, ch, fream_id, data);
    emit sigNewMessageToGraph(fream_id, data, time_index++);
}

void RollShowManage::clearData()
{
    RollShowData.clear();
    rollTableWidget->setRowCount(0);
}

void RollShowManage::initTable()
{
    time_index = 0;
	layout = new QVBoxLayout();
	rollTableWidget = new QTableWidget();
    rollTableWidget->horizontalHeader()->setVisible(false);
    rollTableWidget->verticalHeader()->setVisible(false);
	titleRollTableWidget = new QTableWidget();
	titleRollTableWidget->verticalHeader()->setVisible(false);
	titleRollTableWidget->setFixedHeight(30);
    
	titleRollTableWidget->horizontalHeader()->setStyleSheet(rollTitleStyle);
	tabWidget = new QTabWidget();
	layout->addWidget(titleRollTableWidget);
	layout->addWidget(rollTableWidget);
	layout->setStretch(0, 1);
	layout->setStretch(1, 100);
	QWidget *w = new QWidget();
	w->setLayout(layout);
	tabWidget->addTab(w, "Roll");
	QWidget* w2 = new QWidget();
	tabWidget->addTab(w2, "Simulate");
    m_plot_ptr = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();

    m_plot_dw_ptr = new QLogPlot(0);
    layout->addWidget(m_plot_dw_ptr);
    connect(this, &RollShowManage::sigNewMessageToGraph, m_plot_dw_ptr, &QLogPlot::proLogData);
    m_plot_ptr->setLayout(layout);
    layout->setStretch(1, 10);
    layout->setStretch(0, 1);

    

    tabWidget->addTab(m_plot_ptr, "Plot");
    //connect(this, &RollShowManage::sigRollMes, this, &RollShowManage::onNewRollMes,Qt::QueuedConnection);

}
void RollShowManage::titleRest()
{
	this->titleRoll.clear();
	this->titleRoll.append("序号");
	for (auto x: this->m_currentModel.cItem)
	{
		if (1 == x.opt)continue;

		for (auto v : x.pItem)
		{
			if (v.isRoll)
				this->titleRoll.append(v.bitName);
		}

	}
	this->titleRollTableWidget->setColumnCount(this->titleRoll.size());
	this->titleRollTableWidget->setHorizontalHeaderLabels(this->titleRoll);
	this->titleRollTableWidget->horizontalHeader()->setStyleSheet(rollTitleStyle);
	QFont ff;
	ff.setBold(true);
	this->titleRollTableWidget->horizontalHeader()->setFont(ff);
    for(int i=0;i<titleRoll.size();i++)
        titleRollTableWidget->setColumnWidth(i,120);
}
void RollShowManage::on_currentModelIndex_Changed(int index)
{
	
	qGboleData* qgb = qGboleData::getInstance();
	if (!qgb)return;
	if (index < 0 || index >= qgb->pGboleData.size())
		return;

	this->m_currModelIndex = index;

	m_currentModel = qgb->pGboleData.at(index); 
    time_index = 0;
    if (this->m_plot_dw_ptr)
    {
        this->m_plot_dw_ptr->setModelIndex(index);
    }
	titleRest();
}
void RollShowManage::msgParser(int ch, unsigned int fream_id, QByteArray data)
{

    QStringList binaryStr;
    binaryStr.clear();
    QString hex;
    for (int k = 0; k < data.size(); ++k)
    {
        QString str = QString("%1").arg((uint8_t)data[k], 8, 2, QLatin1Char('0'));
        binaryStr.append(str);

        hex += QString("%1").arg(str.toInt(NULL, 2), 2, 16, QLatin1Char('0')).toUpper() + " ";
    }
   
    bool isRoll = false;
    for (int i = 0; i < m_currentModel.cItem.size(); i++)
    {
        uint currID = m_currentModel.cItem.at(i).strCanId.toUInt(NULL, 16);
        if (currID != fream_id)
            continue;
        for (int m = 0; m < m_currentModel.cItem.at(i).pItem.size(); ++m)
        {
            float temp = 0;
            QString datafrom = m_currentModel.cItem.at(i).pItem.at(m).dataFrom;


            //由其它组成
            if (datafrom != "-1")
            {
                QStringList splt; 
                QString symbol;
                if (datafrom.contains("XOR"))
                {
                    uchar cData[8];
                    for (int d = 0; d < 8; d++)
                    {
                        cData[d] = (uchar)data[d];
                    }
                    uchar res = checksumXOR(cData, data.size());
                    uchar csum = data[m];
                    if (res == csum)
                    {
                        temp = 0;   //校验通过
                    }
                    else
                    {
                        temp = 1;
                    }
                }
                else if (datafrom.toUpper().contains("CH"))
                {
                    if (!m_GetTempPointer)
                        temp = -99.99;
                    else
                        temp = m_GetTempPointer->getTemperatur(datafrom);
                }
                else
                {
                    if (datafrom.contains("*"))
                    {
                        symbol = "*";
                    }
                    else if (datafrom.contains("/"))
                    {
                        symbol = "/";
                    }
                    else if (datafrom.contains("+"))
                    {
                        symbol = "+";
                    }
                    else if (datafrom.contains("-"))
                    {
                        symbol = "-";
                    }
                    splt = datafrom.split(symbol);
                    float v1 = 0;
                    float v2 = 0;
                    if (splt.size() > 1)
                    {
                        uint16_t f1 = splt.at(0).toUInt() - 1;    //行数从
                        uint16_t f2 = splt.at(1).toUInt() - 1;
                        if ((f1 < 0 || f1 >= m_currentModel.cItem.at(i).pItem.size()) || (f2 < 0 || f2 >= m_currentModel.cItem.at(i).pItem.size()))
                        {
                            temp = 0;
                            QLOG_WARN() << "paraser error: id = " << m_currentModel.cItem.at(i).strCanId << ",BitName=" << m_currentModel.cItem.at(i).pItem.at(m).bitName;
                        }
                        else
                        {
                            switch (m_currentModel.agreement)
                            {
                            case 0:
                                v1 = MsgParser::intel_Parser(m_currentModel.cItem.at(i).pItem.at(f1), data, false);
                                v2 = MsgParser::intel_Parser(m_currentModel.cItem.at(i).pItem.at(f2), data, false);
                                break;
                            case 1:
                                v1 = MsgParser::moto_Msb_Parser(m_currentModel.cItem.at(i).pItem.at(f1), data, false);
                                v2 = MsgParser::moto_Msb_Parser(m_currentModel.cItem.at(i).pItem.at(f2), data, false);
                                break;
                            case 2:
                                v1 = MsgParser::moto_Lsb_Parser(m_currentModel.cItem.at(i).pItem.at(f1), data, false);
                                v2 = MsgParser::moto_Lsb_Parser(m_currentModel.cItem.at(i).pItem.at(f2), data, false);
                                break;
                            default:
                                break;
                            }


                            
                        }

                    }
                    if ("*" == symbol)
                    {
                        temp = v1 * v2;
                    }
                    else if ("/" == symbol)
                    {
                        if(v2!=0)
                            temp = v1 / v2;
                    }
                    else if ("+" == symbol)
                    {
                        
                        temp = v1 + v2;
                    }
                    else if ("-" == symbol)
                    {

                        temp = v1 - v2;
                    }
                }
                
                
            }
            
            else
            {
                switch (m_currentModel.agreement)
                {
                case 0:
                    temp = MsgParser::intel_Parser(m_currentModel.cItem.at(i).pItem.at(m), data, false);
                    break;
                case 1:
                    temp = MsgParser::moto_Msb_Parser(m_currentModel.cItem.at(i).pItem.at(m), data, false);
                    break;
                case 2:
                    temp = MsgParser::moto_Lsb_Parser(m_currentModel.cItem.at(i).pItem.at(m), data, false);
                    break;
                default:
                    break;
                }
                

            }


            //如果是需要滚动显示的
            if (m_currentModel.cItem.at(i).pItem.at(m).isRoll)
            {
                //读写锁
                m_read_write_lock.lockForWrite();
                isRoll = true;
                int index = 0;
                //查找里面有没有
                if (YB::nameInVector(RollShowData, m_currentModel.cItem.at(i).pItem.at(m).bitName, index))
                {
                    RollShowData.at(index).value = temp;
                }
                else
                {
                    RollStruct rr;
                    rr.name = m_currentModel.cItem.at(i).pItem.at(m).bitName;
                    rr.value = temp;
                    RollShowData.push_back(rr);
                }
                m_read_write_lock.unlock();
            }

        }
    }
    if (isRoll)
    {
       
        emit sigRollMes();
    }
}
unsigned char RollShowManage::checksumXOR(const uchar data[], int lenght)
{
    if (lenght < 8)return 0xff;
    return (data[1] ^ data[2] ^ data[3] ^ data[4] ^ data[5] ^ data[6] ^ data[7]);
}

bool RollShowManage::readIni(QString path)
{
    QFile f(path);
    if (!f.exists())
    {
        //technologyPW = "1234";
        //adminPW = "kus@1234";
        return false;
    }
    std::shared_ptr<QSettings>s = std::make_shared<QSettings>(path, QSettings::IniFormat);//   new QSettings();
    this->m_iAutoSaveNum = s->value("m_iAutoSaveNum").toInt();
    this->m_bIsAutoSave = s->value("m_bIsAutoSave").toInt();
    return true;
}

void RollShowManage::onNewRollMes()
{
    //设置列数
    rollTableWidget->setColumnCount(titleRoll.size());
    //顺便设置列宽
    for (int i = 0; i < titleRoll.size(); i++)
    {
        rollTableWidget->setColumnWidth(i, 120);
    }
    //在总行数上加一行
    int row = rollTableWidget->rowCount();
    rollTableWidget->setRowCount(row + 1);
   
    rollTableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(row)));
    QTableWidgetItem* b2 = rollTableWidget->item(row, 0);
    if (b2)
        b2->setTextAlignment(Qt::AlignCenter);
    QStringList title;
    title.append("序号");

    //获取读锁
    m_read_write_lock.lockForRead();
    for (int i = 0; i < RollShowData.size(); i++)
    {
        rollTableWidget->setItem(row, i + 1, new QTableWidgetItem(QString::number(RollShowData.at(i).value)));
        title.append(RollShowData.at(i).name);
        QTableWidgetItem* b = rollTableWidget->item(row, i + 1);
        if (!b)
            continue;
        //居中
        b->setTextAlignment(Qt::AlignCenter);
    }
    m_read_write_lock.unlock();

    //表格显示最底
    rollTableWidget->scrollToBottom();
    //设置一下标题，防止错乱
    titleRollTableWidget->setHorizontalHeaderLabels(title);
    
}

void RollShowManage::on_m_singal_mg_clicked()
{
    if (!plotSignalMn)
    {
        plotSignalMn = new QSignalManage(&m_currentModel_Plot);
        connect(plotSignalMn, &QSignalManage::QSiganlClose, this, &RollShowManage::on_showWidget);
    }
    plotSignalMn->setCurrentModel(m_currModelIndex);
    plotSignalMn->show();
}

void RollShowManage::on_showWidget()
{
    //this->m_plot_dw_ptr->setModelData(m_currentModel_Plot);
    //this->m_plot_dw_ptr->plotItem();
}
void RollShowManage::on_slotClearData()
{
    this->clearData();
}
void RollShowManage::onStopShow()
{
    if (this->m_TimerShow)
    {
        this->m_TimerShow->stop();
    }
}
void RollShowManage::doTimerOperator()
{
    this->m_TimerLoss->start(this->m_currentModel.circle * 2);
    if (!this->m_TimerShow->isActive())
        this->m_TimerShow->start(this->m_currentModel.circle);
}