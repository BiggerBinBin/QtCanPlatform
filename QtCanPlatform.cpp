#pragma execution_character_set("utf-8")  
#include "QtCanPlatform.h"
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QHBoxLayout>
#include "qGboleData.h"
#include <QLabel>
#include <QComboBox>
#include <QMessageBox>
#include <QCheckBox>
#include <cstring>
#include <QLineEdit>
#include <QRegExp>
#include <QRegExpValidator>
#include <QHeaderView>
#include "AlgorithmSet.h"
#include "QsLog.h"
#include "QsLogDest.h"
#include <QFile>
#include <QSplitter>
using namespace QsLogging;

QString qmyss = "QComboBox{border: 1px solid gray;border-radius: 5px;padding:1px 2px 1px 2px;s}\
QComboBox::drop-down{subcontrol-origin: padding;subcontrol-position: top right;width: 15px;border-left-width: 1px;border-left-color: darkgray;border-left-style: solid;border-top-right-radius: 3px;border-bottom-right-radius: 3px;}";
QtCanPlatform::QtCanPlatform(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    
    initLogger();
    this->setStyleSheet(qmyss);
    initUi();
    sendTimer = new QTimer();
    connect(sendTimer, &QTimer::timeout, this, &QtCanPlatform::sendData);
   /* QLOG_INFO() << "中文世界";*/
    
}

QtCanPlatform::~QtCanPlatform()
{
    if (canSetting) { delete canSetting; canSetting = nullptr; }
    if (tableView) { delete tableView; tableView = nullptr; }
    if (textBrowser) { delete textBrowser; textBrowser = nullptr; }
    if (tableRecView) { delete tableRecView; tableRecView = nullptr; }
    if (pcan) {
        delete pcan; 
        pcan = nullptr;
    }
    if (cbPcan)
    {
        delete cbPcan; cbPcan = nullptr;
    }
    if (saveData)
    {
        delete saveData;
        saveData = nullptr;
    }
    destroyLogger();   //释放
}

void QtCanPlatform::closeEvent(QCloseEvent* event)
{
    if (pcan)
    {
        pcan->CloseCan();
        QLOG_INFO() << "关闭CAN设备";
    }
    event->accept();
}

void QtCanPlatform::initUi()
{
    //QLOG_INFO() << "初始化界面中……";
    //这个是显示内容的
    saveData = new DataSave(this);
    tableView = new QTableWidget();
    tableView->setColumnCount(8);
    QStringList header;
    header << tr("发送") << tr("操作") << tr("数值") << tr("名称") << tr("地址") << tr("起止字节") << tr("起止位") << tr("长度");
    tableView->setHorizontalHeaderLabels(header);
    QString stt = "QHeaderView::section {background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #0078d7, stop: 0.5 #0078d7,stop: 0.6 #0078d7, stop:1 #0078d7);color: white;border:1px solid;border-color:white;font-weight: bold;}";//QHeaderView{background-color:#0078d7}
    tableView->horizontalHeader()->setStyleSheet(stt);
    tableView->horizontalHeader()->setMinimumHeight(25);
    tableView->setColumnWidth(0, 40);
    connect(tableView, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(on_tableDoubleClicked(int, int)));
    textBrowser = new QTextBrowser();
    textBrowser->setMinimumWidth(100);
    //textBrowser->setMaximumWidth(300);
    initData();
    cbSelectModel = new QComboBox();
    
    qGboleData* qGb = qGboleData::getInstance();
    if (!qGb)return;
    for (int i = 0; i < qGb->pGboleData.size(); i++)
    {
        cbSelectModel->addItem(qGb->pGboleData.at(i).modelName);
        
    }
    if (qGb->pGboleData.size() > 0)
    {
        currentModel = 0;
        cbSelectModel->setCurrentIndex(0);
    }
       
    connect(cbSelectModel, SIGNAL(currentIndexChanged(int)), this, SLOT(on_CurrentModelChanged(int)));
    //添加个按钮
    QPushButton* pbAddModel = new QPushButton(tr("添加Can解析"));
    connect(pbAddModel, &QPushButton::clicked, this, &QtCanPlatform::qCanSettingShow);
    pbSend = new QPushButton(tr("发送"));
    pbSend->setCheckable(true);
    pbSend->setEnabled(false);
    connect(pbSend, SIGNAL(clicked(bool)),this,SLOT(on_pbSend_clicked(bool)));

    QLabel* pcn = new QLabel(tr("选择CAN："));
    cbPcan = new QComboBox();
    pcan = new PCAN(this);
    connect(pcan, &PCAN::getProtocolData, this, &QtCanPlatform::on_ReceiveData);
    QStringList canStr = pcan->DetectDevice();
    for (int i = 0; i < canStr.size(); ++i)
    {
        cbPcan->addItem(canStr.at(i));
    }
    reFresh = new QPushButton(tr("刷新设备"));
    connect(reFresh, SIGNAL(clicked()), this, SLOT(on_pbRefreshDevice_clicked()));
    //添加常用波特率
    cbBitRate = new QComboBox();
    cbBitRate->addItem("200kb/s");
    cbBitRate->addItem("250kb/s");
    cbBitRate->addItem("500kb/s");
    cbBitRate->addItem("800kb/s");
    cbBitRate->setCurrentIndex(1);
    QLabel* Period = new QLabel(tr("报文周期："));
    cycle = new QLineEdit("1000");
    cycle->setValidator(new QIntValidator(0, 9999999, this));
    cycle->setFixedWidth(60);
    pbOpen = new QPushButton(tr("打开设备"));
    connect(pbOpen, SIGNAL(clicked()), this, SLOT(on_pbOpenPcan_clicked()));
    //添加个水平的布局
    QHBoxLayout* hLayout = new QHBoxLayout();
    //把按钮丢进去
    hLayout->addWidget(pbAddModel);
    hLayout->addWidget(pbSend);
    hLayout->addWidget(pcn);
    hLayout->addWidget(cbPcan);
    hLayout->addWidget(reFresh);
    hLayout->addWidget(cbBitRate);
    hLayout->addWidget(Period);
    hLayout->addWidget(cycle);
    hLayout->addWidget(pbOpen);
    
   
    //把弹簧也丢进去
    hLayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));
    QCheckBox* checkTrace = new QCheckBox();
    checkTrace->setText(tr("数据监控   "));
    connect(checkTrace, &QCheckBox::stateChanged, this, &QtCanPlatform::on_checkTraceChanged);
    QLabel* mLabel = new QLabel();
    mLabel->setText(tr("当前型号"));
    hLayout->addWidget(checkTrace);
    hLayout->addWidget(mLabel);
    hLayout->addWidget(cbSelectModel);

    tableRecView = new QTableWidget();
    //设置表格为10列，不加这个内容不会显示的
    tableRecView->setColumnCount(10);
    tableRecView->horizontalHeader()->setVisible(false);
    tableRollTitle = new QTableWidget();
    tableRollTitle->setFixedHeight(30);
    tableRollData = new QTableWidget();
    tableRollData->horizontalHeader()->setVisible(false);
    tableRollData->verticalHeader()->setVisible(false);
    
    QPushButton* pbSaveCanData = new QPushButton(tr("保存数据"));
    QPushButton* pbClearCanData = new QPushButton(tr("清除数据"));
    QHBoxLayout* canButton = new QHBoxLayout();
    canButton->addWidget(pbSaveCanData);
    canButton->addWidget(pbClearCanData);
    connect(pbSaveCanData, SIGNAL(clicked()), this, SLOT(on_pbSaveCanData_clicked()));
    connect(pbClearCanData, SIGNAL(clicked()), this, SLOT(on_pbClearCanData_clicked()));
    canButton->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));
    //QVBoxLayout* canVLaout = new QVBoxLayout();
    //canVLaout->addLayout(canButton);
    //canVLaout->addWidget(tableRollTitle);
    //canVLaout->addWidget(tableRollData);
    //canVLaout->setSpacing(1);
    ////定义一个垂直布局
    //QVBoxLayout* vLayout = new QVBoxLayout();
    //vLayout->addLayout(hLayout);

    ////两个显示数据的垂直在左边
    //QVBoxLayout* vLayoutTable = new QVBoxLayout();
    //vLayoutTable->addWidget(tableView);
    //vLayoutTable->addWidget(tableRecView);
    //vLayoutTable->addLayout(canVLaout);
    //vLayoutTable->setStretch(0, 2);
    //vLayoutTable->setStretch(1, 3);
    //vLayoutTable->setStretch(2, 5);
   
    ////定义一个水平layout
    //QHBoxLayout* mainTLayout = new QHBoxLayout();
    ////两个显示数据的table在左
    //mainTLayout->addLayout(vLayoutTable);
    ////显示日志的在右
    //mainTLayout->addWidget(textBrowser);
    ////再跟顶部的按钮搞在一起
    //vLayout->addLayout(mainTLayout);
    //ui.centralWidget->setLayout(vLayout);
    if(cbSelectModel->count()>0)
        on_CurrentModelChanged(0);
    //QLOG_INFO() << "初始化界面完成";
    connect(this, &QtCanPlatform::sigNewRoll, this, &QtCanPlatform::on_setInToRollData);
    QSplitter* mainQSpli = new QSplitter(Qt::Vertical);
    QSplitter* topSpli = new QSplitter(mainQSpli);
    QWidget* mw = new QWidget;
    mw->setLayout(hLayout);
    topSpli->addWidget(mw);
    QSplitter* mainBottom = new QSplitter(Qt::Horizontal, mainQSpli);
   
    QWidget* mt = new QWidget;
    canButton->setContentsMargins(2, 10, 2, 2);
    mt->setLayout(canButton);
   
    QSplitter* bottom = new QSplitter(Qt::Vertical, mainBottom);
    bottom->addWidget(tableView);
    bottom->addWidget(tableRecView);
    bottom->addWidget(mt);
    bottom->addWidget(tableRollTitle);
    bottom->addWidget(tableRollData);
    //bottom->setHandleWidth(2);
    QSplitter* bootomright = new QSplitter(Qt::Vertical, mainBottom);
    bootomright->addWidget(textBrowser);
    mainBottom->setStretchFactor(0, 7);
    mainBottom->setStretchFactor(1, 2);
    QGridLayout* gg = new QGridLayout();
    gg->addWidget(mainQSpli);
    ui.centralWidget->setLayout(gg);
}

void QtCanPlatform::initData()
{
    //获取数据类指针
    qGboleData* qGb = qGboleData::getInstance();
    if (!qGb)return;
    //如果数据类未曾初始化（从文件读取），则进行初始化（从文件读取）
    if(!qGb->getIsInit())
        qGb->read();
}
bool QtCanPlatform::sendDataIntoTab()
{
    sendCanData.clear();
    if (!tableView)
        return false;
    int rcount = tableView->rowCount();
    for (int m = 0; m < rcount; m++)
        tableView->removeRow(rcount - m - 1);
    qGboleData* qGb = qGboleData::getInstance();
    if (!qGb)return false;
    if (currentModel > qGb->pGboleData.size() - 1 || currentModel< 0)
    {
        QMessageBox::warning(this, tr("warning"), tr("数据出错，当前型号不存在"));
        return false;
    }
    const protoData pTemp = qGb->pGboleData.at(currentModel);
    //canIdData cTemp;
   
    for (int i = 0; i < pTemp.cItem.size(); i++)
    {
        //取出操作为发送的信号
        if (1 == pTemp.cItem.at(i).opt)
        {
            sendCanData.push_back(pTemp.cItem.at(i));
           // break;
        }
    }
    if (sendCanData.size() <= 0)
    {
        QMessageBox::warning(this, tr("warning"), tr("该型号没有发送信号，请添加再操作"));
        return false;
    }
    
    for (int i = 0; i < sendCanData.size(); i++)
    {
        int num = sendCanData.at(i).pItem.size();
       
        for (int j = 0; j < num; j++)
        {
            int cr = tableView->rowCount();
            tableView->setRowCount(cr + 1);
            QCheckBox* cbSend = new QCheckBox();
            cbSend->setBaseSize(15, 15);
            if (sendCanData.at(i).isSend)
            {
                cbSend->setChecked(true);
            }
            //控件居中，就是让各方边距为相等
            QWidget* widget = new QWidget;
            QHBoxLayout* layout = new QHBoxLayout;
            layout->setSpacing(0);
            layout->setMargin(0);
            layout->setAlignment(Qt::AlignCenter);
            layout->addWidget(cbSend);
            widget->setLayout(layout);
            connect(cbSend, &QCheckBox::stateChanged, this, &QtCanPlatform::on_checkSendChanged);
            tableView->setCellWidget(cr, 0, cbSend);
            
            QComboBox* cb = new QComboBox();
            for (int k = 0; k < sendCanData.at(i).pItem.at(j).stl_itemProperty.size();++k)
            {
                QString name = sendCanData.at(i).pItem.at(j).stl_itemProperty.at(k).toWord;
                QColor bc = QColor(sendCanData.at(i).pItem.at(j).stl_itemProperty.at(k).r, sendCanData.at(i).pItem.at(j).stl_itemProperty.at(k).g, sendCanData.at(i).pItem.at(j).stl_itemProperty.at(k).b);
                cb->addItem(name);
                cb->setItemData(cb->count() - 1, bc, Qt::BackgroundColorRole);
            }
            if (cb->count() > 0)
            {
                int k = cb->currentIndex();
                QColor bc = QColor(sendCanData.at(i).pItem.at(j).stl_itemProperty.at(k).r, sendCanData.at(i).pItem.at(j).stl_itemProperty.at(k).g, sendCanData.at(i).pItem.at(j).stl_itemProperty.at(k).b);
          
                QString backcolor = "background-color:#"+ QString("%1").arg(bc.red(), 2, 16, QLatin1Char('0'))+
                    QString("%1").arg(bc.green(), 2, 16, QLatin1Char('0'))+
                    QString("%1").arg(bc.blue(), 2, 16, QLatin1Char('0'));
                cb->setStyleSheet(backcolor);
            }
            connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(on_cbSelectSendItemChanged(int)));
            
           
            tableView->setCellWidget(cr, 1, cb);
            //QString mt = "0x"+QString("%1").arg(sendCanData.at(i).CanId, QString::number(sendCanData.at(i).CanId).length(), 16).toUpper().trimmed();
            QString mt = sendCanData.at(i).strCanId;
            QTableWidgetItem* it1 = new QTableWidgetItem("0");
            it1->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
            tableView->setItem(cr, 2, it1);
            QTableWidgetItem* it3 = new QTableWidgetItem(mt);
            it3->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
            tableView->setItem(cr, 4, it3);
            QTableWidgetItem* it2 = new QTableWidgetItem(sendCanData.at(i).pItem.at(j).bitName);
            it2->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
            tableView->setItem(cr, 3, it2);
            QTableWidgetItem* it4 = new QTableWidgetItem(QString::number(sendCanData.at(i).pItem.at(j).startByte));
            it4->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
            tableView->setItem(cr, 5, it4);
            QTableWidgetItem* it5 = new QTableWidgetItem(QString::number(sendCanData.at(i).pItem.at(j).startBit));
            it5->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
            tableView->setItem(cr, 6, it5);
            QTableWidgetItem* it6 = new QTableWidgetItem(QString::number(sendCanData.at(i).pItem.at(j).bitLeng));
            it6->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
            tableView->setItem(cr, 7, it6);
          
        }
   }
    return true;
}

bool QtCanPlatform::recDataIntoTab()
{
   
    rollTitle.clear();
    recCanData.clear();
    if (!tableRecView)
        return false;
    int rcount = tableRecView->rowCount();
    for (int m = 0; m < rcount; m++)
        tableRecView->removeRow(rcount - m - 1);
    
    qGboleData* qGb = qGboleData::getInstance();
    if (!qGb)return false;
    if (currentModel > qGb->pGboleData.size() - 1 || currentModel < 0)
    {
        QMessageBox::warning(this, tr("warning"), tr("数据出错，当前型号不存在"));
        return false;
    }
    const protoData pTemp = qGb->pGboleData.at(currentModel);
   
    //canIdData cTemp;
   
    for (int i = 0; i < pTemp.cItem.size(); i++)
    {
        //取出操作为接收的信号
        if (0 == pTemp.cItem.at(i).opt)
        {
            recCanData.push_back(pTemp.cItem.at(i));
            // break;
        }
    }
    if (recCanData.size() <= 0)
    {
        QMessageBox::warning(this, tr("warning"), tr("该型号没有接收信号，请添加再操作"));
        return false;
    }
    rollTitle.append(tr("序号"));
    for (int i = 0; i < recCanData.size(); i++)
    {
        int num = recCanData.at(i).pItem.size();
        int cr = tableRecView->rowCount();
        //每加一行就要设置到表格去
        tableRecView->setRowCount(cr+1);
        int idex = 0;
        for (int j = 0; j <num; j++, idex++)
        {
            if (idex > 9)
            {
                idex = 0;
                cr = tableRecView->rowCount();
                tableRecView->setRowCount(cr + 2);
                cr += 1;
            }
            QTableWidgetItem* item = new QTableWidgetItem(recCanData.at(i).pItem.at(j).bitName);
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            item->font().setBold(true);
            //item->backgroundColor().setRgb(10, 10, 240);
            tableRecView->setItem(cr, idex, item);
           
            
            
            if (recCanData.at(i).pItem.at(j).isRoll)
            {
                rollTitle.append(recCanData.at(i).pItem.at(j).bitName);
            }
        }
        cr = tableRecView->rowCount();
        tableRecView->setRowCount(cr + 1);
        //tableRecView->insertRow(2);
    }
    tableRollTitle->setColumnCount(rollTitle.size());
    tableRollTitle->clear();
    tableRollTitle->setHorizontalHeaderLabels(rollTitle);
    QString stt = "QHeaderView::section {background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #0078d7, stop: 0.5 #0078d7,stop: 0.6 #0078d7, stop:1 #0078d7);color: white;border:1px solid;border-color:white;font-weight: bold;}QHeaderView{background-color:#0078d7}";
    tableRollTitle->horizontalHeader()->setStyleSheet(stt);
    //QHeaderView::section {background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #0078d7, stop: 0.5 #0078d7,stop: 0.6 #0078d7, stop:1 #0078d7);color: white;}
    for (int i = 0; i < rollTitle.size(); i++)
    {
        tableRollTitle->setColumnWidth(i, 100);
        
        
        //tableRollData->setColumnWidth(i, 40);
    }
    int col = tableRecView->columnCount();
    int row = tableRecView->rowCount();
    for (int h = 0; h < row; h++)
    {
        for (int g = 0; g < col; g++)
        {
            QTableWidgetItem* mItem = new QTableWidgetItem();
            mItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            QFont ff;
            ff.setBold(true);
            mItem->setFont(ff);
            tableRecView->setItem(h, g, mItem);
            if (h % 2 == 0)
            {
                tableRecView->item(h, g)->setBackgroundColor(recBackgroudColor);
                
            }
        }
    }
    int cr = 0;
    for (int i = 0; i < recCanData.size(); i++)
    {
        int num = recCanData.at(i).pItem.size();
        
        int idex = 0;
        for (int j = 0; j < num; j++, idex++)
        {
            if (idex > 9)
            {
                idex = 0;
                cr += 2;
            }
            QTableWidgetItem* item = new QTableWidgetItem(recCanData.at(i).pItem.at(j).bitName);
            item->setBackgroundColor(recBackgroudColor);
            item->setForeground(QBrush(recFontColor));
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            QFont ff;
            ff.setBold(true);
            item->setFont(ff);
            tableRecView->setItem(cr, idex, item);

        }
        cr += 2;
    }

    return true;
}
void QtCanPlatform::sendData()
{
    uchar s_Data[8];
    
    for (int i = 0; i < sendCanData.size(); i++)
    {
        if (!sendCanData.at(i).isSend)
            continue;
        memset(s_Data, 0, 8 * sizeof(uchar));
        unsigned int fream_id;
        bool b= intelProtocol(sendCanData.at(i), s_Data, fream_id);
        if (!b)
            continue;
        if(isTrace)
        {
            //QStringList binaryStr;
            QString hex;
            for (int k = 0; k < 8; ++k)
            {
                QString str = QString("%1").arg((uint8_t)s_Data[k], 2, 16, QLatin1Char('0'));
                hex += str+" ";
            }
            QString strId = "0x"+QString("%1").arg(fream_id, 8, 16, QLatin1Char('0')).toUpper();
            QLOG_INFO() << "Tx:" << strId << "  " << hex;
        }
       
        pcan->SendFrame(fream_id, s_Data);
        //_sleep(20);
    }

}
bool QtCanPlatform::intelProtocol(canIdData& cdata,uchar data[], unsigned int& fream_id)
{
    if (cdata.pItem.size() <= 0)
        return false;
    fream_id = cdata.strCanId.toUInt(NULL, 16);
    for (int i = 0; i < cdata.pItem.size() && i < 8; i++)
    {
        const protoItem &itemp = cdata.pItem.at(i);
        int startbyte = itemp.startByte;
        int startbit = itemp.startBit;
        int lengght = itemp.bitLeng;
        int senddd = itemp.send;
        startbyte = YB::InRang(0, 7, startbyte);
        if (lengght <= 8)
        {
            int pos = startbit % 8;             //起止位，模8，1字节8位，uchar是1节长度的
            uchar m_send = senddd << pos &0xff; //左移起止位，再&0xff，保证数据是不超过255
            data[startbyte] += m_send;                  //加上去，有可能其它的数据也在这个字节里
        }
        else if(lengght<=16)
        {
            int pos = startbit % 8;
            uchar m_send = senddd << pos & 0xff; //低8位
            data[startbyte] += m_send;
            m_send = senddd >> 8 & 0xff;         //高8位
            data[startbyte+1] += m_send;
        }
    }
    return true;
}
bool QtCanPlatform::motoProtocol(canIdData& cdata,uchar data[], unsigned int& fream_id)
{
    if (cdata.pItem.size() <= 0)
        return false;
    fream_id = cdata.strCanId.toUInt(NULL, 16);
    for (int i = 0; i < cdata.pItem.size() && i < 8; i++)
    {
        const protoItem& itemp = cdata.pItem.at(i);
        int startbyte = itemp.startByte;
        int startbit = itemp.startBit;
        int lengght = itemp.bitLeng;
        int senddd = itemp.send;
        startbyte = YB::InRang(0, 7, startbyte);
        if (lengght <= 8)
        {
            int pos = startbit % 8;             //起止位，模8，1字节8位，uchar是1节长度的
            uchar m_send = senddd << pos & 0xff; //左移起止位，再&0xff，保证数据是不超过255
            data[startbyte] += m_send;                  //加上去，有可能其它的数据也在这个字节里
        }
        else if (lengght <= 16)
        {
            int pos = startbit % 8;
            uchar m_send = senddd << pos & 0xff; //低8位
            data[startbyte] += m_send;
            m_send = senddd >> 8 & 0xff;         //高8位
            data[startbyte + 1] += m_send;
        }
    }
    return true;
}
void QtCanPlatform::recAnalyseIntel(unsigned int fream_id,QByteArray data)
{
    QStringList binaryStr;
    QString hex;
    for (int k = 0; k < data.size(); ++k)
    {
        QString str = QString("%1").arg((uint8_t)data[k], 8, 2, QLatin1Char('0'));
        binaryStr.append(str);

        hex += QString("%1").arg(str.toInt(NULL, 2), 2, 16, QLatin1Char('0')).toUpper() + " ";
    }
    if (isTrace)
    {
        QString strId = "0x" + QString("%1").arg(fream_id, 8, 16, QLatin1Char('0')).toUpper();
        QLOG_INFO() << "Rx:" << strId << "  " << hex;
    }

    bool isRoll = false;
    struct dFromStru {
        int index;
        float f1;
        float f2;
        int showCount;
    };
    for (int i = 0; i < recCanData.size(); i++)
    {
        uint currID = recCanData.at(i).strCanId.toUInt(NULL, 16);
        if (currID != fream_id)
            continue;
        std::vector<parseData>parseArr;
        std::vector<dFromStru>ddFF;
        for (int m = 0; m < recCanData.at(i).pItem.size(); ++m)
        {
            int startByte = recCanData.at(i).pItem.at(m).startByte;
            int startBit = recCanData.at(i).pItem.at(m).startBit;
            int startLenght = recCanData.at(i).pItem.at(m).bitLeng;
            float precision = recCanData.at(i).pItem.at(m).precision;
            int offset = recCanData.at(i).pItem.at(m).offset;
            parseData pd;
            int temp=0;
            QString datafrom = recCanData.at(i).pItem.at(m).dataFrom;
           
            
            //判断是否跨字节，起止位模8，得出是当前字节的起止位，再加个长度
            if (datafrom != "-1")
            {
                QStringList splt = datafrom.split("*");
                if (splt.size() > 1)
                {
                    dFromStru ddf;
                    ddf.index = m;
                    ddf.f1 = splt.at(0).toInt();
                    ddf.f2 = splt.at(1).toInt();
                    ddf.showCount = -1;
                    ddFF.push_back(ddf);
                }
            }
            else
            {
                //判断是否跨字节，起止位模8，得出是当前字节的起止位，再加个长度
                int len = startBit % 8 + startLenght;
                if (len <= 8)
                {   //不跨字节，这个就比较简单了
                    //15 14 13 12 11 10 9 8   7 6 5 4 3 2 1 0
                    //^高位在前，低位在后
                    temp = binaryStr[startByte].mid(8 - (startLenght + (startBit % 8)), startLenght).toInt(NULL, 2) * precision + offset;
                }
                else if (len <= 16)
                {
                    temp = (binaryStr[startByte + 1].mid(8 - (startLenght - (8 - startBit % 8)), startLenght - (8 - startBit % 8)) + binaryStr[startByte].mid(0, 8 - (startBit % 8))).toInt(NULL, 2);
                }
                {
                    //跨三个字节的，应该没有
                }
            }
            
            pd.name = recCanData.at(i).pItem.at(m).bitName;
            pd.value = temp;
            pd.toWord = QString::number(temp);
            pd.color.r = 255;
            pd.color.g = 255;
            pd.color.b = 255;
            
            // std::map<QString, cellProperty>& tt = recCanData.at(i).pItem.at(m).itemProperty;
            std::vector<cellProperty>& ss = recCanData.at(i).pItem.at(m).stl_itemProperty;

            //如果是需要滚动显示的
            if (recCanData.at(i).pItem.at(m).isRoll)
            {
                isRoll = true;
                int index = 0;
                if (YB::nameInVector(RollShowData, recCanData.at(i).pItem.at(m).bitName, index))
                {
                    RollShowData.at(index).value = temp;
                }
                else
                {
                    RollStruct rr;
                    rr.name = recCanData.at(i).pItem.at(m).bitName;
                    rr.value = temp;
                    RollShowData.push_back(rr);
                }
                if (datafrom != "-1" && ddFF.size() > 0)
                    ddFF.at(ddFF.size() - 1).showCount++;
            }

            std::vector<dFromStru>::iterator iB = ddFF.begin();
            std::vector<dFromStru>::iterator iE = ddFF.end();
            int count0 = 0;
            while (iB != iE)
            {
                int max = iB->f1 > iB->f2 ? iB->f1 : iB->f2;
                if (m - 1 > max - 1)
                {
                    parseArr.at(iB->index).value = parseArr.at(iB->f1 - 1).value * parseArr.at(iB->f2 - 1).value;
                    parseArr.at(iB->index).toWord = QString::number(parseArr.at(iB->index).value);
                    if (RollShowData.size() - 1 >= iB->showCount)
                    {
                        RollShowData.at(iB->showCount).value = parseArr.at(iB->index).value;
                    }
                    ddFF.erase(iB);
                    break;
                }
                iB++;
            }
            for (int i = 0; i < ss.size(); i++)
            {
                if (ss.at(i).value.toInt() == temp)
                {
                    pd.color.r = ss.at(i).r;
                    pd.color.g = ss.at(i).g;
                    pd.color.b = ss.at(i).b;
                    pd.toWord = ss.at(i).toWord;
                    break;
                }

            }
            parseArr.push_back(pd);
        }
        //判断map里面是否已经存在有了
        if (YB::keyInMap(showTableD, QString::number(fream_id)))
        {
            showTableD[QString::number(fream_id)] = parseArr;
        }
        else
        {
            showTableD.insert({ QString::number(fream_id),parseArr });
        }

    }
    //recCanData.clear();
    if (!tableRecView)
        return;
  /*  int rcount = tableRecView->rowCount();
    for (int m = 0; m < rcount; m++)
        tableRecView->removeRow(rcount - m - 1);*/

    std::map<QString, std::vector<parseData>>::iterator iBegin = showTableD.begin();
    std::map<QString, std::vector<parseData>>::iterator iEnd = showTableD.end();
    int cr = 0;
    while (iBegin != iEnd)
    {
        
        //每加一行就要设置到表格去
        int num = iBegin->second.size();
        int idex = 0;
        for (int j = 0; j < num; j++, idex++)
        {
            if (idex > 9)
            {
                idex = 0;
                cr += 2;
            }

            QString tnamp = iBegin->second.at(j).name;
            QString toword = iBegin->second.at(j).toWord;
            tableRecView->setItem(cr, idex, new QTableWidgetItem(tnamp));
            QFont ff;
            ff.setBold(true);
            tableRecView->item(cr, idex)->setFont(ff);
            tableRecView->item(cr, idex)->setTextAlignment(Qt::AlignCenter);
            tableRecView->item(cr, idex)->setBackgroundColor(recBackgroudColor);
            tableRecView->item(cr, idex)->setForeground(QBrush(recFontColor));
            tableRecView->setItem(cr + 1, idex, new QTableWidgetItem(toword));
            tableRecView->item(cr + 1, idex)->setBackgroundColor(QColor(iBegin->second.at(j).color.r, iBegin->second.at(j).color.g, iBegin->second.at(j).color.b));
            tableRecView->item(cr + 1, idex)->setTextAlignment(Qt::AlignCenter);

        }
        cr += 2;
        iBegin++;

    }
    if (isRoll)
        emit sigNewRoll();
}
void QtCanPlatform::recAnalyseMoto(unsigned int fream_id, QByteArray data)
{
    QStringList binaryStr;
    QString hex;
    for (int k = 0; k < data.size(); ++k)
    {
        QString str = QString("%1").arg((uint8_t)data[k], 8, 2, QLatin1Char('0'));
        binaryStr.append(str);

        hex += QString("%1").arg(str.toInt(NULL, 2), 2, 16, QLatin1Char('0')).toUpper() + " ";
    }
    if (isTrace)
    {
        QString strId = "0x" + QString("%1").arg(fream_id, 8, 16, QLatin1Char('0')).toUpper();
        QLOG_INFO() << "Rx:" << strId << "  " << hex;
    }

    bool isRoll = false;
    //std::vector<std::vector<parseData>>showVec;
    struct dFromStru {
        int index;
        float f1;
        float f2;
        int showCount;
    };
    for (int i = 0; i < recCanData.size(); i++)
    {
        uint currID = recCanData.at(i).strCanId.toUInt(NULL, 16);
        if (currID != fream_id)
            continue;
        std::vector<parseData>parseArr;
        std::vector<dFromStru>ddFF;
        int countShow = 0;
        for (int m = 0; m < recCanData.at(i).pItem.size(); ++m)
        {
            int startByte = recCanData.at(i).pItem.at(m).startByte;
            int startBit = recCanData.at(i).pItem.at(m).startBit;
            int startLenght = recCanData.at(i).pItem.at(m).bitLeng;
            float precision = recCanData.at(i).pItem.at(m).precision;
            int offset = recCanData.at(i).pItem.at(m).offset;
            QString datafrom = recCanData.at(i).pItem.at(m).dataFrom;
            parseData pd;
            int temp=0;
            //判断是否跨字节，起止位模8，得出是当前字节的起止位，再加个长度
            if (datafrom != "-1")
            {
                QStringList splt = datafrom.split("*");
                if (splt.size() > 1)
                {
                    dFromStru ddf;
                    ddf.index = m;
                    ddf.f1 = splt.at(0).toInt();
                    ddf.f2 = splt.at(1).toInt();
                    ddFF.push_back(ddf);
                }
            }
            else
            {
                int len = startBit % 8 + startLenght;
                if (len <= 8)
                {   //不跨字节，这个就比较简单了
                    //15 14 13 12 11 10 9 8   7 6 5 4 3 2 1 0
                    //^高位在前，低位在后
                    temp = binaryStr[startByte].mid(startBit % 8, startLenght).toInt(NULL, 2) * precision + offset;
                }
                else if (len <= 16)
                {
                    temp = (binaryStr[startByte].mid(0, 8 - (startBit % 8)) + binaryStr[startByte + 1].mid(startBit % 8, startLenght - (8 - startBit % 8))).toInt(NULL, 2);
                }
                {
                    //跨三个字节的，应该没有
                }
            }
            
            pd.name = recCanData.at(i).pItem.at(m).bitName;
            pd.value = temp;
            pd.toWord = QString::number(temp);
            pd.color.r = 255;
            pd.color.g = 255;
            pd.color.b = 255;
            
            // std::map<QString, cellProperty>& tt = recCanData.at(i).pItem.at(m).itemProperty;
            std::vector<cellProperty>& ss = recCanData.at(i).pItem.at(m).stl_itemProperty;

            //如果是需要滚动显示的
            if (recCanData.at(i).pItem.at(m).isRoll)
            {
                isRoll = true;
                int index = 0;
                if (YB::nameInVector(RollShowData, recCanData.at(i).pItem.at(m).bitName, index))
                {
                    RollShowData.at(index).value = temp;
                }
                else
                {
                    RollStruct rr;
                    rr.name = recCanData.at(i).pItem.at(m).bitName;
                    rr.value = temp;
                    RollShowData.push_back(rr);
                }
                if (datafrom != "-1" && ddFF.size()>0)
                    ddFF.at(ddFF.size()-1).showCount++;
            }

            std::vector<dFromStru>::iterator iB = ddFF.begin();
            std::vector<dFromStru>::iterator iE = ddFF.end();
            int count0 = 0;
            while (iB != iE)
            {
                int max = iB->f1 > iB->f2 ? iB->f1 : iB->f2;
                if (m >= max - 1)
                {
                    parseArr.at(iB->index).value = parseArr.at(iB->f1 - 1).value * parseArr.at(iB->f2 - 1).value;
                    parseArr.at(iB->index).toWord = QString::number(parseArr.at(iB->index).value);
                    if (RollShowData.size() - 1 >= iB->showCount)
                    {
                        RollShowData.at(iB->showCount).value = parseArr.at(iB->index).value;
                    }
                    ddFF.erase(iB);
                    break;
                }
                iB++;
            }

            for (int i = 0; i < ss.size(); i++)
            {
                if (ss.at(i).value.toInt() == temp)
                {
                    pd.color.r = ss.at(i).r;
                    pd.color.g = ss.at(i).g;
                    pd.color.b = ss.at(i).b;
                    pd.toWord = ss.at(i).toWord;
                    break;
                }

            }
            parseArr.push_back(pd);
        }
        //判断map里面是否已经存在有了
        if (YB::keyInMap(showTableD, QString::number(fream_id)))
        {
            showTableD[QString::number(fream_id)] = parseArr;
        }
        else
        {
            showTableD.insert({ QString::number(fream_id),parseArr });
        }

    }
    //recCanData.clear();
    if (!tableRecView)
        return;
    int rcount = tableRecView->rowCount();
    for (int m = 0; m < rcount; m++)
        tableRecView->removeRow(rcount - m - 1);
    //for (int n = 0; n < showVec.size(); n++)
    std::map<QString, std::vector<parseData>>::iterator iBegin = showTableD.begin();
    std::map<QString, std::vector<parseData>>::iterator iEnd = showTableD.end();
    int cr = 0;
    while (iBegin != iEnd)
    {
        int num = iBegin->second.size();
        int idex = 0;
        for (int j = 0; j < num; j++, idex++)
        {
            if (idex > 9)   //每行10列
            {
                idex = 0;
                cr += 2;
            }

            QString tnamp = iBegin->second.at(j).name;
            QString toword = iBegin->second.at(j).toWord;
            tableRecView->setItem(cr, idex, new QTableWidgetItem(tnamp));
            QFont ff;
            ff.setBold(true);
            tableRecView->item(cr, idex)->setFont(ff);
            tableRecView->item(cr, idex)->setTextAlignment(Qt::AlignCenter);
            tableRecView->item(cr, idex)->setBackgroundColor(recBackgroudColor);
            tableRecView->item(cr, idex)->setForeground(QBrush(recFontColor));
            tableRecView->setItem(cr + 1, idex, new QTableWidgetItem(toword));
            tableRecView->item(cr + 1, idex)->setBackgroundColor(QColor(iBegin->second.at(j).color.r, iBegin->second.at(j).color.g, iBegin->second.at(j).color.b));
            tableRecView->item(cr + 1, idex)->setTextAlignment(Qt::AlignCenter);

        }
        cr += 2;
        iBegin++;

    }
    if (isRoll)
        emit sigNewRoll();
}
void QtCanPlatform::getModelTitle()
{

    rollTitle;
}
void QtCanPlatform::on_CurrentModelChanged(int index)
{
    currentModel = index;
    sendDataIntoTab();
    recDataIntoTab();
}
void QtCanPlatform::on_pbSend_clicked(bool clicked)
{
    if (clicked)
    {
        int period = cycle->text().toInt();
        if (period < 50)
            period = 50;
        sendTimer->start(period);
        cycle->setEnabled(false);
        cbBitRate->setEnabled(false);
        reFresh->setEnabled(false);
    }
    else
    {
        sendTimer->stop();
        cycle->setEnabled(true);
        cbBitRate->setEnabled(true);
        reFresh->setEnabled(true);
    }
}
void QtCanPlatform::on_pbRefreshDevice_clicked()
{
    QStringList canStr = pcan->DetectDevice();
    for (int i = 0; i < canStr.size(); ++i)
    {
        cbPcan->addItem(canStr.at(i));
    }
}
void QtCanPlatform::on_pbOpenPcan_clicked()
{
    if (pcanIsOpen)
    {
        pcan->CloseCan();
        pbOpen->setText(tr("打开设备"));
        cbBitRate->setEnabled(true);
        cbPcan->setEnabled(true);
        pbSend->setChecked(false);
        pbSend->setEnabled(false);
        pcanIsOpen = false;
        sendTimer->stop();
    }
    else
    {
        if (cbPcan->count() <= 0)
            return;
        int curindex = cbBitRate->currentIndex();
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
        bool b = pcan->ConnectDevice(cbPcan->currentIndex(), bitRate);
        if (!b)
        {
            QMessageBox::warning(NULL, tr("错误"), tr("打开CAN失败,请检测设备是否被占用或者已经连接？"));
            return;
        }
        pbOpen->setText(tr("关闭设备"));
        cbBitRate->setEnabled(false);
        cbPcan->setEnabled(false);
        pbSend->setEnabled(true);
        pcanIsOpen = true;
    }
    
}
/*
* @brief: 这个是为了建立单元格被编辑后的槽，因为编辑之前必须要双击。
* @parm: row 发生变化的所在行
* @parm: rcol 发生变化的所在列
* return: 无
*/
void QtCanPlatform::on_tableDoubleClicked(int, int)
{
    connect(tableView, SIGNAL(cellChanged(int, int)), this, SLOT(on_tableClicked(int, int)));
}

/*
* @brief: 发送数据表的数据发生变化的槽函数
* @parm: row 发生变化的所在行
* @parm: rcol 发生变化的所在列
* return: 无
*/
void QtCanPlatform::on_tableClicked(int row, int col)
{
    int rows = tableView->rowCount();
    if (row > rows - 1)
    {
        //断开信号槽，因为添加数据的时候会触发这个
        disconnect(tableView, SIGNAL(cellChanged(int, int)), this, SLOT(on_tableClicked(int, int)));
        return;
    }
    QString sendId = tableView->item(row, 2)->text();
    QString senddt = tableView->item(row, 1)->text();
    for (int i = 0; i < sendCanData.size(); i++)
    {
        //找出数据所在的型号
        if (sendId.trimmed() == sendCanData.at(i).strCanId)
        {
            int num = 0;
            for (int m = 0; m < i; m++)
            {
                num += sendCanData.at(m).pItem.size();
            }
            if (row > sendCanData.at(i).pItem.size() - 1- num)
            {
                //断开信号槽，因为添加数据的时候会触发这个
                disconnect(tableView, SIGNAL(cellChanged(int, int)), this, SLOT(on_tableClicked(int, int)));
                return;
            }
            //存储修改后的值
            sendCanData.at(i).pItem.at(row-num).send = senddt.toInt();
        }
    }
    //断开信号槽，因为添加数据的时候会触发这个
    disconnect(tableView, SIGNAL(cellChanged(int, int)), this, SLOT(on_tableClicked(int, int)));
       
}
/*
* @brief: pcan返回数据的槽函数，并且解析
* @parm: fream_id 报文的标识
* @parm: data 报文数据
* return: 无
*/
void QtCanPlatform::on_ReceiveData(uint fream_id, QByteArray data)
{
    
    int index = cbSelectModel->currentIndex();
    
    qGboleData* qGb = qGboleData::getInstance();
    if (!qGb)return;
    if (index > qGb->pGboleData.size()-1)
        return;
    if (0 == qGb->pGboleData.at(index).agreement)
    {
        recAnalyseIntel(fream_id, data);
    }
    else
    {
        recAnalyseMoto(fream_id, data);
    }
}
/*
* @brief: 设置窗口关闭时，不管三七二十一，刷新一下显示的数据
* @parm: 无
* return: 无
*/
void QtCanPlatform::on_SettingWidowsClose()
{
    int index = cbSelectModel->currentIndex();
    on_CurrentModelChanged(index);
}
/*
* @brief:发送字段的下拉框发生变化时的响应槽函数
*/
void QtCanPlatform::on_cbSelectSendItemChanged(int index)
{
    QComboBox* cb = dynamic_cast<QComboBox*>(sender());
    if (!cb)
    {
        QLOG_INFO() << "当前的QComboBox sender 无效";
        return;
    }
    QModelIndex cbIndex = tableView->indexAt(QPoint(cb->geometry().x(), cb->geometry().y()));
    int cbRow = cbIndex.row();
    int cbCol = cbIndex.column();
    unsigned int cbInId = tableView->item(cbRow, 4)->text().toUInt(NULL, 16);
    
    for (int k = 0; k < sendCanData.size(); k++)
    {
        if (sendCanData.at(k).strCanId.toUInt(NULL,16) != cbInId)
            continue;
        int cnum=0;
        //这步很关键，算出前面的型号有多少个item
        for (int m = 0; m < k ; m++)
            cnum += sendCanData.at(m).pItem.size();
        if (cbCol == 0)
        {
           //当前型号的当前下标：当前行-前面的型号的item个数（cbRow - cnum）
           sendCanData.at(k).pItem.at(cbRow - cnum).send = sendCanData.at(k).pItem.at(cbRow- cnum).stl_itemProperty.at(index).value.toInt();
           tableView->setItem(cbRow,1, new QTableWidgetItem(sendCanData.at(k).pItem.at(cbRow - cnum).stl_itemProperty.at(index).value));
           tableView->item(cbRow, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
           QColor bc = QColor(sendCanData.at(k).pItem.at(cbRow - cnum).stl_itemProperty.at(index).r, sendCanData.at(k).pItem.at(cbRow - cnum).stl_itemProperty.at(index).g, sendCanData.at(k).pItem.at(cbRow- cnum).stl_itemProperty.at(index).b);
          
           QString backcolor = "background-color:#" + QString("%1").arg(bc.red(), 2, 16, QLatin1Char('0')) +
               QString("%1").arg(bc.green(), 2, 16, QLatin1Char('0')) +
               QString("%1").arg(bc.blue(), 2, 16, QLatin1Char('0'));
           cb->setStyleSheet(backcolor);
        }
    }
}
void QtCanPlatform::on_checkSendChanged(int check)
{
    QCheckBox* cb = dynamic_cast<QCheckBox*>(sender());
    if (!cb)
    {
        QLOG_INFO() << "当前的QComboBox sender 无效";
        return;
    }
    QModelIndex cbIndex = tableView->indexAt(QPoint(cb->geometry().x(), cb->geometry().y()));
    int cbRow = cbIndex.row();
    int cbCol = cbIndex.column();
    unsigned int cbInId = tableView->item(cbRow, 4)->text().toUInt(NULL, 16);
    bool b = check > 0 ? true : false;
    for (int k = 0; k < sendCanData.size(); k++)
    {
        if (sendCanData.at(k).strCanId.toUInt(NULL, 16) != cbInId)
            continue;
        sendCanData.at(k).isSend = b;
        break;
    }
    int tbRowCount = tableView->rowCount();
    for (int m = 0; m < tbRowCount; m++)
    {
        unsigned int cbInId2 = tableView->item(m, 4)->text().toUInt(NULL, 16);
        if(cbInId!= cbInId2)
            continue;
        QCheckBox* cb2 = dynamic_cast<QCheckBox*>(tableView->cellWidget(m,0));
        if(cb2)
            cb2->setChecked(b);
    }
}
/*
* @brief：把要滚动显示的数据添加到表格里面
* @param：无
* @return：void
*/
void QtCanPlatform::on_setInToRollData()
{
    //设置列数
    tableRollData->setColumnCount(rollTitle.size());
    //顺便设置列宽
    for (int i = 0; i < rollTitle.size(); i++)
    {
        tableRollData->setColumnWidth(i, 100);
    }
    int row = tableRollData->rowCount();
    //增加一行
    tableRollData->setRowCount(row + 1);
    QString dTime = QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-ss-zzz");
    uint curCount = strSaveList.size()+1;
    QString dTemp= QString::number(curCount)+","+ dTime+",";
    excelTitle = "序号,北京时间,";
    //序号
    tableRollData->setItem(row, 0, new QTableWidgetItem(QString::number(curCount)));
    tableRollData->item(row, 0)->setTextAlignment(Qt::AlignCenter);
    for (int i = 0;i< RollShowData.size(); i++)
    {
        tableRollData->setItem(row, i+1, new QTableWidgetItem(QString::number(RollShowData.at(i).value)));
        tableRollData->item(row, i + 1)->setTextAlignment(Qt::AlignCenter);
        dTemp += QString::number(RollShowData.at(i).value) + ",";
        //这个是保存到excel的表头
        excelTitle += RollShowData.at(i).name + ",";
    }
    //表格永远显示底部
    tableRollData->scrollToBottom();
    //移除最后一个逗号
    dTemp.remove(dTemp.size() - 1, 1);
    excelTitle.remove(excelTitle.size() - 1, 1);
    //放到一个list存着
    strSaveList.append(dTemp);
    //够数的就保存到excel
    if (strSaveList.size() >= saveListNum)
    {
        saveCanData();
    }
}
/*
* @brief:保存CAN数据
* @param:无
* @return:无
*/
void QtCanPlatform::saveCanData()
{
    QString appPath = QApplication::applicationDirPath() + "/PCAN-DATA/";
    QDir dd(appPath);
    if (!dd.exists())
    {
        dd.mkpath(appPath);
    }
    appPath += QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-ss-zzz") + ".xlsx";
    if (strSaveList.size() < 1)
        return;
    //tableRollData->clear();
   
    saveData->setTitle(excelTitle);
    saveData->SaveData(strSaveList, strSaveList.size(), appPath);
    on_pbClearCanData_clicked();
    //strSaveList.clear();
}
void QtCanPlatform::on_checkTraceChanged(int check)
{
    if (check)
    {
        isTrace = true;
    }
    else
        isTrace = false;
}
void QtCanPlatform::on_pbSaveCanData_clicked()
{
    saveCanData();
}
void QtCanPlatform::on_pbClearCanData_clicked()
{
    strSaveList.clear();
    int rcount = tableRollData->rowCount();
    for (int m = 0; m < rcount; m++)
        tableRollData->removeRow(rcount - m - 1);
    
}
void QtCanPlatform::initLogger()
{
    Logger& logger = Logger::instance();    //初始化
    logger.setLoggingLevel(QsLogging::InfoLevel);  //设置写入等级
    QString logsfile = QApplication::applicationDirPath() + "/logs";
    QDir dir(logsfile);
    if (!dir.exists())
    {
        dir.mkpath(logsfile);
    }
    //设置log位置为exe所在目录
    const QString pp = logsfile + "/log.txt";
    //const QString sLogPath(QDir(QCoreApplication::applicationDirPath()).filePath("/logs/log.txt"));

    // 2. 添加两个destination
    DestinationPtr fileDestination(DestinationFactory::MakeFileDestination(
        pp, EnableLogRotation, MaxSizeBytes(1024*1024*100), MaxOldLogCount(100)));
    DestinationPtr debugDestination(DestinationFactory::MakeDebugOutputDestination());
    //DestinationPtr functorDestination(DestinationFactory::MakeFunctorDestination(&logFunction));

    //这样和槽函数连接
    DestinationPtr sigsSlotDestination(DestinationFactory::MakeFunctorDestination(this, SLOT(logSlot(QString, int))));
    //添加debug流向
    logger.addDestination(debugDestination);
    //添加文件流向
    logger.addDestination(fileDestination);
    //logger.addDestination(functorDestination);
    //添加槽函数流向
    logger.addDestination(sigsSlotDestination);
    QLOG_INFO() << "欢迎使用CANPlatform";
   
}
void QtCanPlatform::destroyLogger()
{
    QsLogging::Logger::destroyInstance();
}
void QtCanPlatform::logSlot(const QString& message, int level)
{
    //qUtf8Printable使用这个宏避免中文乱码
    textBrowser->append(qUtf8Printable(message));
    if (maxTextBrowser > 200)
    {
        textBrowser->clear();
        maxTextBrowser = 0;
    }
        
    maxTextBrowser++;
}
/*
* @brief: 打开设置窗口，这里主要设置各类的字段
* @parm: 无
* return: 无
*/
void QtCanPlatform::qCanSettingShow()
{
    if (!canSetting) {
        canSetting = new QCanSetting();
        connect(canSetting, &QCanSetting::settingWidowsClose, this, &QtCanPlatform::on_SettingWidowsClose);
    }
    canSetting->show();
}