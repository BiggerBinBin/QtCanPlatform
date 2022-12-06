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
#include <qsettings.h>
#include <WinBase.h>

using namespace QsLogging;

QString qmyss = "QComboBox{border: 1px solid gray;border-radius: 5px;padding:1px 2px 1px 2px;s}\
QComboBox::drop-down{subcontrol-origin: padding;subcontrol-position: top right;width: 15px;border-left-width: 1px;border-left-color: darkgray;border-left-style: solid;border-top-right-radius: 3px;border-bottom-right-radius: 3px;}";
QString rollTitleStyle = "QHeaderView::section {background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #0078d7, stop: 0.5 #0078d7,stop: 0.6 #0078d7, stop:1 #0078d7);color: white;border:1px solid;border-color:white;font-weight: bold;}QHeaderView{background-color:#0078d7}";

QtCanPlatform::QtCanPlatform(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    //防止休眠
    SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED);
    initLogger();
    this->setStyleSheet(qmyss);
    initUi();
    sendTimer = new QTimer();
    connect(sendTimer, &QTimer::timeout, this, &QtCanPlatform::sendData);
    this->setWindowTitle(tr("PHU-CAN-APP V1.11.30"));
    this->showMaximized();
    connect(this, &QtCanPlatform::sigEndRunWork, this, &QtCanPlatform::on_recSigEndRunWork);
    readSetFile();
    //博世的获取版本定时器
    t_GetVer = new QTimer(this);

}

QtCanPlatform::~QtCanPlatform()
{
    //恢复休眠
    SetThreadExecutionState(ES_CONTINUOUS);
    if (canSetting) { delete canSetting; canSetting = nullptr; }
    if (tableView) { delete tableView; tableView = nullptr; }
    if (textBrowser) { delete textBrowser; textBrowser = nullptr; }
    if (tableRecView) { delete tableRecView; tableRecView = nullptr; }
    if (pcan) {
        delete pcan; 
        pcan = nullptr;
    }
    for (int i = 0; i < 4; i++)
    {
        if (pcanArr[i]) { delete pcanArr[i]; pcanArr[i] = nullptr; }

        if (saveDataArr[i]) { delete saveDataArr[i]; saveDataArr[i] = nullptr; }
    }
    if (kcan)
    {
        //kcan->closeCAN();
        delete kcan;
        kcan = nullptr;
    }
    if (canayst)
    {
        delete canayst;
        canayst = nullptr;
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
    if (dCtrl)
    {
        delete dCtrl;
        dCtrl = nullptr;
    }
    if (t_GetVer)
    {
        delete t_GetVer;
        t_GetVer = nullptr;
    }
    if (progress)
    {
        delete progress; progress = nullptr;
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
    for (int i = 0; i < 4; i++)
    {
        if (pcanArr[i]) { pcanArr[i]->CloseCan(); }
    }
    if (kcan)
    {
        kcan->closeCAN();
    }
    if (canayst)
    {
        canayst->stopped = true;
        canayst->stop();
        canayst->closeCAN();
    }
    if (dCtrl)
        dCtrl->closeSomething();
    event->accept();
}

void QtCanPlatform::initUi()
{
    this->setWindowIcon(QIcon(":/QtCanPlatform/app-logo.ico"));
    lostQTimer = new QTimer();
    connect(lostQTimer, SIGNAL(timeout()), this, SLOT(on_recTimeout()));
    //QLOG_INFO() << "初始化界面中……";
    //这个是显示内容的
    saveData = new DataSave(this);
    for (int i = 0; i < 4; i++)
    {
        saveDataArr[i] = new DataSave(this);
    }
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
    QPushButton* pbClearText = new QPushButton(this);
    connect(pbClearText, &QPushButton::clicked, this, &QtCanPlatform::on_pbClearLogShow_clicked);
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
    cbCanType = new QComboBox();
    cbCanType->addItem(tr("选择PCAN"));
    cbCanType->addItem(tr("选择Kvaser"));
    cbCanType->addItem(tr("选择CANayst"));
    connect(cbCanType, SIGNAL(currentIndexChanged(int)), this, SLOT(on_cbCanType_currentIndexChanged(int)));
    cbPcan = new QComboBox();
   
    for (int i = 0; i < 4; i++)
    {
        lostQTimerArr[i] = new QTimer(this);
        pcanArr[i] = new PCAN(this);
        pcanArr[i]->DetectDevice();
        connect(pcanArr[i], SIGNAL(getProtocolData(quint32, QByteArray)), this, SLOT(on_ReceiveDataMulti(quint32, QByteArray)));
        connect(lostQTimerArr[i], &QTimer::timeout, this, &QtCanPlatform::on_recTimeoutMutil);
        

    }
    canayst = new CANThread();
    connect(canayst, SIGNAL(getProtocolData(int, quint32, QByteArray)), this, SLOT(on_ReceiveData(int, quint32, QByteArray)));
    kcan = new kvaser();
    connect(kcan, SIGNAL(getProtocolData(int, quint32, QByteArray)), this, SLOT(on_ReceiveData(int, quint32, QByteArray)));
    pcan = new PCAN(this);
    connect(pcan, SIGNAL(getProtocolData(quint32, QByteArray)), this, SLOT(on_ReceiveData(quint32, QByteArray)));
    pcan->DetectDevice();
    QStringList canStr = pcanArr[0]->DetectDevice();
    for (int i = 0; i < canStr.size(); ++i)
    {
        cbPcan->addItem(canStr.at(i));
    }
    QLOG_INFO() << "CAN Number:" << canStr.size();
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
    cbIsMutil = new QComboBox();
    cbIsMutil->addItem(tr("打开单个"));
    cbIsMutil->addItem(tr("打开全部"));
    cbIsMutil->setCurrentIndex(0);
    pbOpen = new QPushButton(tr("打开设备"));
    connect(pbOpen, SIGNAL(clicked()), this, SLOT(on_pbOpenPcan_clicked()));
    QLabel* cstate = new QLabel(tr(" 通信状态："));
    communicaLabel = new QLabel(tr("待机中…"));
    communicaLabel->setStyleSheet("background-color:yellow");
    //添加个水平的布局
    QHBoxLayout* hLayout = new QHBoxLayout();
    //把按钮丢进去
    hLayout->addWidget(pbAddModel);
    hLayout->addWidget(pbSend);
    hLayout->addWidget(cbCanType);
    hLayout->addWidget(cbPcan);
    hLayout->addWidget(reFresh);
    hLayout->addWidget(cbBitRate);
    hLayout->addWidget(Period);
    hLayout->addWidget(cycle);
    hLayout->addWidget(cbIsMutil);
    hLayout->addWidget(pbOpen);
    hLayout->addWidget(cstate);
    hLayout->addWidget(communicaLabel);
   
    //把弹簧也丢进去
    hLayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));

    QLineEdit* debugLine = new QLineEdit("-1");
    connect(debugLine, &QLineEdit::editingFinished, this, &QtCanPlatform::on_lineEdit_editingFinished);
    QCheckBox* checkTrace = new QCheckBox();
    checkTrace->setText(tr("数据监控   "));
    connect(checkTrace, &QCheckBox::stateChanged, this, &QtCanPlatform::on_checkTraceChanged);
    QLabel* mLabel = new QLabel();
    mLabel->setText(tr("当前型号"));
    hLayout->addWidget(debugLine);
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
    
    tabRecBox = new QTabWidget(this);
    tabRollBox = new QTabWidget(this);

    //四通道分别设置
    for (int i = 0; i < 4; i++)
    {
        w[i] = new QWidget(this);
        tableArray[i] = new QTableWidget(this);
        tableArray[i]->setColumnCount(10);
        tableArray[i]->horizontalHeader()->setVisible(false);
        QVBoxLayout* vLayoutTable = new QVBoxLayout();
        vLayoutTable->addWidget(tableArray[i]);
        tabRecBox->addTab(w[i], tr("通道") + QString::number(i+1));
        tabRecBox->widget(i)->setLayout(vLayoutTable);

       

        QVBoxLayout* vLayoutTable2 = new QVBoxLayout();
        w_rolling[i] =  new QTableWidget(this);
        tableRollDataArray[i]= new QTableWidget(this);
        tableRollTitleArray[i]= new QTableWidget(this);
        //设置表头固定宽
        tableRollTitleArray[i]->setFixedHeight(30);
        //设置数据显示表不显示表头
        tableRollDataArray[i]->horizontalHeader()->setVisible(false);
        tableRollDataArray[i]->verticalHeader()->setVisible(false);
        vLayoutTable2->addWidget(tableRollTitleArray[i]);
        vLayoutTable2->addWidget(tableRollDataArray[i]);
        tabRollBox->addTab(w_rolling[i], tr("通道") + QString::number(i + 1));
        tabRollBox->widget(i)->setLayout(vLayoutTable2);
        
    }
   
    
    

    //界面左边下方的几个按钮
    QPushButton* pbSaveCanData = new QPushButton(tr("保存数据"));
    QPushButton* pbClearCanData = new QPushButton(tr("清除数据"));
    pbGetVer = new QPushButton(tr("获取版本号(博世)"));
    pbGetVer->setCheckable(true);
    QHBoxLayout* canButton = new QHBoxLayout();
    canButton->addWidget(pbSaveCanData);
    canButton->addWidget(pbClearCanData);
    canButton->addWidget(pbGetVer);
    canButton->addWidget(&labelVer1);
    canButton->addWidget(&labelVer2);
    canButton->addWidget(&labelVer3);
    canButton->addWidget(&labelVer4);
    connect(pbSaveCanData, SIGNAL(clicked()), this, SLOT(on_pbSaveCanData_clicked()));
    connect(pbClearCanData, SIGNAL(clicked()), this, SLOT(on_pbClearCanData_clicked()));
    connect(pbGetVer, SIGNAL(clicked(bool)), this, SLOT(on_pbGetVer_clicked(bool)));
    canButton->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));
    //默认第一个型号为当前型号
    if(cbSelectModel->count()>0)
        on_CurrentModelChanged(0);
    //建立动态显示报文的槽
    connect(this, &QtCanPlatform::sigNewRoll, this, &QtCanPlatform::on_setInToRollData);
    connect(this, &QtCanPlatform::sigNewRollMult, this, &QtCanPlatform::on_setInToRollDataMult);
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
    //bottom->addWidget(tableRecView);
    bottom->addWidget(tabRecBox);
    bottom->addWidget(mt);
    bottom->addWidget(tabRollBox);
    /*bottom->addWidget(tableRollTitle);
    bottom->addWidget(tableRollData);*/
    //bottom->setHandleWidth(2);
    QSplitter* bootomright = new QSplitter(Qt::Vertical, mainBottom);
    dCtrl = new QDeviceCtrl();
    connect(dCtrl, &QDeviceCtrl::sigWorkRun, this, &QtCanPlatform::on_autoWork);
    connect(dCtrl, &QDeviceCtrl::sigCanChanged, this, &QtCanPlatform::on_pbRefreshDevice_clicked);

    QHBoxLayout* clay = new QHBoxLayout(this);
    clay->addWidget(pbClearText);
    clay->addSpacerItem(new QSpacerItem(20, 10));
    QWidget* ctx = new QWidget(this);
    ctx->setLayout(clay);
    pbClearText->setText(tr("清除日志"));
    pbClearText->setFixedWidth(80);
    bootomright->addWidget(dCtrl);
    bootomright->addWidget(ctx);
    bootomright->addWidget(textBrowser);
    bootomright->setStretchFactor(0, 2);
    bootomright->setStretchFactor(1, 1);
    bootomright->setStretchFactor(2, 6);
    
    mainBottom->setStretchFactor(0, 8);
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
/*
* ================“发送报文”填充到Tablewidget去============
*/
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
    cbBitRate->setCurrentIndex(pTemp.bundRate);
    cycle->setText(QString::number(pTemp.circle));
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
            
            //发送操作的下拉框
            QComboBox* cb = new QComboBox();
            for (int k = 0; k < sendCanData.at(i).pItem.at(j).stl_itemProperty.size();++k)
            {
                QString name = sendCanData.at(i).pItem.at(j).stl_itemProperty.at(k).toWord;
                QColor bc = QColor(sendCanData.at(i).pItem.at(j).stl_itemProperty.at(k).r, sendCanData.at(i).pItem.at(j).stl_itemProperty.at(k).g, sendCanData.at(i).pItem.at(j).stl_itemProperty.at(k).b);
                cb->addItem(name);
                //设置Item的数据，使用Qt::BackgroundColorRole声明
                cb->setItemData(cb->count() - 1, bc, Qt::BackgroundColorRole);
            }
            int sendValue = 0;
            if (cb->count() > 0)
            {
                int k = cb->currentIndex();
                QColor bc = QColor(sendCanData.at(i).pItem.at(j).stl_itemProperty.at(k).r, sendCanData.at(i).pItem.at(j).stl_itemProperty.at(k).g, sendCanData.at(i).pItem.at(j).stl_itemProperty.at(k).b);
                sendValue = sendCanData.at(i).pItem.at(j).stl_itemProperty.at(k).value.toInt();
                QString backcolor = "background-color:#"+ QString("%1").arg(bc.red(), 2, 16, QLatin1Char('0'))+
                    QString("%1").arg(bc.green(), 2, 16, QLatin1Char('0'))+
                    QString("%1").arg(bc.blue(), 2, 16, QLatin1Char('0'));
                cb->setStyleSheet(backcolor);
            }
            else
            {
                cb->setStyleSheet("background-color:#CCCCCC");
            }
            connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(on_cbSelectSendItemChanged(int)));
            
           
            tableView->setCellWidget(cr, 1, cb);
            //QString mt = "0x"+QString("%1").arg(sendCanData.at(i).CanId, QString::number(sendCanData.at(i).CanId).length(), 16).toUpper().trimmed();
            QString mt = sendCanData.at(i).strCanId;
            QTableWidgetItem* it1 = new QTableWidgetItem(QString::number(sendValue));
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

    for (int ch = 0; ch < 4; ch++)
    {

   
    if (!tableArray[ch])
        return false;
    int rcount = tableArray[ch]->rowCount();
    for (int m = 0; m < rcount; m++)
        tableArray[ch]->removeRow(rcount - m - 1);
    
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
            if(ch==0)
                recCanData.push_back(pTemp.cItem.at(i));
            // break;
        }
    }
    if (recCanData.size() <= 0)
    {
        QMessageBox::warning(this, tr("warning"), tr("该型号没有接收信号，请添加再操作"));
        return false;
    }
    if (ch == 0)
    {
        excelTitle = "序号,北京时间,";
        rollTitle.append(tr("序号"));
    }
   
    for (int i = 0; i < recCanData.size(); i++)
    {
        int num = recCanData.at(i).pItem.size();
        int cr = tableArray[ch]->rowCount();
        //每加一行就要设置到表格去
        tableArray[ch]->setRowCount(cr+1);
        int idex = 0;
        for (int j = 0; j <num; j++, idex++)
        {
            if (idex > 9)
            {
                idex = 0;
                cr = tableArray[ch]->rowCount();
                tableArray[ch]->setRowCount(cr + 2);
                cr += 1;
            }
            QTableWidgetItem* item = new QTableWidgetItem(recCanData.at(i).pItem.at(j).bitName);
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            item->font().setBold(true);
            //item->backgroundColor().setRgb(10, 10, 240);
            tableArray[ch]->setItem(cr, idex, item);
            if (ch == 0)
            {
                //保存Excel的表头
                excelTitle += recCanData.at(i).pItem.at(j).bitName + ",";

                if (recCanData.at(i).pItem.at(j).isRoll)
                {
                    rollTitle.append(recCanData.at(i).pItem.at(j).bitName);
                }
            }
            
        }
        cr = tableArray[ch]->rowCount();
        tableArray[ch]->setRowCount(cr + 1);
        //tableRecView->insertRow(2);
    }
    if (ch == 0)
    {
        excelTitle.remove(excelTitle.size() - 1, 1);
        tableRollTitle->setColumnCount(rollTitle.size());
        tableRollTitle->clear();
        tableRollTitle->setHorizontalHeaderLabels(rollTitle);
       
        tableRollTitle->horizontalHeader()->setStyleSheet(rollTitleStyle);
    }
    tableRollTitleArray[ch]->setColumnCount(rollTitle.size());
    tableRollTitleArray[ch]->clear();
    tableRollTitleArray[ch]->setHorizontalHeaderLabels(rollTitle);
    tableRollTitleArray[ch]->horizontalHeader()->setStyleSheet(rollTitleStyle);
    //QHeaderView::section {background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #0078d7, stop: 0.5 #0078d7,stop: 0.6 #0078d7, stop:1 #0078d7);color: white;}
    for (int i = 0; i < rollTitle.size(); i++)
    {
        tableRollTitle->setColumnWidth(i, 100);
        tableRollTitleArray[ch]->setColumnWidth(i, 100);
        
        
        //tableRollData->setColumnWidth(i, 40);
    }
    int col = tableArray[ch]->columnCount();
    int row = tableArray[ch]->rowCount();
    for (int h = 0; h < row; h++)
    {
        for (int g = 0; g < col; g++)
        {
            QTableWidgetItem* mItem = new QTableWidgetItem();
            mItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            QFont ff;
            ff.setBold(true);
            mItem->setFont(ff);
            tableArray[ch]->setItem(h, g, mItem);
            if (h % 2 == 0)
            {
                tableArray[ch]->item(h, g)->setBackgroundColor(recBackgroudColor);
                
            }
        }
    }
    //忘记了这里为什么要再初始一次表格了，唉
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
            tableArray[ch]->setItem(cr, idex, item);

        }
        cr += 2;
    }
   }
    return true;
}

void QtCanPlatform::sendData()
{
    uchar s_Data[8];
    getSendDataFromTable();
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
        if (cbCanType->currentIndex() == 0)
        {
            if(cbIsMutil->currentIndex()==0)
                pcan->SendFrame(fream_id, s_Data);
            else
            {
                for(int i=0;i<4;i++)
                    if(pcanArr[i]->IsOpen())
                       pcanArr[i]->SendFrame(fream_id, s_Data);
            }
            
        }
            
        else if (cbCanType->currentIndex() == 1)
        {
            kcan->canSendAll(fream_id, s_Data);
        }
        else if (cbCanType->currentIndex() == 2)
        {
            canayst->sendData(fream_id, s_Data);
        }
        //_sleep(20);
    }

}
void QtCanPlatform::getSendDataFromTable()
{
    int rows = tableView->rowCount();
    
    for (int i = 0; i < rows; i++)
    {
        unsigned int cbInId = tableView->item(i, 4)->text().toUInt(NULL, 16);
        for (int k = 0; k < sendCanData.size(); k++)
        {
            if (sendCanData.at(k).strCanId.toUInt(NULL, 16) != cbInId)
                continue;
            int cnum = 0;
            //这步很关键，算出前面的型号有多少个item
            for (int m = 0; m < k; m++)
                cnum += sendCanData.at(m).pItem.size();
            //当前型号的当前下标：当前行-前面的型号的item个数（cbRow - cnum）
            sendCanData.at(k).pItem.at(i - cnum).send = tableView->item(i, 2)->text().toInt();


        }
    }
    
}
bool QtCanPlatform::intelProtocol(canIdData& cdata,uchar data[], unsigned int& fream_id)
{
    if (cdata.pItem.size() <= 0)
        return false;
    fream_id = cdata.strCanId.toUInt(NULL, 16);
    protoItem crcTemp;
    bool crc = false;
    for (int i = 0; i < cdata.pItem.size() && i < 8; i++)
    {
        const protoItem &itemp = cdata.pItem.at(i);
        int startbyte = itemp.startByte;
        int startbit = itemp.startBit;
        int lengght = itemp.bitLeng;
        int senddd = itemp.send * itemp.precision+itemp.offset;
        if (itemp.dataFrom == "CRC")
        {
            crcTemp = cdata.pItem.at(i);
            crc = true;
        }
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
    //目前就只有博士的有CRC
    if (!crc)
        return true;
    data[crcTemp.startByte] = crc_high_first(data, 7);
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
        int senddd = itemp.send * itemp.precision + itemp.offset;
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
   /* if (isTrace)
    {
        QString strId = "0x" + QString("%1").arg(fream_id, 8, 16, QLatin1Char('0')).toUpper();
        QLOG_INFO() << "Rx:" << strId << "  " << hex;
    }*/

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
            bool octHex = recCanData.at(i).pItem.at(m).octhex;
            parseData pd;
            float temp=0;
            QString datafrom = recCanData.at(i).pItem.at(m).dataFrom;
           
            
            //判断是否跨字节，起止位模8，得出是当前字节的起止位，再加个长度
            if (datafrom.contains("*") || datafrom.contains("/"))
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
                    if (octHex)
                    {
                        int nn = binaryStr[startByte].mid(8 - (startLenght + (startBit % 8)), startLenght).toInt(NULL, 2);
                        QString ss = QString("%1").arg(nn, 0, 16, QLatin1Char('0'));
                        temp = ss .toInt() * precision + offset;
                    }
                    else
                    {
                        temp = binaryStr[startByte].mid(8 - (startLenght + (startBit % 8)), startLenght).toInt(NULL, 2) * precision + offset;
                    }
                    
                }
                else if (len <= 16)
                {
                    temp = (binaryStr[startByte + 1].mid(8 - (startLenght - (8 - startBit % 8)), startLenght - (8 - startBit % 8)) + binaryStr[startByte].mid(0, 8 - (startBit % 8))).toInt(NULL, 2) * precision + offset;
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
                //其它数据是否已经解析完成？
                //如果解析完成，就是生成本数据了
                int max = iB->f1 > iB->f2 ? iB->f1 : iB->f2;
                if (m - 1 > max - 1)
                {
                    //本数据是由其它数据生成，这里只做了由其它数据相乘
                    parseArr.at(iB->index).value = parseArr.at(iB->f1 - 1).value * parseArr.at(iB->f2 - 1).value;
                    parseArr.at(iB->index).toWord = QString::number(parseArr.at(iB->index).value);
                    //更新RollShowData里面的数据，因为是有序的，之前push进去的数据为空的
                    if (RollShowData.size() - 1 >= iB->showCount)
                    {
                        RollShowData.at(iB->showCount).value = parseArr.at(iB->index).value;
                    }
                    ddFF.erase(iB);
                    break;
                }
                iB++;
            }
            int stdddd = 0;
            for (int i = 0; i < ss.size(); i++)
            {
                if (ss.at(i).isStand)
                {
                    stdddd = ss.at(i).value.toInt();
                }
                if (ss.at(i).value.toInt() == temp)
                {
                    pd.color.r = ss.at(i).r;
                    pd.color.g = ss.at(i).g;
                    pd.color.b = ss.at(i).b;
                    pd.toWord = ss.at(i).toWord;
                    //break;
                }

            }
            
            //这里保存有错误的，stdddd是正确的标志，如果返回的数据跟正确的不一样，就是有错误
            if (ss.size() > 0 && stdddd != temp && isRecordError)
            {
                
                Error[0].insert(pd.toWord);

            } 
            if (datafrom == "-2")
            {
                if (ss.size() > 0 && stdddd != temp)
                {
                    multErr.insert(pd.toWord);
                }
                else
                    multErr.clear();
                pd.toWord.clear();
                for (auto mk : multErr)
                      pd.toWord += mk + "\n";
                pd.toWord.remove(pd.toWord.size() - 1, 1);
            }
            parseArr.push_back(pd);
        }

        for (auto& x : parseArr)
        {
            
            if (x.name == "功率W" || x.name == "功率kW"|| x.name == "功率")
            {
                realPower[0] = x.value;
            }
            else if (x.name == "电压V")
            {
                realVolt[0] = x.value;
            }
            else if (x.name == "出口温度°C")
            {
                realWTemp[0] = x.value;
            }
            else if (x.name == "高压异常故障")
            {
                realHVErr[0] = x.toWord;
            }
            else if (x.name == "过温保护" || x.name == "过温故障")
            {
                realOTPro[0] = x.toWord;
            }
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
    if (!tableArray[0])
        return;
  /*  int rcount = tableRecView->rowCount();
    for (int m = 0; m < rcount; m++)
        tableRecView->removeRow(rcount - m - 1);*/

    //=================10.10===============
    QString dTime = QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss-zzz");
    QDateTime dd = QDateTime::currentDateTime();
    uint curCount = strSaveList.size() + 1;
    QString dTemp = QString::number(curCount) + "," + dTime + ",";

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
            if (idex > 9)   //一行最多放10个数据
            {
                //满10个，从头开始
                idex = 0;
                //下一行的下一行，也就是隔一行，要加2；
                cr += 2;
            } 
            tableArray[0]->resizeRowToContents(cr + 1);
            QString tnamp = iBegin->second.at(j).name;
            QString toword = iBegin->second.at(j).toWord;
            try
            {
                tableArray[0]->setItem(cr, idex, new QTableWidgetItem(tnamp));
                QFont ff;
                ff.setBold(true);
                QTableWidgetItem* b = tableArray[0]->item(cr, idex);
                if (!b)
                    continue;
                b->setFont(ff);
                b = tableArray[0]->item(cr, idex);
                if (!b) 
                    continue;
                b->setTextAlignment(Qt::AlignCenter);
                b = tableArray[0]->item(cr, idex);
                if (!b)
                    continue;
                b->setBackgroundColor(recBackgroudColor);
                b = tableArray[0]->item(cr, idex);
                if (!b)
                    continue;
                b->setForeground(QBrush(recFontColor));
                tableArray[0]->setItem(cr + 1, idex, new QTableWidgetItem(toword));
                b = tableArray[0]->item(cr + 1, idex);
                if (!b)
                    continue;
                b->setBackgroundColor(QColor(iBegin->second.at(j).color.r, iBegin->second.at(j).color.g, iBegin->second.at(j).color.b));
                b = tableArray[0]->item(cr + 1, idex);
                if (!b)
                    continue;
                b->setTextAlignment(Qt::AlignCenter);
            }
            catch (const std::exception&e)
            {
                QLOG_INFO() << "Error:" << e.what();
            }
           
            
            dTemp += toword + ",";
        }
        cr += 2;
        iBegin++;

    }
    dTemp.remove(dTemp.size() - 1, 1);
    
    uint nn = (dd.toMSecsSinceEpoch() - lastTime.toMSecsSinceEpoch());
    lastTime = dd;
    //QLOG_INFO() << "nn:" << nn;
    if (nn > 50)
        strSaveList.append(dTemp); //放到一个list存着
    if (isRoll)
        emit sigNewRollMult(0);
    if (strSaveList.size() >= saveListNum)
    {
        saveCanData();
    }
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
            bool octHex = recCanData.at(i).pItem.at(m).octhex;
            parseData pd;
            float temp=0;
            //判断是否跨字节，起止位模8，得出是当前字节的起止位，再加个长度
            //if (datafrom != "-1")
            if(datafrom.contains("*")|| datafrom.contains("/"))
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

                    if (octHex)
                    {
                        int nn = binaryStr[startByte].mid(8 - (startLenght + (startBit % 8)), startLenght).toInt(NULL, 2);
                        QString ss = QString("%1").arg(nn, 0, 16, QLatin1Char('0'));
                        temp = ss.toInt() * precision + offset;
                    }
                    else
                    {
                        temp = binaryStr[startByte].mid(startBit % 8, startLenght).toInt(NULL, 2) * precision + offset;
                    }
                    
                }
                else if (len <= 16)
                {

                    if (octHex)
                    {
                        int nn = (binaryStr[startByte].mid(0, 8 - (startBit % 8)) + binaryStr[startByte + 1].mid(startBit % 8, startLenght - (8 - startBit % 8))).toInt(NULL, 2);
                        QString ss = QString("%1").arg(nn, 0, 16, QLatin1Char('0'));
                        temp = ss.toInt() * precision + offset;
                    }
                    else
                    {
                        temp = (binaryStr[startByte].mid(0, 8 - (startBit % 8)) + binaryStr[startByte + 1].mid(startBit % 8, startLenght - (8 - startBit % 8))).toInt(NULL, 2) * precision + offset;
                    }
                   
                }
                {
                    //跨三个字节的，应该没有
                }
            }
            
            pd.name = recCanData.at(i).pItem.at(m).bitName;
            pd.value = temp;
            pd.toWord = QString::number(temp,'g',2);
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
    if (!tableArray[0])
        return;
    int rcount = tableArray[0]->rowCount();
    for (int m = 0; m < rcount; m++)
        tableArray[0]->removeRow(rcount - m - 1);
    //=================10.10===============
    //=================10.10===============
    QString dTime = QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss-zzz");
    QDateTime dd = QDateTime::currentDateTime();
    uint curCount = strSaveList.size() + 1;
    QString dTemp = QString::number(curCount) + "," + dTime + ",";

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
            if (idex > 9)   //一行最多放10个数据
            {
                //满10个，从头开始
                idex = 0;
                //下一行的下一行，也就是隔一行，要加2；
                cr += 2;
            }
            tableArray[0]->setRowCount(cr+2);
            QString tnamp = iBegin->second.at(j).name;
            QString toword = iBegin->second.at(j).toWord;
            try
            {
                tableArray[0]->setItem(cr, idex, new QTableWidgetItem(tnamp));
                QFont ff;
                ff.setBold(true);
                QTableWidgetItem* b = tableArray[0]->item(cr, idex);
                if (!b)
                    continue;
                b->setFont(ff);
                b = tableArray[0]->item(cr, idex);
                if (!b)
                    continue;
                b->setTextAlignment(Qt::AlignCenter);
                b = tableArray[0]->item(cr, idex);
                if (!b)
                    continue;
                b->setBackgroundColor(recBackgroudColor);
                b = tableArray[0]->item(cr, idex);
                if (!b)
                    continue;
                b->setForeground(QBrush(recFontColor));
                tableArray[0]->setItem(cr + 1, idex, new QTableWidgetItem(toword));
                b = tableArray[0]->item(cr + 1, idex);
                if (!b)
                    continue;
                b->setBackgroundColor(QColor(iBegin->second.at(j).color.r, iBegin->second.at(j).color.g, iBegin->second.at(j).color.b));
                b = tableArray[0]->item(cr + 1, idex);
                if (!b)
                    continue;
                b->setTextAlignment(Qt::AlignCenter);
            }
            catch (const std::exception& e)
            {
                QLOG_INFO() << "Error:" << e.what();
            }


            dTemp += toword + ",";
        }
        cr += 2;
        iBegin++;

    }
    dTemp.remove(dTemp.size() - 1, 1);

    uint nn = (dd.toMSecsSinceEpoch() - lastTime.toMSecsSinceEpoch());
    lastTime = dd;
    //QLOG_INFO() << "nn:" << nn;
    if (nn > 50)
    {
        strSaveList.append(dTemp); //放到一个list存着
        if (isRoll)
            emit sigNewRollMult(0);
    }
       
    if (strSaveList.size() >= saveListNum)
    {
        saveCanData();
    }
}
void QtCanPlatform::recAnalyseIntel(int ch,unsigned int fream_id, QByteArray data)
{

    QStringList binaryStr;
    QString hex;
    for (int k = 0; k < data.size(); ++k)
    {
        QString str = QString("%1").arg((uint8_t)data[k], 8, 2, QLatin1Char('0'));
        binaryStr.append(str);

        hex += QString("%1").arg(str.toInt(NULL, 2), 2, 16, QLatin1Char('0')).toUpper() + " ";
    }
    /*if (isTrace)
    {
        QString strId = "0x" + QString("%1").arg(fream_id, 8, 16, QLatin1Char('0')).toUpper();
        QLOG_INFO() << "Rx:" << strId << "  " << hex;
    }*/

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
            bool octHex = recCanData.at(i).pItem.at(m).octhex;
            parseData pd;
            float temp = 0;
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

                    if (octHex)
                    {
                        int nn = binaryStr[startByte].mid(8 - (startLenght + (startBit % 8)), startLenght).toInt(NULL, 2);
                        QString ss  = QString("%1").arg(nn, 0, 16, QLatin1Char('0'));
                        temp = ss.toInt() * precision + offset;
                    }
                    else
                    {
                        temp = binaryStr[startByte].mid(8 - (startLenght + (startBit % 8)), startLenght).toInt(NULL, 2) * precision + offset;
                    }
                    
                
                }
                else if (len <= 16)
                {
                    if (octHex)
                    {
                        int nn = (binaryStr[startByte + 1].mid(8 - (startLenght - (8 - startBit % 8)), startLenght - (8 - startBit % 8)) + binaryStr[startByte].mid(0, 8 - (startBit % 8))).toInt(NULL, 2);
                        QString ss = QString("%1").arg(nn, 0, 16, QLatin1Char('0'));
                        temp = ss.toInt() * precision + offset;
                    }
                    else
                    {
                        temp = (binaryStr[startByte + 1].mid(8 - (startLenght - (8 - startBit % 8)), startLenght - (8 - startBit % 8)) + binaryStr[startByte].mid(0, 8 - (startBit % 8))).toInt(NULL, 2) * precision + offset;;
                    }
                    
                }
                {
                    //跨三个字节的，应该没有
                }
            }
            
            pd.name = recCanData.at(i).pItem.at(m).bitName;
            pd.value = temp;
            pd.toWord = QString::number(temp,'g',2);
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
                //数据由其它字段相乘得来，如功率 = byte[0]*byte[1]
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
            int stdddd = 0;
            for (int i = 0; i < ss.size(); i++)
            {
                if (ss.at(i).isStand)
                {
                    stdddd = ss.at(i).value.toInt();
                }
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

            //这里保存有错误的，stdddd是正确的标志，如果返回的数据跟正确的不一样，就是有错误
            if (ss.size() > 0 && stdddd != temp && isRecordError)
            {
                Error[ch].insert(pd.toWord);
            }
        }
        for (auto& x : parseArr)
        {

            if (x.name == "功率W" || x.name == "功率kW" || x.name == "功率")
            {
                realPower[ch] = x.value;
            }
            else if (x.name == "电压V")
            {
                realVolt[ch] = x.value;
            }
            else if (x.name == "出口温度°C")
            {
                realWTemp[ch] = x.value;
            }
            else if (x.name == "高压异常故障")
            {
                realHVErr[ch] = x.toWord;
            }
            else if (x.name == "过温保护" || x.name == "过温故障")
            {
                realOTPro[ch] = x.toWord;
            }
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
    if (!tableArray[ch])
        return;
    /*  int rcount = tableRecView->rowCount();
      for (int m = 0; m < rcount; m++)
          tableRecView->removeRow(rcount - m - 1);*/

          //=================10.10===============
    QString dTime = QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss-zzz");
    QDateTime dd = QDateTime::currentDateTime();
    uint curCount = tableRollDataArray[ch]->rowCount() + 1;

    QString dTemp = QString::number(curCount) + "," + dTime + ",";

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
            if (idex > 9)   //一行最多放10个数据
            {
                //满10个，从头开始
                idex = 0;
                //下一行的下一行，也就是隔一行，要加2；
                cr += 2;
            }

            QString tnamp = iBegin->second.at(j).name;
            QString toword = iBegin->second.at(j).toWord;
            tableArray[ch]->setItem(cr, idex, new QTableWidgetItem(tnamp));
            QFont ff;
            ff.setBold(true);
            tableArray[ch]->item(cr, idex)->setFont(ff);
            tableArray[ch]->item(cr, idex)->setTextAlignment(Qt::AlignCenter);
            tableArray[ch]->item(cr, idex)->setBackgroundColor(recBackgroudColor);
            tableArray[ch]->item(cr, idex)->setForeground(QBrush(recFontColor));
            tableArray[ch]->setItem(cr + 1, idex, new QTableWidgetItem(toword));
            tableArray[ch]->item(cr + 1, idex)->setBackgroundColor(QColor(iBegin->second.at(j).color.r, iBegin->second.at(j).color.g, iBegin->second.at(j).color.b));
            tableArray[ch]->item(cr + 1, idex)->setTextAlignment(Qt::AlignCenter);
            dTemp += toword + ",";
        }
        cr += 2;
        iBegin++;

    }
    dTemp.remove(dTemp.size() - 1, 1);

    uint nn = (dd.toMSecsSinceEpoch() - lastTime.toMSecsSinceEpoch());
    lastTime = dd;
    

    //if (nn > 50)
    {
        if (YB::isExistKey(multReceData, ch))
        {
            if (multReceData[ch].size() > 1)
            {
                QStringList ss = multReceData[ch].at(multReceData[ch].size() - 1).split(",");
                QStringList ss2 = dTemp.split(",");
                if (ss.at(0) != ss2.at(0))
                {
                    multReceData[ch].append(dTemp);
                }
            }
            else
            {
                multReceData[ch].append(dTemp);
            }
           
        }
        else
        {
            QStringList ss;
            ss.append(dTemp);
            multReceData.insert(std::make_pair(ch, ss));
        }
    }
   

    //if (nn > 50)
    //    strSaveList.append(dTemp); //放到一个list存着
    if (isRoll)
        emit sigNewRollMult(ch);
    if (tableRollDataArray[ch]->rowCount() >= saveListNum)
    {
        saveCanDataMult();
    }
}
void QtCanPlatform::recAnalyseMoto(int ch,unsigned int fream_id, QByteArray data)
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
            bool octHex = recCanData.at(i).pItem.at(m).octhex;
            QString datafrom = recCanData.at(i).pItem.at(m).dataFrom;
            parseData pd;
            float temp = 0;
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

                    if (octHex)
                    {
                        int nn = binaryStr[startByte].mid(startBit % 8, startLenght).toInt(NULL, 2);
                        QString ss = QString("%1").arg(nn, 0, 16, QLatin1Char('0'));
                        temp = ss.toInt() * precision + offset;
                        //temp = binaryStr[startByte].mid(startBit % 8, startLenght).toInt(NULL, 2) * precision + offset;
                    }
                    else
                    {
                        temp = binaryStr[startByte].mid(startBit % 8, startLenght).toInt(NULL, 2) * precision + offset;
                    }
                    
                }
                else if (len <= 16)
                {
                    if (octHex)
                    {
                        int nn = (binaryStr[startByte].mid(0, 8 - (startBit % 8)) + binaryStr[startByte + 1].mid(startBit % 8, startLenght - (8 - startBit % 8))).toInt(NULL, 2);
                        QString ss = QString("%1").arg(nn, 0, 16, QLatin1Char('0'));
                        temp = ss.toInt() * precision + offset;
                    }
                    else
                    {
                        temp = (binaryStr[startByte].mid(0, 8 - (startBit % 8)) + binaryStr[startByte + 1].mid(startBit % 8, startLenght - (8 - startBit % 8))).toInt(NULL, 2) * precision + offset;
                    }
                   
                }
                {
                    //跨三个字节的，应该没有
                }
            }

            pd.name = recCanData.at(i).pItem.at(m).bitName;
            pd.value = temp;
            pd.toWord = QString::number(temp,'g',2);
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
            if (pd.name == "功率" || pd.name == "功率kW")
            {
                if (ch >= 0 && ch < 4)
                    realPower[ch] = pd.value;
            }
            if (pd.name == "电压")
            {
                if (ch >= 0 && ch < 4)
                    realVolt[ch] = pd.value;
            }
            if (pd.name == "出口温度°C")
            {
                if (ch >= 0 && ch < 4)
                    realWTemp[ch] = pd.value;
            }
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
    if (!tableArray[ch])
        return;
    int rcount = tableArray[ch]->rowCount();
    for (int m = 0; m < rcount; m++)
        tableArray[ch]->removeRow(rcount - m - 1);
    //=================10.10===============
    QString dTime = QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-ss-zzz");
    QDateTime dd = QDateTime::currentDateTime();
    uint curCount = tableRollDataArray[ch]->rowCount() + 1;
    QString dTemp = QString::number(curCount) + "," + dTime + ",";

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
            tableArray[ch]->setItem(cr, idex, new QTableWidgetItem(tnamp));
            QFont ff;
            ff.setBold(true);
            tableArray[ch]->item(cr, idex)->setFont(ff);
            tableArray[ch]->item(cr, idex)->setTextAlignment(Qt::AlignCenter);
            tableArray[ch]->item(cr, idex)->setBackgroundColor(recBackgroudColor);
            tableArray[ch]->item(cr, idex)->setForeground(QBrush(recFontColor));
            tableArray[ch]->setItem(cr + 1, idex, new QTableWidgetItem(toword));
            tableArray[ch]->item(cr + 1, idex)->setBackgroundColor(QColor(iBegin->second.at(j).color.r, iBegin->second.at(j).color.g, iBegin->second.at(j).color.b));
            tableArray[ch]->item(cr + 1, idex)->setTextAlignment(Qt::AlignCenter);
            dTemp += toword + ",";
        }
        cr += 2;
        iBegin++;

    }
    dTemp.remove(dTemp.size() - 1, 1);
    uint nn = (dd.toMSecsSinceEpoch() - lastTime.toMSecsSinceEpoch());
    lastTime = dd;
    //if (nn > 50)
    {
        if (YB::isExistKey(multReceData, ch))
        {
            if (multReceData[ch].size() > 1)
            {
                QStringList ss = multReceData[ch].at(multReceData[ch].size() - 1).split(",");
                QStringList ss2 = dTemp.split(",");
                if (ss.at(0) != ss2.at(0))
                {
                    multReceData[ch].append(dTemp);
                }
            }
            else
            {
                multReceData[ch].append(dTemp);
            }

        }
        else
        {
            QStringList ss;
            ss.append(dTemp);
            multReceData.insert(std::make_pair(ch, ss));
        }
    }

    if (isRoll)
        emit sigNewRollMult(ch);
    if (tableRollDataArray[ch]->rowCount() >= saveListNum)
    {
        saveCanDataMult();
    }
}
unsigned char QtCanPlatform::crc_high_first(uchar data[], unsigned char len)
{
    //    unsigned char i;
    unsigned char crc = 0xFF; /* 计算的初始crc值 */
    uchar* ptr = &data[1];
    //    *ptr++;

    while (len--)
    {
        crc ^= *ptr++;  /* 每次先与需要计算的数据异或,计算完指向下一数据 */
        for (int i = 8; i > 0; --i)   /* 下面这段计算过程与计算一个字节crc一样 */
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x2F;
            else
                crc = (crc << 1);

        }
    }
    crc ^= 0xFF;
    qDebug() << "crc:" << crc;
    return (crc);
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
void QtCanPlatform::on_cbCanType_currentIndexChanged(int index)
{
    cbPcan->clear();
    on_pbRefreshDevice_clicked();
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
        //用来检测通信状态的定时器
        if (!lostQTimer)
        {
            lostQTimer = new QTimer();
            connect(lostQTimer, SIGNAL(timeout()), this, SLOT(on_recTimeout()));
        }
        for (int i = 0; i < 4; i++)
        {
            lostQTimerArr[i]->start(lostTimeOut);
        }
        lostQTimer->start(lostTimeOut);
        pbSend->setStyleSheet("background-color:#00FF00");

        int index = cbSelectModel->currentIndex();
        qGboleData* qGb = qGboleData::getInstance();
        if (!qGb)return;
        if (index > qGb->pGboleData.size() - 1)
            return;
        if (0 == qGb->pGboleData.at(index).agreement)
        {
            pcan->setProperty("isExtend", true);
        }
        else
        {
            pcan->setProperty("isExtend", false);
        }

    }
    else
    {
        sendTimer->stop();
        cycle->setEnabled(true);
        cbBitRate->setEnabled(true);
        reFresh->setEnabled(true);
        if (lostQTimer)
        {
            lostQTimer->stop();
        }
        communicaLabel->setText(tr("待机中…"));
        communicaLabel->setStyleSheet("background-color:#FAFA00");
        pbSend->setStyleSheet("");
    }
}
void QtCanPlatform::on_pbRefreshDevice_clicked()
{
    cbPcan->clear();
    if (cbCanType->currentIndex() == 0)
    {
        pcan->DetectDevice();
        pcanArr[0]->DetectDevice();
        pcanArr[1]->DetectDevice();
        pcanArr[2]->DetectDevice();
        QStringList canStr = pcanArr[3]->DetectDevice();
        for (int i = 0; i < canStr.size(); ++i)
        {
            cbPcan->addItem(canStr.at(i));
        }
    }
    else if (cbCanType->currentIndex() == 1)
    {
        int n = kcan->getCanCount();
        for (int m = 0; m < n; m++)
        {
            cbPcan->addItem("ch" + QString::number(m+1));
        }
    }
    else if (cbCanType->currentIndex() == 2)
    {
        QStringList n = canayst->DetectDevice();
        for (int m = 0; m < n.size(); m++)
        {
            cbPcan->addItem("can" + n.at(m));
        }
        //canayst
    }
    
}
void QtCanPlatform::on_pbOpenPcan_clicked()
{
    if (cbCanType->currentIndex() == 0)
    {
        if (pcanIsOpen)
        {
            pcan->CloseCan();
            for (int i = 0; i < 4 && i < cbPcan->count(); i++)
                pcanArr[i]->CloseCan();
            pbOpen->setText(tr("打开设备"));
            cbBitRate->setEnabled(true);
            cbPcan->setEnabled(true);
            pbSend->setChecked(false);
            pbSend->setEnabled(false);
            pbSend->setStyleSheet("");
            pcanIsOpen = false;
            sendTimer->stop();
            reFresh->setEnabled(true);
            communicaLabel->setText(tr("待机中..."));
        }
        else
        {
            reFresh->setEnabled(true);
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
            bool b = false;;
            if (1 == cbIsMutil->currentIndex())
            {
                for (int i = 0; i < 4 && i < cbPcan->count(); i++)
                       b |= pcanArr[i]->ConnectDevice(pcanArr[i]->m_interfaces.at(i).name(), bitRate);
            }
            else if (0 == cbIsMutil->currentIndex())
            {
                b = pcan->ConnectDevice(cbPcan->currentText(), bitRate);
               
            }
            tabRollBox->tabBar()->setTabIcon(0, QApplication::style()->standardIcon((QStyle::StandardPixmap)31));
            tabRollBox->tabBar()->setTabIcon(1, QApplication::style()->standardIcon((QStyle::StandardPixmap)31));
            tabRollBox->tabBar()->setTabIcon(2, QApplication::style()->standardIcon((QStyle::StandardPixmap)31));
            tabRollBox->tabBar()->setTabIcon(3, QApplication::style()->standardIcon((QStyle::StandardPixmap)31));
             // = pcan->ConnectDevice(cbPcan->currentIndex(), bitRate);
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
    else if (cbCanType->currentIndex() == 1)
    {
        if (pcanIsOpen)
        {
            kcan->closeCAN();
            pbOpen->setText(tr("打开设备"));
            cbBitRate->setEnabled(true);
            cbPcan->setEnabled(true);
            pbSend->setChecked(false);
            pbSend->setEnabled(false);
            pbSend->setStyleSheet("");
            pcanIsOpen = false;
            sendTimer->stop();
            reFresh->setEnabled(true);
            communicaLabel->setText(tr("待机中..."));
            communicaLabel->setStyleSheet("background-color:yellow");
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
            //bool b = pcan->ConnectDevice(cbPcan->currentIndex(), bitRate);
            QString err;
            ckHandle =  kcan->openCanAll(bitRate, err);
            if (!ckHandle)
            {
                QMessageBox::warning(NULL, tr("错误"), tr("打开KCAN失败,请检测设备是否被占用或者已经连接？"));
                return;
            }
            for (int k = 0; k < 4; k++)
            {
                if (ckHandle[k] >= 0)
                    QLOG_INFO() << "Open Ch" + QString::number(k) + "Success!";
                else
                    QLOG_INFO() << "Open Ch" + QString::number(k) + "failed!";
            }
            pbOpen->setText(tr("关闭设备"));
            cbBitRate->setEnabled(false);
            cbPcan->setEnabled(false);
            pbSend->setEnabled(true);
            pcanIsOpen = true;
        }
    }
    else if (cbCanType->currentIndex() == 2)
    {
        if (pcanIsOpen)
        {
            canayst->closeCAN();
            pbOpen->setText(tr("打开设备"));
            cbBitRate->setEnabled(true);
            cbPcan->setEnabled(true);
            pbSend->setChecked(false);
            pbSend->setEnabled(false);
            pbSend->setStyleSheet("");
            pcanIsOpen = false;
            sendTimer->stop();
            reFresh->setEnabled(true);
            communicaLabel->setText(tr("待机中..."));
            
        }
        else
        {
            /*if (cbPcan->count() <= 0)
                return;*/
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
            //bool b = pcan->ConnectDevice(cbPcan->currentIndex(), bitRate);
            bool b = canayst->openCANAll(bitRate);
            if (!b)
            {
                QMessageBox::warning(NULL, tr("错误"), tr("打开KCAN失败,请检测设备是否被占用或者已经连接？"));
                return;
            }
            pbOpen->setText(tr("关闭设备"));
            cbBitRate->setEnabled(false);
            cbPcan->setEnabled(false);
            pbSend->setEnabled(true);
            pcanIsOpen = true;
        }
    }
    if (dCtrl) {
        dCtrl->on_dCanRefresh_clicked();
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
    QString sendId = tableView->item(row, 4)->text().trimmed();
    QString senddt = tableView->item(row, 2)->text().trimmed();
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
                //disconnect(tableView, SIGNAL(cellChanged(int, int)), this, SLOT(on_tableClicked(int, int)));
                QLOG_INFO() << "sendData over row";
                continue;
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
    //这是专门处理博世的版本号
    if (fream_id == 0x18AD7F5A && pbGetVer->isChecked() && data.at(0) == 0x06)
    {
        int b_m = data.at(1);
        int b_s = data.at(2);
        int s_m = data.at(3);
        int s_s = data.at(4);
        QString str = tr("1# 主:") + QString::number(b_m) + "." + QString::number(b_s) + \
            tr(" 从:") + QString::number(s_m) + "." + QString::number(s_s);
        labelVer1.setText(str);
        return;

    }
    //没有按下发送就不要接收数据
    //才改一天，这新源动力又说这不行，，，，shit
    /*if (!pbSend->isChecked())
        return;*/
    communicaLabel->setText(tr("通信正常"));
    communicaLabel->setStyleSheet("background-color:green");
    lostQTimer->start(lostTimeOut);
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
void QtCanPlatform::on_ReceiveDataMulti(uint fream_id, QByteArray data)
{
    PCAN* _pcan = dynamic_cast<PCAN*>(sender());
    if (!_pcan)return;
    for (int i = 0; i < 4; i++)
        if (pcanArr[i] != nullptr && pcanArr[i] == _pcan)
        {
            on_ReceiveData(i, fream_id, data);
            tabRollBox->tabBar()->setTabIcon(i, QApplication::style()->standardIcon((QStyle::StandardPixmap)31));
            tabRecBox->tabBar()->setTabIcon(i, QApplication::style()->standardIcon((QStyle::StandardPixmap)31));
            lostQTimerArr[i]->start(3000);
        }
            
}
void QtCanPlatform::on_ReceiveData(const int ch, uint fream_id, QByteArray data)
{
    if (1 == cbCanType->currentIndex() || 2 == cbCanType->currentIndex())
    {
        tabRollBox->tabBar()->setTabIcon(ch, QApplication::style()->standardIcon((QStyle::StandardPixmap)31));
        tabRecBox->tabBar()->setTabIcon(ch, QApplication::style()->standardIcon((QStyle::StandardPixmap)31));
        lostQTimerArr[ch]->start(3000);
    }
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
        QString strId = "ch"+QString::number(ch) + "- 0x" + QString("%1").arg(fream_id, 8, 16, QLatin1Char('0')).toUpper();
        QLOG_INFO() << "Rx:" << strId << "  " << hex;
    }
    //这是专门处理博世的版本号
    if (fream_id == 0x18AD7F5A && pbGetVer->isChecked() && data.at(0) == 0x06)
    {
        int b_m = data.at(1);
        int b_s = data.at(2);
        int s_m = data.at(3);
        int s_s = data.at(4);
        QString str = QString::number(ch)+tr("# 主:") + QString::number(b_m) + "." + QString::number(b_s) + \
            tr(" 从:") + QString::number(s_m) + "." + QString::number(s_s);
        if(0==ch)
            labelVer1.setText(str);
        else if(1==ch)
            labelVer2.setText(str);
        else if (2 == ch)
            labelVer3.setText(str);
        else
            labelVer4.setText(str);
        return;
    }
    //没有按下发送就不要接收数据
    /*if (!pbSend->isChecked())
        return;*/
    communicaLabel->setText(tr("通信正常"));
    communicaLabel->setStyleSheet("background-color:green");
    lostQTimer->start(lostTimeOut);
    int index = cbSelectModel->currentIndex();

    qGboleData* qGb = qGboleData::getInstance();
    if (!qGb)return;
    if (index > qGb->pGboleData.size() - 1)
        return;
    if (0 == qGb->pGboleData.at(index).agreement)
    {
        recAnalyseIntel(ch,fream_id, data);
    }
    else
    {
        recAnalyseMoto(ch,fream_id, data);
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
        if (cbCol != 1)
            continue;
    
        //当前型号的当前下标：当前行-前面的型号的item个数（cbRow - cnum）
        sendCanData.at(k).pItem.at(cbRow - cnum).send = sendCanData.at(k).pItem.at(cbRow- cnum).stl_itemProperty.at(index).value.toInt();
        tableView->setItem(cbRow,2, new QTableWidgetItem(sendCanData.at(k).pItem.at(cbRow - cnum).stl_itemProperty.at(index).value));
        tableView->item(cbRow, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        QColor bc = QColor(sendCanData.at(k).pItem.at(cbRow - cnum).stl_itemProperty.at(index).r, sendCanData.at(k).pItem.at(cbRow - cnum).stl_itemProperty.at(index).g, sendCanData.at(k).pItem.at(cbRow- cnum).stl_itemProperty.at(index).b);
          
        QString backcolor = "background-color:#" + QString("%1").arg(bc.red(), 2, 16, QLatin1Char('0')) +
            QString("%1").arg(bc.green(), 2, 16, QLatin1Char('0')) +
            QString("%1").arg(bc.blue(), 2, 16, QLatin1Char('0'));
        cb->setStyleSheet(backcolor);
    
    }
}
/*
* @brief: 用来处理在“发送表格”前面的启用发送checkbox，勾选上了，则会发送，没勾选则不会发送
* @param : int check，若大于0，则选中，此项所有的ID组所有位都会发送，否则不会
* @return: void
*/
void QtCanPlatform::on_checkSendChanged(int check)
{
    QCheckBox* cb = dynamic_cast<QCheckBox*>(sender());
    if (!cb)
    {
        QLOG_INFO() << "当前的QComboBox sender 无效";
        return;
    }
    //获取CheckBox所在行
    QModelIndex cbIndex = tableView->indexAt(QPoint(cb->geometry().x(), cb->geometry().y()));
    int cbRow = cbIndex.row();
    int cbCol = cbIndex.column();
    unsigned int cbInId = tableView->item(cbRow, 4)->text().toUInt(NULL, 16);
    bool b = check > 0 ? true : false;
    //设置发送项的标志，如果此项为false，则在定时发送时忽略当前项
    for (int k = 0; k < sendCanData.size(); k++)
    {
        if (sendCanData.at(k).strCanId.toUInt(NULL, 16) != cbInId)
            continue;
        sendCanData.at(k).isSend = b;
        break;
    }
    int tbRowCount = tableView->rowCount();
    //设置整组的Checkbox
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
void QtCanPlatform::on_recTimeout()
{
    communicaLabel->setText(tr("通信超时"));
    communicaLabel->setStyleSheet("background-color:red");
}
void QtCanPlatform::on_recTimeoutMutil()
{
    //获取发送者的指针
    QTimer* t = dynamic_cast<QTimer*>(sender());
    if (!t)
        return;
    if (cbIsMutil->currentIndex() == 0 && cbCanType->currentIndex() < 1)
        return;
    for (int i = 0; i < 4; i++)
    {
        //判断是哪个定时器
        if (t->timerId() == lostQTimerArr[i]->timerId())
        {
            //设置tabBar的ICON
            tabRollBox->tabBar()->setTabIcon(i, QApplication::style()->standardIcon((QStyle::StandardPixmap)11));
            tabRecBox->tabBar()->setTabIcon(i, QApplication::style()->standardIcon((QStyle::StandardPixmap)11));
        }     
    }
}
/*
* @brief：把要滚动显示的数据添加到表格里面
* @param：无
* @return：void
*/
void QtCanPlatform::on_setInToRollDataMult(int ch)
{
    //设置列数
    tableRollDataArray[ch]->setColumnCount(rollTitle.size());
    //顺便设置列宽
    for (int i = 0; i < rollTitle.size(); i++)
    {
        tableRollDataArray[ch]->setColumnWidth(i, 100);
    }
    int row = tableRollDataArray[ch]->rowCount();
    //增加一行
    tableRollDataArray[ch]->setRowCount(row + 1);
   /* QString dTime = QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-ss-zzz");
    uint curCount = strSaveList.size()+1;
    QString dTemp= QString::number(curCount)+","+ dTime+",";*/
    //excelTitle = "序号,北京时间,";
    uint curCount = strSaveList.size()+1;
    //序号
    tableRollDataArray[ch]->setItem(row, 0, new QTableWidgetItem(QString::number(row)));
    QTableWidgetItem* b2 = tableRollDataArray[ch]->item(row, 0);
    if(b2)
        b2->setTextAlignment(Qt::AlignCenter);
    QStringList title;
    title.append("序号");
    for (int i = 0;i< RollShowData.size(); i++)
    {
        tableRollDataArray[ch]->setItem(row, i+1, new QTableWidgetItem(QString::number(RollShowData.at(i).value)));
        title.append(RollShowData.at(i).name);
        QTableWidgetItem* b = tableRollDataArray[ch]->item(row, i + 1);
        if (!b)
            continue;
        b->setTextAlignment(Qt::AlignCenter);
       // dTemp += QString::number(RollShowData.at(i).value) + ",";
        //这个是保存到excel的表头
        //excelTitle += RollShowData.at(i).name + ",";
    }
    tableRollTitleArray[ch]->setHorizontalHeaderLabels(title);
    //表格永远显示底部
    tableRollDataArray[ch]->scrollToBottom();
    //移除最后一个逗号
   // dTemp.remove(dTemp.size() - 1, 1);
   // excelTitle.remove(excelTitle.size() - 1, 1);
    //放到一个list存着
    //strSaveList.append(dTemp);
    //够数的就保存到excel
    /*if (strSaveList.size() >= saveListNum)
    {
        saveCanData();
    }*/
}
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
    /* QString dTime = QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-ss-zzz");
     uint curCount = strSaveList.size()+1;
     QString dTemp= QString::number(curCount)+","+ dTime+",";*/
     //excelTitle = "序号,北京时间,";
    uint curCount = strSaveList.size() + 1;
    //序号
    tableRollData->setItem(row, 0, new QTableWidgetItem(QString::number(row)));
    tableRollData->item(row, 0)->setTextAlignment(Qt::AlignCenter);
    for (int i = 0; i < RollShowData.size(); i++)
    {
        tableRollData->setItem(row, i + 1, new QTableWidgetItem(QString::number(RollShowData.at(i).value)));
        tableRollData->item(row, i + 1)->setTextAlignment(Qt::AlignCenter);
        // dTemp += QString::number(RollShowData.at(i).value) + ",";
         //这个是保存到excel的表头
         //excelTitle += RollShowData.at(i).name + ",";
    }
    //表格永远显示底部
    tableRollData->scrollToBottom();
    //移除最后一个逗号
   // dTemp.remove(dTemp.size() - 1, 1);
   // excelTitle.remove(excelTitle.size() - 1, 1);
    //放到一个list存着
    //strSaveList.append(dTemp);
    //够数的就保存到excel
    /*if (strSaveList.size() >= saveListNum)
    {
        saveCanData();
    }*/
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
    appPath += QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss-zzz") + ".xlsx";
    if (strSaveList.size() < 1)
        return;
    //tableRollData->clear();
   
    saveData->setTitle(excelTitle);
    saveData->SaveData(strSaveList, strSaveList.size(), appPath);
    on_pbClearCanData_clicked();
    //strSaveList.clear();
}
/*
* @brief：
*/
void QtCanPlatform::saveCanDataMult()
{
    QString appPath = QApplication::applicationDirPath() + "/PCAN-DATA/";
    QDir dd(appPath);
    if (!dd.exists())
    {
        dd.mkpath(appPath);
    }
    for (auto &x:multReceData)
    {
       QString savepath =  appPath + "通道"+QString::number(x.first+1)+"数据" + QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss-zzz") + ".xlsx";
        if (x.second.size() < 1)
            continue;
        saveDataArr[x.first]->setTitle(excelTitle);
        saveDataArr[x.first]->SaveData(x.second, x.second.size(), savepath);
        Sleep(60);
        x.second.clear();
    }
    on_pbClearCanData_clicked();
}
void QtCanPlatform::saveAutoTestRes(const QString & fileName,const QStringList& data)
{
    QString appPath = QApplication::applicationDirPath() + "/AUTO_TEST_RES/";
    QDir dd(appPath);
    if (!dd.exists())
    {
        dd.mkpath(appPath);
    }
    if (!fileName.isEmpty())
    {
        appPath += fileName + ".xlsx";
    }
    else
    {
        appPath += QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss-zzz") + ".xlsx";
    }
    DataSave d;
    d.setTitle(QString("测试项,结果"));
    d.SaveData(data, data.size(), appPath);
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
    if (cbCanType->currentIndex() == 0 && cbIsMutil->currentIndex() == 0)
        saveCanData();
    else if (cbCanType->currentIndex() == 1|| cbCanType->currentIndex() ==2|| cbIsMutil->currentIndex() == 1)
        saveCanDataMult();
}
void QtCanPlatform::on_pbClearCanData_clicked()
{
    multErr.clear();
    strSaveList.clear();
    multReceData.clear();
    recDataIntoTab();
    labelVer1.clear();
    labelVer2.clear();
    labelVer3.clear();
    labelVer4.clear();
    for (int k = 0; k < 4; k++)
    {
        int rcount = tableRollDataArray[k]->rowCount();
        for (int m = 0; m < rcount; m++)
            tableRollDataArray[k]->removeRow(rcount - m - 1);
    }
    
    
}
void QtCanPlatform::on_pbGetVer_clicked(bool isCheck)
{
    if (!t_GetVer)
        t_GetVer = new QTimer(this);
     QPushButton* pb = dynamic_cast<QPushButton*>(sender());
     if (!pb)return;
     if (isCheck)
     {
         pbSend->setChecked(false);
         this->on_pbSend_clicked(false);
         t_GetVer->start(100);
         if(cbCanType->currentIndex()==0)
         connect(t_GetVer, &QTimer::timeout, [&]() {
             if(0==cbIsMutil->currentIndex())
                pcan->SendFrame(0x18ACE48D, data_ver);
             else
             {
                 for(int i=0;i<4&&i<cbPcan->count();i++)
                     if(pcanArr[i]->IsOpen())
                         pcanArr[i]->SendFrame(0x18ACE48D, data_ver);
             }
             });
         else
         {
             connect(t_GetVer, &QTimer::timeout, [&]() {
                 kcan->canSendAll(0x18ACE48D, data_ver);
                 });
         }
     }
     else
     {
         t_GetVer->stop();
     }

}
void QtCanPlatform::on_pbClearLogShow_clicked()
{
    if (textBrowser)
        textBrowser->clear();
}
void QtCanPlatform::on_lineEdit_editingFinished()
{
    QLineEdit* le = dynamic_cast<QLineEdit*>(sender());
    if (!le)return;
    runStep = le->text().toInt();
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
void QtCanPlatform::readSetFile()
{
    
    qGboleData* gb = qGboleData::getInstance();
    if (!gb)
        return;
    gb->read_ini();
    struct autoTestData at = gb->getATData();
    lowVolt = at.m_iLowVolt;
    highVolt = at.m_iHeightVolt;
    m_iHeatTempture = at.m_iHeatTempture;
    m_iPowerTempture = at.m_iPowerTempture;
    m_iOverTime = at.m_iOverTime;
    m_iVoltError = at.m_iVoltError;
    m_iVoltStep = at.m_iVoltError;


}
/******************************************************
* @brief: (丢弃了)创建ini文件,当readSetFile()函数检测到ini文件不存在时，就调用这个函数创建
* @param: void
* @return: void
******************************************************/
void QtCanPlatform::configSetFile()
{
    //程序目录下Data文件，名字为Jugde.ini
    QString filepath = QApplication::applicationDirPath() + "/Data/Jugde.ini";
    QSettings* setf = new QSettings(filepath, QSettings::IniFormat);
    
    setf->setValue("lowVolt", lowVolt);
    setf->setValue("highVolt", highVolt);
    setf->setValue("avgPower", avgPower);
    setf->setValue("m_iHeatTempture", m_iHeatTempture);
    setf->setValue("m_iPowerTempture", m_iPowerTempture);
    setf->setValue("m_iOverTime", m_iOverTime);
    
    setf->setValue("rmFirstFream", rmFirstFream);
    setf->setValue("agvPowerFream", agvPowerFream);
}
void QtCanPlatform::on_autoWork(bool isRun)
{
    if (isRun)
    {
        QMessageBox tip(QMessageBox::Warning, "tips", tr("请确认已经打开了设备"), QMessageBox::Yes | QMessageBox::No);
        int ret = tip.exec();
        if (ret != QMessageBox::Yes)
            return;
        bool b1 = dCtrl->getProcess1State();
        bool b2 = dCtrl->getProcess2State();
        bool b3 = dCtrl->getProcess3State();
        if (b1)
        {
            QString text = dCtrl->getPhuCode(1).trimmed();
            if (text.isEmpty())
            {
                QMessageBox tip(QMessageBox::Warning, "tips", tr("1#件未输入二维码"), QMessageBox::Yes | QMessageBox::No);
                dCtrl->on_pbProcessSet_clicked(false);
                return;
            }
        }
        if (b2)
        {
            QString text = dCtrl->getPhuCode(2).trimmed();
            if (text.isEmpty())
            {
                QMessageBox tip(QMessageBox::Warning, "tips", tr("2#件未输入二维码"), QMessageBox::Yes | QMessageBox::No);
                dCtrl->on_pbProcessSet_clicked(false);
                return;
            }
        }
        if (b3)
        {
            QString text = dCtrl->getPhuCode(3).trimmed();
            if (text.isEmpty())
            {
                QMessageBox tip(QMessageBox::Warning, "tips", tr("3#件未输入二维码"), QMessageBox::Yes | QMessageBox::No);
                dCtrl->on_pbProcessSet_clicked(false);
                return;
            }
        }
        _bWork = true;
        QtConcurrent::run(this, &QtCanPlatform::workRun);
    }
    else
    {
        _bWork = false;
        on_pbSend_clicked(false);
        //pbSend->setChecked(false);
    }
        //workRun();
}
void QtCanPlatform::on_recSigEndRunWork(int n,int channel)
{
    if (n == 0)
    {
        runStep = -1;
        QLOG_INFO() << "Process Ending";
        _bWork = false;
        dCtrl->setWorkButton(0);
    }
    else
    {
        if (1 == n)
        {
            runStep = -1;
            QMessageBox tip2(QMessageBox::Warning, "tips", tr("1#工位产品未放到出水孔里,请放好再重新开始"));
            tip2.exec();
            dCtrl->setWorkButton(0);

            on_autoWork(false);
        }
        else if (2 == n)
        {
            runStep = -1;
            QMessageBox tip2(QMessageBox::Warning, "tips", tr("2#工位产品未放到出水孔里,请放好再重新开始"));
            tip2.exec();
            dCtrl->setWorkButton(0);
            on_autoWork(false);
        }
        else if (3 == n)
        {
            runStep = -1;
            QMessageBox tip2(QMessageBox::Warning, "tips", tr("3#工位产品未放到出水孔里,请放好再重新开始"));
            tip2.exec();
            dCtrl->setWorkButton(0);
            on_autoWork(false);
        }
        else if (4 == n)
        {
            runStep = -1;
            QMessageBox tip2(QMessageBox::Warning, "tips", tr("未勾选工位，请至少选择一个工位"));
            tip2.exec();
            dCtrl->setWorkButton(0);
            on_autoWork(false);

        }
        else if (5 == n)                    //发送请求
        {
            dCtrl->setResInLabel(0, "", QColor(255, 255, 255));
            dCtrl->setResInLabel(1, "", QColor(255, 255, 255));
            dCtrl->setResInLabel(2, "", QColor(255, 255, 255));
            runStep = 5;
            on_pbSend_clicked(true);
            pbSend->setChecked(true);
        }
        else if (10 == n)                   //冷水机
        {
            //冷水机外循环（水流）
            dCtrl->on_pbStartOutCricle_clicked(true);
            //内循环（制冷）
            dCtrl->on_pbStartInCricle_clicked(true);
            QLOG_INFO() << "打开冷水机";

            runStep = 10;
        }
        else if (12 == n)                   //冷水机
        {
            ////冷水机外循环（水流）
            //dCtrl->on_pbStartOutCricle_clicked(true);
            //内循环（制冷）
            dCtrl->on_pbStartInCricle_clicked(true);
            QLOG_INFO() << "打开冷水机制冷";

            runStep = 12;
        }
        else if (15 == n)                   //上高压
        {
            if (0 == channel)
            {
                dCtrl->setHV_1(600, 30);
                dCtrl->setHV_2(600, 30);
            }
            else if (1 == channel)
            {
                dCtrl->setHV_1(600, 30);
            }
            else if (2 == channel)
            {
                dCtrl->setHV_2(600, 30);
            }
            runStep = 15;
            QLOG_INFO() << "打开高压电源";
        }
        else if (20 == n)
        {
            QLOG_INFO() << "使能";
            if (cbSelectModel->currentText().contains("海卓"))
            {
                
                QComboBox* cb = dynamic_cast<QComboBox*> (tableView->cellWidget(0, 1));
                if (!cb)
                {
                    _bWork = false;
                    QLOG_WARN() << "get enable button fail,autotest exit";
                    QMessageBox::warning(this, tr("Error"), tr("获取使能按钮出错，自动化测试退出"));
                    return;
                }
                cb->setCurrentIndex(1);
                QTableWidgetItem* it = tableView->item(1, 2);
                if (!it)
                {
                    _bWork = false;
                    QLOG_WARN() << "set power fail,autotest exit";
                    QMessageBox::warning(this, tr("Error"), tr("设置功率出错，自动化测试退出"));
                    return;
                }
                if(channel==0)
                     it->setText("7000");
                else
                    it->setText("0");
                it = tableView->item(2, 2);
                if (!it)
                {
                    _bWork = false;
                    QLOG_WARN() << "set tempture fail,autotest exit";
                    QMessageBox::warning(this, tr("Error"), tr("设置温度出错，自动化测试退出"));
                    return;
                }
                if (channel == 0)
                    it->setText("85");
                else
                    it->setText("0");
            }
            else if (cbSelectModel->currentText().contains("标准件"))
            {
                QComboBox* cb = dynamic_cast<QComboBox*> (tableView->cellWidget(1, 1));
                if (!cb)
                {
                    _bWork = false;
                    QLOG_WARN() << "get enable button fail,autotest exit";
                    QMessageBox::warning(this, tr("Error"), tr("获取使能按钮出错，自动化测试退出"));
                    return;
                }
                cb->setCurrentIndex(1);
                QTableWidgetItem* it = tableView->item(0, 2);
                if (!it)
                {
                    _bWork = false;
                    QLOG_WARN() << "set power fail,autotest exit";
                    QMessageBox::warning(this, tr("Error"), tr("设置功率出错，自动化测试退出"));
                    return;
                }
                if(0==channel)
                    it->setText("100");
                else
                    it->setText("0");
               
            }
            runStep = 20;
        }
        else if (25 == n)
        {

        QLOG_INFO() << "关使能";
        if (cbSelectModel->currentText().contains("海卓"))
        {

            QComboBox* cb = dynamic_cast<QComboBox*> (tableView->cellWidget(0, 1));
            if (!cb)
            {
                _bWork = false;
                QLOG_WARN() << "get enable button fail,autotest exit";
                QMessageBox::warning(this, tr("Error"), tr("获取使能按钮出错，自动化测试退出"));
                return;
            }
            cb->setCurrentIndex(0);
            QTableWidgetItem* it = tableView->item(1, 2);
            if (!it)
            {
                _bWork = false;
                QLOG_WARN() << "set power fail,autotest exit";
                QMessageBox::warning(this, tr("Error"), tr("设置功率出错，自动化测试退出"));
                return;
            }
            it->setText("0");
            it = tableView->item(2, 2);
            if (!it)
            {
                _bWork = false;
                QLOG_WARN() << "set tempture fail,autotest exit";
                QMessageBox::warning(this, tr("Error"), tr("设置温度出错，自动化测试退出"));
                return;
            }
            it->setText("0");
        }
        else if (cbSelectModel->currentText().contains("标准件"))
        {
            QComboBox* cb = dynamic_cast<QComboBox*> (tableView->cellWidget(1, 1));
            if (!cb)
            {
                QLOG_WARN() << "get enable button fail";
            }
            cb->setCurrentIndex(0);
            QTableWidgetItem* it = tableView->item(0, 2);
            if (!it)
            {
                
                QLOG_WARN() << "set power fail";
                
            }
            it->setText("0");

        }

            //下高压
            dCtrl->on_pbOffVolt_clicked();
            QLOG_INFO() << "下高压";
            runStep = 25;
        }
        else if (28 == n)
        {
            //冷水机外循环
            dCtrl->on_pbStartInCricle_clicked(false);
            QLOG_INFO() << "关闭冷水机内循环";
            runStep = 28;
        }
        else if (30 == n)
        {
            //冷水机内外循环
            dCtrl->on_pbStartOutCricle_clicked(false);
            dCtrl->on_pbStartInCricle_clicked(false);
            QLOG_INFO() << "关闭冷水机内外循环";
            runStep = 30;
        }
        else if (32 == n)
        {
            bool b = dCtrl->outCycleState();
            if (!b)
            {
                runStep = 32;
            }

        }
        else if (35 == n)
        {
            //吹水功能
            dCtrl->on_pbBlowWater_4_clicked(true);
            QLOG_INFO() << "吹水";
        }
        else if (55 == n)
        {
            if (dCtrl->getProcess1State())
            {
                QColor C(0, 200, 0);
                QColor C2(200, 0, 0);
                if (Error[0].size() > 0)
                {
                    QString nn;
                    std::set<QString>::iterator b = Error[0].begin();
                    while (b != Error[0].end())
                    {
                        nn += *b+"\n";
                        b++;
                    }
                    nn = "\n" + nn;
                    dCtrl->setResInLabel(0, phuRes_1 +nn, C);
                }  
                else
                {
                    dCtrl->setResInLabel(0, phuRes_1, C2);
                }
            }
            if (dCtrl->getProcess2State())
            {
                QColor C(0, 200, 0);
                QColor C2(200, 0, 0);
                if (Error[1].size() > 0)
                {
                    QString nn;
                    std::set<QString>::iterator b = Error[1].begin();
                    while (b != Error[1].end())
                    {
                        nn += *b+"\n";
                        b++;
                    }
                    nn = "\n" + nn;
                    dCtrl->setResInLabel(1, phuRes_2  + nn, C);
                }
                else
                {
                    dCtrl->setResInLabel(1, phuRes_2, C2);
                }
            }
            if (dCtrl->getProcess3State())
            {
                QColor C(0, 200, 0);
                QColor C2(200, 0, 0);
                if (Error[2].size() > 0)
                {
                    QString nn;
                    std::set<QString>::iterator b = Error[2].begin();
                    while (b != Error[2].end())
                    {
                        nn += *b+"\n";
                        b++;
                    }
                    nn = "\n" + nn;
                    dCtrl->setResInLabel(2, phuRes_3  + nn, C);
                }
                else
                {
                    dCtrl->setResInLabel(2, phuRes_3, C2);
                }
            }
        }
        else if (n == 60)
        {
            m1_strPhuCode = dCtrl->getPhuCode(1);
            m2_strPhuCode = dCtrl->getPhuCode(2);
            m3_strPhuCode = dCtrl->getPhuCode(3);
            runStep = 60;
        }
        else if (n==65)
        {
            on_autoWork(false);
            dCtrl->on_setProcessSetState(false);
        }
        else if (330 <= n && n<=998)
        {
            if (0 == channel)
            {
                dCtrl->setHV_1(n, 60);
                dCtrl->setHV_2(n, 60);
            }
            else if (1 == channel)
            {
                dCtrl->setHV_1(n, 60);
            }
            else if (2 == channel)
            {
                dCtrl->setHV_2(n, 60);
            }
           
            QLOG_INFO() << "setVolt:"<<n;
        }
        else if (999 == n)
        {
            //进度条
            if (!progress)
            {
                progress = new QProgressDialog(this);
                progress->setModal(true);
                progress->setRange(0, 120);
            }
            progress->setLabelText(tr("吹水功能正在进行中"));
            progress->reset();
            progress->setAutoClose(false);
            progress->setAutoReset(false);
            progress->show();
        }
        else if (9999 == n)
        {
            progress->setValue(channel);
            if (channel >= 120)
            {
                progress->setLabelText(tr("吹水完成，请关闭各个冷水阀"));
            }
            
        }
    }
}
void QtCanPlatform::workRun()
{
    
   // m1_listTestRes.append("")


    if (!dCtrl)
    {
        QLOG_INFO() << "dCtrl is Nullptr";
        return;
    }
    //高压保护结果
    bool HVProtectRes_1 = false;
    bool HVProtectRes_2 = false;
    bool HVProtectRes_3 = false;

    phuRes_1 = "";
    phuRes_2 = "";
    phuRes_3 = "";
    //清除之前的结果
    emit sigEndRunWork(55, 0);
    //if(dCtrl->ge)
    //1 判断哪个工位要测试
    //2 打开相应的IO
    //3 开冷水机，上高压电
    //4 使能
    if (!_bWork)
    {
        QLOG_INFO() << "_bWork is false";
        return;
    }
    //获取开启了哪个工位
    bool b1 = dCtrl->getProcess1State();
    bool b2 = dCtrl->getProcess2State();
    bool b3 = dCtrl->getProcess3State();
    if (!(b1 || b2 || b3))
    {
        //三个工位没开启
        emit sigEndRunWork(4,0);
        return;
    }
    if (b1)
    {
        if (dCtrl->bInState_One[5])
        {
            emit sigEndRunWork(1,0);
            return;
        }
        /* dCtrl->on_pbCover_clicked(true);
        dCtrl->on_pbColdWater_clicked(true);*/
    }
    if (b2)
    {
        if (dCtrl->bInState_Two[1])
        {
            emit sigEndRunWork(2,0);
            return;
        }
        /*dCtrl->on_pbCover_2_clicked(true);
        dCtrl->on_pbColdWater_2_clicked(true);*/
    }
    if (b3)
    {
        if (dCtrl->bInState_Two[5])
        {
            emit sigEndRunWork(3,0);
            return;
        }
        /*dCtrl->on_pbCover_3_clicked(true);
        dCtrl->on_pbColdWater_3_clicked(true);*/
    }
    emit sigEndRunWork(5,0);
    while (_bWork)
    {
        if (runStep == 5)
            break;
        Sleep(100);
    }

    Sleep(3000);
    
    //打开高压电源
    emit sigEndRunWork(15,0);

    while (_bWork)
    {
        if (runStep == 15)
            break;
        Sleep(100);
    }

    //测高压保护功能
    bool bflag1 = true;
    bool bflag2 = true;
    bool bflag3 = true;

    //等待高压到600;
    while (_bWork)
    {
        Sleep(100);
        if (!_bWork)
        {
            QLOG_INFO() << "Exit!,_bWork turn to false."<< runStep;
            return;
        }
        float diff = std::abs(realVolt[0] - 600);
        float diff2 = std::abs(realVolt[1] - 600);
        float diff3 = std::abs(realVolt[2] - 600);
        if (diff > 8 && b1)
            continue;
        if (diff2 > 8 && b2)
            continue;
        if (diff3 > 8 && b3)
            continue;

        QLOG_INFO() << "Tempture up to 600,done";
        break;
    }
    if (!_bWork)
    {
        QLOG_INFO() << "Exit!,_bWork turn to false." << runStep;
        return;
    }
    //若是海卓的，需要使能
    emit sigEndRunWork(20, 1);

    while (_bWork)
    {
        if (runStep == 20)
            break;
        Sleep(100);
    }

    if (!_bWork)
    {
        QLOG_INFO() << "Exit!,_bWork turn to false." << runStep;
        return;
    }

    emit sigEndRunWork(lowVolt,0);
    //高压到330V,每次加3V，然后一直加到高压正常
    int HVVar1 = lowVolt;
    int HVVar2 = lowVolt;
    int low_lastHv1 = HVVar1;
    int low_lastHv2 = HVVar2;
    int low_lastHv3 = HVVar2;
    bflag1 = true; bflag1 &= b1;
    bflag2 = true; bflag2 &= b2;
    bflag3 = true; bflag3 &= b3;
    while (_bWork)
    {
        Sleep(100);
        float diff = std::abs(realVolt[0] - HVVar1);
        float diff2 = std::abs(realVolt[1] - HVVar2);
        float diff3 = std::abs(realVolt[2] - HVVar2);
        if (diff > m_iVoltError && b1)
            continue;
        if (diff2 > m_iVoltError && b2)
            continue;
        if (diff3 > m_iVoltError && b3)
            continue;
        Sleep(2000);
        if (b1 && bflag1)
        {
           
            if (realHVErr[0] != "高压异常")
            {
                bflag1 = false;
            }
            else if (realHVErr[0] == "高压异常")
            {
                low_lastHv1 = HVVar1;
            }
                
        }
        if (b2 && bflag2)
        {

            if (realHVErr[1] != "高压异常")
            {
                bflag2 = false;
            }
            else if (realHVErr[1] == "高压异常")
            {
                low_lastHv2 = HVVar2;
            }
        }
        if (b3 && bflag3)
        {
            if (realHVErr[2] != "高压异常")
            {
                bflag3 = false;
            }
            else if (realHVErr[2] == "高压异常")
            {
                low_lastHv3 = HVVar2;
            }
        }
        //三路都不报高压异常了
        if (!(bflag1 || bflag2 || bflag3))
            break;
        //继续往上加
        HVVar1 += m_iVoltStep;
        HVVar2 += m_iVoltStep;
        if(bflag1)
            emit sigEndRunWork(HVVar1,1);
        if (bflag2)
            emit sigEndRunWork(HVVar2, 2);
        if (bflag3)
            emit sigEndRunWork(HVVar2,2);
        
    }
    if (!_bWork)
    {
        emit sigEndRunWork(25, 0);
        QLOG_INFO() << "_bWork is false,exit:"<<runStep;
        return;
    }
    if (HVVar1 <= 334|| HVVar2<=334)
    {
        QLOG_INFO() << "没有高压欠压保护，退出测试";
        if (b1)
        {
            phuRes_1 = "2#没有高压欠压保护";
        }
        if (b2)
        {
            phuRes_3 = "2#没有高压欠压保护";
        }
        if (b3)
        {
            phuRes_3 = "3#没有高压欠压保护";
        }
        //显示结果
        emit sigEndRunWork(55, 0);
        return;
    }
    if (!_bWork)
    {
        emit sigEndRunWork(25, 0);
        QLOG_INFO() << "_bWork is false,exit:" << runStep;
        return;
    }
    //在刚才的电压上，每次减3V，然后一直减到高压正常
    bflag1 = true; bflag1 &= b1;
    bflag2 = true; bflag2 &= b2;
    bflag3 = true; bflag3 &= b3;
    sigEndRunWork(HVVar1, 1);
    sigEndRunWork(HVVar2, 2);
    while (_bWork)
    {
        Sleep(100);
        float diff = std::abs(realVolt[0] - HVVar1);
        float diff2 = std::abs(realVolt[1] - HVVar2);
        float diff3 = std::abs(realVolt[2] - HVVar2);
        if (diff > m_iVoltError && b1)
            continue;
        if (diff2 > m_iVoltError && b2)
            continue;
        if (diff3 > m_iVoltError && b3)
            continue;
        Sleep(2000);
        if (b1 && bflag1)
        {

            if (realHVErr[0] != "高压正常")
            {
                bflag1 = false;
            }
            else if (realHVErr[0] == "高压正常")
            {
                low_lastHv1 = HVVar1;
            }

        }
        if (b2 && bflag2)
        {

            if (realHVErr[1] != "高压正常")
            {
                bflag2 = false;
            }
            else if (realHVErr[1] == "高压正常")
            {
                low_lastHv2 = HVVar2;
            }
        }
        if (b3 && bflag3)
        {
            if (realHVErr[2] != "高压正常")
            {
                bflag3 = false;
            }
            else if (realHVErr[2] == "高压正常")
            {
                low_lastHv3 = HVVar2;
            }
        }
        //三路都不报高压异常了
        if (!(bflag1 || bflag2 || bflag3))
            break;
        //继续往下减
        HVVar1 -= m_iVoltStep;
        HVVar2 -= m_iVoltStep;
        if (bflag1)
            emit sigEndRunWork(HVVar1, 1);
        if (bflag2)
            emit sigEndRunWork(HVVar2, 2);
        if (bflag3)
            emit sigEndRunWork(HVVar2, 2);
        
    }
    if (b1)
    {
        if (bflag1)
        {
            phuRes_1 += "低压保护电压：" + QString::number(low_lastHv1) + "(异常值)\n";
            QLOG_INFO() << "1#低压保护电压：" << low_lastHv1 << "(异常值)";
            m1_listTestRes.append("1#低压保护电压," + QString::number(low_lastHv1) + "(异常值)");
        }
        else
        {
            phuRes_1 += "低压保护电压：" + QString::number(low_lastHv1) + "\n";
            QLOG_INFO() << "1#低压保护电压：" << low_lastHv1;
            m1_listTestRes.append("1#低压保护电压," + QString::number(low_lastHv1));
        }
    }
    if (b2)
    {
        if (bflag2)
        {
            phuRes_2 += "低压保护电压：" + QString::number(low_lastHv2) + "(异常值)\n";
            QLOG_INFO() << "2#低压保护电压：" << low_lastHv2 << "(异常值)";
            m2_listTestRes.append("2#低压保护电压," + QString::number(low_lastHv2) + "(异常值)");
        }
        else
        {
            phuRes_2 += "低压保护电压：" + QString::number(low_lastHv2) + "\n";
            QLOG_INFO() << "2#低压保护电压：" << low_lastHv2;
            m2_listTestRes.append("2#低压保护电压," + QString::number(low_lastHv2));
        }

    }
    if (b3)
    {
        if (bflag3)
        {
            phuRes_3 += "低压保护电压：" + QString::number(low_lastHv3) + "(异常值)\n";
            QLOG_INFO() << "3#低压保护电压：" << low_lastHv3 << "(异常值)";
            m3_listTestRes.append("3#低压保护电压," + QString::number(low_lastHv3) + "(异常值)");
        }
        else
        {
            phuRes_3 += "低压保护电压：" + QString::number(low_lastHv3) + "\n";
            QLOG_INFO() << "3#低压保护电压：" << low_lastHv3;
            m3_listTestRes.append("3#低压保护电压," + QString::number(low_lastHv3));
        }


    }
    
    //显示结果
    emit sigEndRunWork(55, 0);

    if (!_bWork)
    {
        emit sigEndRunWork(25, 0);
        QLOG_INFO() << "_bWork is false,exit:" << runStep;
        return;
    }
    //=======增加步骤
    //验证高压保护

    emit sigEndRunWork(highVolt, 0);
    HVVar1 = HVVar2 = highVolt;
    sigEndRunWork(HVVar1, 1);
    sigEndRunWork(HVVar2, 2);
    bflag1 = true; bflag1 &= b1;
    bflag2 = true; bflag2 &= b2;
    bflag3 = true; bflag3 &= b3;
    while (_bWork)
    {
        Sleep(100);
        float diff = std::abs(realVolt[0] - HVVar1);
        float diff2 = std::abs(realVolt[1] - HVVar2);
        float diff3 = std::abs(realVolt[2] - HVVar2);
        if (diff > m_iVoltError && b1)
            continue;
        if (diff2 > m_iVoltError && b2)
            continue;
        if (diff3 > m_iVoltError && b3)
            continue;
        Sleep(2000);
        if (b1 && bflag1)
        {

            if (realHVErr[0] != "高压正常")
            {
                bflag1 = false;
            }
            else if (realHVErr[0] == "高压正常")
            {
                low_lastHv1 = HVVar1;
            }

        }
        if (b2 && bflag2)
        {

            if (realHVErr[1] != "高压正常")
            {
                bflag2 = false;
            }
            else if (realHVErr[1] == "高压正常")
            {
                low_lastHv2 = HVVar2;
            }
        }
        if (b3 && bflag3)
        {
            if (realHVErr[2] != "高压正常")
            {
                bflag3 = false;
            }
            else if (realHVErr[2] == "高压正常")
            {
                low_lastHv3 = HVVar2;
            }
        }
        //三路都不报高压异常了
        if (!(bflag1 || bflag2 || bflag3))
            break;
        //继续往上加
        HVVar1 += m_iVoltStep;
        HVVar2 += m_iVoltStep;
        if (bflag1)
            emit sigEndRunWork(HVVar1, 1);
        if (bflag2)
            emit sigEndRunWork(HVVar2, 2);
        if (bflag3)
            emit sigEndRunWork(HVVar2, 2);
        
    }
    //在刚才的电压上，每次减3V，然后一直减到高压正常
    bflag1 = true; bflag1 &= b1;
    bflag2 = true; bflag2 &= b2;
    bflag3 = true; bflag3 &= b3;

    if (!_bWork)
    {
        emit sigEndRunWork(25, 0);
        QLOG_INFO() << "_bWork is false,exit:" << runStep;
        return;
    }

    if (b1)
        emit sigEndRunWork(HVVar1, 1);
    if (b2)
        emit sigEndRunWork(HVVar2, 2);
    if (b3)
        emit sigEndRunWork(HVVar2, 2);
    while (_bWork)
    {
        Sleep(100);
        float diff = std::abs(realVolt[0] - HVVar1);
        float diff2 = std::abs(realVolt[1] - HVVar2);
        float diff3 = std::abs(realVolt[2] - HVVar2);
        if (diff > m_iVoltError && b1)
            continue;
        if (diff2 > m_iVoltError && b2)
            continue;
        if (diff3 > m_iVoltError && b3)
            continue;
        Sleep(2000);
        if (b1 && bflag1)
        {

            if (realHVErr[0] != "高压异常")
            {
                bflag1 = false;
            }
            else if (realHVErr[0] == "高压异常")
            {
                low_lastHv1 = HVVar1;
            }

        }
        if (b2 && bflag2)
        {

            if (realHVErr[1] != "高压异常")
            {
                bflag2 = false;
            }
            else if (realHVErr[1] == "高压异常")
            {
                low_lastHv2 = HVVar2;
            }
        }
        if (b3 && bflag3)
        {
            if (realHVErr[2] != "高压异常")
            {
                bflag3 = false;
            }
            else if (realHVErr[2] == "高压异常")
            {
                low_lastHv3 = HVVar2;
            }
        }
        //三路都不报高压异常了
        if (!(bflag1 || bflag2 || bflag3))
            break;
        //继续往下减
        HVVar1 -= m_iVoltStep;
        HVVar2 -= m_iVoltStep;
        if (bflag1)
            emit sigEndRunWork(HVVar1, 1);
        if (bflag2)
            emit sigEndRunWork(HVVar1, 2);
        if (bflag3)
            emit sigEndRunWork(HVVar2, 2);
        
    }

    if (b1) 
    {
        if (bflag1)
        {
            phuRes_1 += "高压保护电压：" + QString::number(low_lastHv1) + "(异常值)\n"; 
            QLOG_INFO() << "1#高压保护电压：" << low_lastHv1<<"(异常值)";
            m1_listTestRes.append("1#高压保护电压," + QString::number(low_lastHv1) + "(异常值)");
        }
        else
        {
            phuRes_1 += "高压保护电压：" + QString::number(low_lastHv1) + "\n";
            QLOG_INFO() << "1#高压保护电压：" << low_lastHv1;
            m1_listTestRes.append("1#高压保护电压," + QString::number(low_lastHv1));
        }
    }
    if (b2)
    { 
        if (bflag2)
        {
            phuRes_2 += "高压保护电压：" + QString::number(low_lastHv2) + "(异常值)\n";
            QLOG_INFO() << "2#高压保护电压：" << low_lastHv2 << "(异常值)";
            m2_listTestRes.append("2#高压保护电压," + QString::number(low_lastHv2) + "(异常值)");
        }
        else
        {
            phuRes_2 += "高压保护电压：" + QString::number(low_lastHv2) + "\n";
            QLOG_INFO() << "2#高压保护电压：" << low_lastHv2; 
            m2_listTestRes.append("2#高压保护电压," + QString::number(low_lastHv2));
        }
       
    }
    if (b3) 
    { 
        if (bflag3)
        {
            phuRes_3 += "高压保护电压：" + QString::number(low_lastHv3) + "(异常值)\n";
            QLOG_INFO() << "3#高压保护电压：" << low_lastHv3 << "(异常值)";
            m3_listTestRes.append("3#高压保护电压," + QString::number(low_lastHv3) + "(异常值)");
        }
        else
        {
            phuRes_3 += "高压保护电压：" + QString::number(low_lastHv3) + "\n";
            QLOG_INFO() << "3#高压保护电压：" << low_lastHv3;
            m3_listTestRes.append("3#高压保护电压," + QString::number(low_lastHv3));
        }
       

    }
    //显示结果
    emit sigEndRunWork(55, 0);

    if (!_bWork)
    {
        emit sigEndRunWork(25, 0);
        QLOG_INFO() << "_bWork is false,exit:" << runStep;
        return;
    }
    //下面测试温度
    bflag1 = true;
    bflag2 = true;
    bflag3 = true;
    

    //打开冷水机
    emit sigEndRunWork(10,0);
   
    while (_bWork)
    {
        if (runStep == 10)
            break;
        Sleep(100);
    }
    if (b1)
        emit sigEndRunWork(600, 1);
    if (b2)
        emit sigEndRunWork(600, 2);
    if (b3)
        emit sigEndRunWork(600, 2);
    //等待温度
    bflag1 = true;
    bflag2 = true;
    bflag3 = true;
   
    if (!_bWork)
    {
        emit sigEndRunWork(25, 0);
        QLOG_INFO() << "_bWork is false,exit:" << runStep;
        return;
    }
    QLOG_INFO() << "等待温度-15";
    while (_bWork)
    { 
        if (b1 && bflag1)
        {  
            if (realWTemp[0] > m_iHeatTempture)
                continue;                   //没到温度，继续循环等
            bflag1 = false;                 //温度到了，就不要进入了
        }
        if (b2 && bflag2)
        {
            if (realWTemp[1] > m_iHeatTempture)
                continue;                   //没到温度，继续循环等
            bflag2 = false;                 //温度到了，就不要进入了
        }
        if (b3 && bflag3)
        {
            if (realWTemp[2] > m_iHeatTempture)
                continue;                   //没到温度，继续循环等
            bflag3 = false;                 //温度到了，就不要进入了
        }
        break;
        Sleep(100);
    }

    if (!_bWork)
    {
        emit sigEndRunWork(25, 0);
        QLOG_INFO() << "_bWork is false,exit:" << runStep;
        return;
    }

    //使能
    emit sigEndRunWork(20,0);
    while (_bWork)
    {
        if (runStep == 20)
            break;
        Sleep(100);
    }
    //休眠3秒，因为升温没那么快
    Sleep(3000);

    if (!_bWork)
    {
        emit sigEndRunWork(25, 0);
        QLOG_INFO() << "_bWork is false,exit:" << runStep;
        return;
    }

    PowerArr[0].clear();
    PowerArr[1].clear();
    PowerArr[2].clear();
    PowerArr[3].clear();

    isRecordError = true;
    //等待温度
    bflag1 = true;
    bflag2 = true;
    bflag3 = true;
    bool bIn = true;
    QTime t_OT = QTime::currentTime().addSecs(m_iOverTime);
    while (_bWork)
    {
        bIn = false;
        if (b1 && bflag1)
        {
            //这个是为了跳出外面这个循环
            bIn = true;
            if (realWTemp[0] == m_iPowerTempture)
            {
                if (PowerArr[0].size() < agvPowerFream)
                    PowerArr[0].push_back(realPower[0]);                //没到温度，继续循环等
                else
                    bflag1 = false;                                     //够数了
            }
            if(realWTemp[0] > m_iPowerTempture)
                bflag1 = false;                                         //超温了
        }
        if (b2 && bflag2)
        {
            //这个是为了跳出外面这个循环
            bIn = true;
            if (realWTemp[1] == m_iPowerTempture)
            {
                if (PowerArr[1].size() < agvPowerFream)
                    PowerArr[1].push_back(realPower[1]);                //没到温度，继续循环等
                else
                    bflag2 = false;                                     //够数了
            }
            if (realWTemp[1] > m_iPowerTempture)
                bflag2 = false;                                         //超温了
        }
        if (b3 && bflag3)
        {
            //这个是为了跳出外面这个循环
            bIn = true;
            if (realWTemp[2] == m_iPowerTempture)
            {
                if (PowerArr[2].size() < agvPowerFream)
                    PowerArr[2].push_back(realPower[2]);                //没到温度，继续循环等
                else
                    bflag3 = false;                                     //够数了
            }
            if (realWTemp[2] > m_iPowerTempture)
                bflag3 = false;                                         //超温了
        }

        if (QTime::currentTime() > t_OT)
        {
            QLOG_INFO() << tr("等待水温超时，功率测试模块跳过");
            break;
        }
        //上面三个if都没有跳进去，说明要跳出这个循环了
        if (!bIn)
            break;
        Sleep(1000);
        //三个都超温了
        if (realWTemp[0] > m_iPowerTempture && realWTemp[1] > m_iPowerTempture && realWTemp[2] > m_iPowerTempture)
        {
            break;
        }
    }   
    isRecordError = false;
    if (b1)
    {
        int sum=0;
        for (int i = 0; i < PowerArr[0].size(); i++)
        {
            sum += PowerArr[0].at(i);
        }
        float avage = 0;
        if(PowerArr[0].size()>0)
            avage = sum / PowerArr[0].size();
        QLOG_INFO() << "1#件平均功率：" << avage << ",记录了" << PowerArr[0].size() << "次";
        phuRes_1 += "1#件平均功率：" + QString::number(avage);
        m1_listTestRes.append("1#件平均功率," + QString::number(avage));
        
    }
    if (b2)
    {
        int sum = 0;
        for (int i = 0; i < PowerArr[1].size(); i++)
        {
            sum += PowerArr[1].at(i);
        }
        float avage = 0;
        if (PowerArr[1].size() > 0)
            avage = sum / PowerArr[1].size();
        QLOG_INFO() << "2#件平均功率：" << avage << ",记录了" << PowerArr[1].size() << "次";
        phuRes_2 += "2#件平均功率：" + QString::number(avage);
        m2_listTestRes.append("2#件平均功率," + QString::number(avage));
        
    }
    if (b3)
    {
        int sum = 0;
        for (int i = 0; i < PowerArr[2].size(); i++)
        {
            sum += PowerArr[2].at(i);
        }
        float avage = 0;
        if (PowerArr[2].size() > 0)
            avage = sum / PowerArr[2].size();
        QLOG_INFO() << "3#件平均功率：" << avage << ",记录了" << PowerArr[2].size() << "次";
        phuRes_3 += "3#件平均功率：" + QString::number(avage);
        m3_listTestRes.append("3#件平均功率," + QString::number(avage));
       
    }

    emit sigEndRunWork(55, 0);
    if (!_bWork)
    {
        emit sigEndRunWork(25, 0);
        QLOG_INFO() << "_bWork is false,exit:" << runStep;
        return;
    }

    //关闭内循环
    //===================
    emit sigEndRunWork(30, 0);
    Sleep(3000);
    bflag1 = true; bflag1 &= b1;
    bflag2 = true; bflag2 &= b2;
    bflag3 = true; bflag3 &= b3;
    int protectV1 = 0;
    int protectV2 = 0;
    int protectV3 = 0;
    t_OT = QTime::currentTime().addSecs(m_iOverTime);
    while (_bWork)
    {

        if (b1 && bflag1)
        {
            if (realOTPro[0] == "过温保护")
            {
                protectV1 = realWTemp[0];
                bflag1 = false;
            }
        }
        if (b2 && bflag2)
        {
            if (realOTPro[1] == "过温保护")
            {
                protectV2 = realWTemp[1];
                bflag2 = false;
            }
        }
        if (b3 && bflag3)
        {
            if (realOTPro[2] == "过温保护")
            {
                protectV3 = realWTemp[2];
                bflag3 = false;
            }
        }
        if (QTime::currentTime() > t_OT)
        {
            QLOG_INFO() << "过温保护测试模块超时";
            break;
        }
           
        if (!(bflag1 || bflag2 || bflag3))
            break;

    }
    if (b1)
    {
        if (bflag1)
        {
            phuRes_1 += "\n1#过温保护:未完成测试";
            m1_listTestRes.append("1#过温保护,未完成测试");
        }
        else
        {
            phuRes_1 += "\n1#过温保护," + QString::number(protectV1) + "°C";
            m1_listTestRes.append("1#过温保护,"+ QString::number(protectV1) + "°C");
        }
        QString er = "";
        for (std::set<QString>::iterator it = Error[0].begin(); it != Error[0].end(); it++)
            er += *it + ",";
        m1_listTestRes.append("1#件故障," + er);
        QLOG_INFO() << "1#件故障：" << er;
    }
    if (b2)
    {
        if (bflag1)
        {
            phuRes_2 += "\n2#过温保护:未完成测试";
            m2_listTestRes.append("2#过温保护,未完成测试");
        }
        else
        {
            phuRes_2 += "\n2#过温保护," + QString::number(protectV2) + "°C";
            m2_listTestRes.append("2#过温保护," + QString::number(protectV2) + "°C");
        }
        QString er = "";
        for (std::set<QString>::iterator it = Error[1].begin(); it != Error[1].end(); it++)
            er += *it + ",";
        m2_listTestRes.append("2#件故障," + er);
        QLOG_INFO() << "2#件故障：" << er;
    }
        
    if (b3)
    {
        if (bflag3)
        {
            phuRes_3 += "\n3#过温保护:未完成测试";
            m3_listTestRes.append("3#过温保护,未完成测试");
        }
        else
        {
            phuRes_3 += "\n3#过温保护," + QString::number(protectV3) + "°C";
            m3_listTestRes.append("3#过温保护," + QString::number(protectV3) + "°C");
        }

        QString er = "";
        for (std::set<QString>::iterator it = Error[2].begin(); it != Error[2].end(); it++)
            er += *it + ",";
        m3_listTestRes.append("3#件故障," + er);
        QLOG_INFO() << "3#件故障：" << er;
    }
    
    emit sigEndRunWork(60, 0);
    while (_bWork)
    {
        if (runStep == 60)
            break;
        Sleep(100);
    }
    if (b1)
    {
        if (!m1_strPhuCode.isEmpty())
            saveAutoTestRes(m1_strPhuCode, m1_listTestRes);
        else
        {
            QLOG_WARN() << "1#件未有二维码，保存名字为当前时间";
            saveAutoTestRes(m1_strPhuCode, m1_listTestRes);
        }
    }
    if (b2)
    {
        if (!m2_strPhuCode.isEmpty())
            saveAutoTestRes(m2_strPhuCode, m2_listTestRes);
        else
        {
            QLOG_WARN() << "3#件未有二维码，保存名字为当前时间";
            saveAutoTestRes(m2_strPhuCode, m2_listTestRes);
        }
    }
    if (b3)
    {
        if (!m3_strPhuCode.isEmpty())
            saveAutoTestRes(m3_strPhuCode, m3_listTestRes);
        else
        {
            QLOG_WARN() << "3#件未有二维码，保存名字为当前时间";
            saveAutoTestRes(m3_strPhuCode, m3_listTestRes);
        }
    }


    //显示
    emit sigEndRunWork(55, 0);
    //关闭使能，下高压
    emit sigEndRunWork(25,0);


    if (!_bWork)
    {
        
        QLOG_INFO() << "_bWork is false,exit:" << runStep;
        return;
    }
    //关闭内循环
    //===================
    emit sigEndRunWork(30, 0);
    Sleep(3000);
    QLOG_INFO() << "等待内外循环关闭";
    while (1)
    {
        if (!_bWork)
            return;
        Sleep(500);
        emit sigEndRunWork(32, 0);
        if (runStep < 32)
            continue;
        else
            break;
    }
    if (!_bWork)
    {
        emit sigEndRunWork(25, 0);
        QLOG_INFO() << "_bWork is false,exit:" << runStep;
        return;
    }

    //开启吹水功能
    emit sigEndRunWork(35, 0);
    int i = 0;
    //开启进度条
    emit sigEndRunWork(999, i);
    while (_bWork && i < 120)
    {
        i++;
        //更新进度条
        emit sigEndRunWork(9999, i);
        Sleep(1000);
    }
    //按钮可用
    emit sigEndRunWork(65, 0);
    return;

}