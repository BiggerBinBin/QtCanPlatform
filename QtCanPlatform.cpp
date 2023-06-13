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

QString qmyss = "QComboBox{height:25px;border: 1px solid gray;border-radius: 5px;padding:1px 2px 1px 2px;}\
QComboBox::drop-down{subcontrol-origin: padding;subcontrol-position: top right;width: 15px;border-left-width: 1px;border-left-color: darkgray;border-left-style: solid;border-top-right-radius: 3px;border-bottom-right-radius: 3px;image: url(:/QtCanPlatform/Resources/down.png)}";
QString rollTitleStyle = "QHeaderView::section {background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #0078d7, stop: 0.5 #0078d7,stop: 0.6 #0078d7, stop:1 #0078d7);color: white;border:1px solid;border-color:white;font-weight: bold;}QHeaderView{background-color:#0078d7}";
#define _NEED_CODE_
QtCanPlatform::QtCanPlatform(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    //防止休眠
    SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED);
    initLogger();
    this->setStyleSheet(qmyss);
    readSetFile();
    initUi();
    sendTimer = new QTimer(this);
    connect(sendTimer, &QTimer::timeout, this, &QtCanPlatform::sendData);
    
    QFile F(QApplication::QCoreApplication::applicationDirPath() + "/Data/title_version.kus");
    if (F.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        this->setWindowTitle(F.readAll());
    }
    else
    {
        this->setWindowTitle(tr("KUS-PHU 功能测试上位机 V2.02.10"));
    }
    
    this->showMaximized();
    //connect(this, &QtCanPlatform::sigEndRunWork, this, &QtCanPlatform::on_recSigEndRunWork);
    
    connect(this, &QtCanPlatform::sigSendHttp, this, &QtCanPlatform::on_commitData);
    connect(this, &QtCanPlatform::sigSendPutMesData, this, &QtCanPlatform::on_TCPDataSend_MES);
    connect(this, &QtCanPlatform::sigSendPutPowData, this, &QtCanPlatform::on_sigFromThisPowerSet);
    //博世的获取版本定时器
    t_GetVer = new QTimer(this);
    autoDevMan = nullptr;
    m_bGetVer = false;
    m_bParseVer = false;
    m_usRoll = 0;
    countPeroid = 0;
    m_iSavePeroidNum = 1;
    ui.menu->setEnabled(true);
    isLogin = false;
}

QtCanPlatform::~QtCanPlatform()
{
    runStep = -1;
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
    /*if (saveData)
    {
        delete saveData;
        saveData = nullptr;
    }*/
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
    if (qlogp)
    {
        delete qlogp; qlogp = nullptr;
    }
    event->accept();
}

void QtCanPlatform::initUi()
{
   /* QString path = QApplication::applicationDirPath() + "/app-logo.ico";
    this->setWindowIcon(QIcon(path));*/
    this->setWindowIcon(QIcon(":/QtCanPlatform/app-logo.ico"));
    lostQTimer = new QTimer(this);
    connect(lostQTimer, SIGNAL(timeout()), this, SLOT(on_recTimeout()));
    //QLOG_INFO() << "初始化界面中……";
    //这个是显示内容的
    saveData = new DataSave(this);
    for (int i = 0; i < 4; i++)
    {
        saveDataArr[i] = new DataSave(this);
    }
    tableView = new QTableWidget(this);
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
    HashArr.clear();
    for (int i = 0; i < qGb->pGboleData.size(); i++)
    {
        cbSelectModel->addItem(qGb->pGboleData.at(i).modelName);
        HashArr.push_back(i);
        
    }
    if (qGb->pGboleData.size() > 0)
    {
        currentModel = 0;
        cbSelectModel->setCurrentIndex(0);
    }
       
    connect(cbSelectModel, SIGNAL(currentIndexChanged(int)), this, SLOT(on_CurrentModelChanged(int)));
    //添加个按钮
    QPushButton* pbAddModel = new QPushButton(tr("协议管理"));
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
    cbCanType->addItem(tr("选择PLIN"));
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

   /* p_dev_Handle.reset(new peakCAN);
    connect(p_dev_Handle.data(), SIGNAL(sigNewMessage(int, quint32, QByteArray)), this, SLOT(on_ReceiveData(int, quint32, QByteArray)));*/

    pHardWare = new HardWarePlin(this);
    QStringList canStr = pcanArr[0]->DetectDevice();
    cbPcan->addItems(canStr);
    /*for (int i = 0; i < canStr.size(); ++i)
    {
        cbPcan->addItem(canStr.at(i));
    }*/
    //cbPcan->addItems(p_dev_Handle->enumDevice());
    cbPcan->setMinimumWidth(100);
    QLOG_INFO() << "CAN Number:" << canStr.size();
    reFresh = new QPushButton(tr("刷新设备"));
    connect(reFresh, SIGNAL(clicked()), this, SLOT(on_pbRefreshDevice_clicked()));
    //添加常用波特率
    cbBitRate = new QComboBox();
    cbBitRate->addItem("200kb/s");
    cbBitRate->addItem("250kb/s");
    cbBitRate->addItem("500kb/s");
    cbBitRate->addItem("800kb/s");
    cbBitRate->addItem("2400b/s");
    cbBitRate->addItem("9600b/s");
    cbBitRate->addItem("10400b/s");
    cbBitRate->addItem("19200b/s");
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

    LogPb = new QPushButton(this);
    LogPb->setText(tr("报文回放&&图形化"));
    connect(LogPb, &QPushButton::clicked, this, &QtCanPlatform::on_pbLogReplay_clicked);
    //添加个水平的布局
    QHBoxLayout* hLayout = new QHBoxLayout();
    //把按钮丢进去
    
   
    hLayout->addWidget(cbCanType);
    hLayout->addWidget(cbPcan);
    hLayout->addWidget(reFresh);
    hLayout->addWidget(cbBitRate);
    hLayout->addWidget(Period);
    hLayout->addWidget(cycle);
    hLayout->addWidget(cbIsMutil);
    hLayout->addWidget(pbOpen);
    hLayout->addWidget(pbSend);
    hLayout->addWidget(cstate);
    hLayout->addWidget(communicaLabel);
   
   
    //把弹簧也丢进去
    hLayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));
    hLayout->addWidget(pbAddModel);
    hLayout->addWidget(LogPb);
    QLineEdit* debugLine = new QLineEdit("-1");
    connect(debugLine, &QLineEdit::editingFinished, this, &QtCanPlatform::on_lineEdit_editingFinished);
    QCheckBox* checkTrace = new QCheckBox();
    checkTrace->setText(tr("数据监控   "));
    connect(checkTrace, &QCheckBox::stateChanged, this, &QtCanPlatform::on_checkTraceChanged);
    QLabel* pLabel = new QLabel();
    pLabel->setText(tr("平台"));
    pLabel->setStyleSheet("font:normal bold 18px SimHei");
    QComboBox* cbPlatform = new QComboBox(this);
    cbPlatform->addItem("无分类");
    cbPlatform->addItem("3kW");
    cbPlatform->addItem("4kW");
    cbPlatform->addItem("5kW");
    cbPlatform->addItem("7kW");
    cbPlatform->addItem("10kW");
    cbPlatform->addItem("15kW");
    cbPlatform->addItem("19kW");
    cbPlatform->addItem("CAT-MID");
    cbPlatform->addItem("8kW");
    QLabel* mLabel = new QLabel();
    mLabel->setText(tr("当前型号"));
    mLabel->setStyleSheet("font:normal bold 18px SimHei");
    //hLayout->addWidget(debugLine);
   
    //hLayout->addWidget(checkTrace);
    hLayout->addWidget(pLabel);
    hLayout->addWidget(cbPlatform);
    hLayout->addWidget(mLabel);
    hLayout->addWidget(cbSelectModel);
    connect(cbPlatform, SIGNAL(currentIndexChanged(int)), this, SLOT(on_CurrentPlatformChanged(int)));
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
    //pbGetVer->setEnabled(false);
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
    //connect(dCtrl, &QDeviceCtrl::sigWorkRun, this, &QtCanPlatform::on_autoWork);
    connect(dCtrl, &QDeviceCtrl::sigCanChanged, this, &QtCanPlatform::on_pbRefreshDevice_clicked);
    m_cbOutTempMonitor = new QCheckBox(this);
    m_cbOutTempMonitor->setText("出口温度监控");
    connect(m_cbOutTempMonitor, &QCheckBox::stateChanged, this, &QtCanPlatform::on_outTempMonitor_Changed);
    pbStartAutoTest = new QPushButton(this);
    pbStartAutoTest->setText("开启自动测试");
    pbStartAutoTest->setCheckable(true);
    connect(pbStartAutoTest, &QPushButton::clicked, this, &QtCanPlatform::on_pbStartAutoTest_clicked);
    connect(this, &QtCanPlatform::sigAutoTestSend, this, &QtCanPlatform::on_processAutoTestSignal);
    pbDevicesManage = new QPushButton(this);
    pbDevicesManage->setText("设备连接管理");
    connect(pbDevicesManage, &QPushButton::clicked, this, &QtCanPlatform::on_pbDevicesManage_clicked);
    pbGeneralParameter = new QPushButton(this);
    pbGeneralParameter->setText("测试通用参数");
    connect(pbGeneralParameter, &QPushButton::clicked, this, &QtCanPlatform::on_pbGeneralParameter_clicked);
    lineEditCodeIn = new QLineEdit(this);
    connect(lineEditCodeIn, &QLineEdit::editingFinished, this, &QtCanPlatform::on_lineEditCodeIn_editingFinished,Qt::QueuedConnection);
    pbGeneralParameter->setHidden(true);
    pbSummitCode = new QPushButton(this);
    pbSummitCode->setText("提交");
    pbSummitCode->setDefault(true);
    connect(pbSummitCode, &QPushButton::clicked, this, &QtCanPlatform::on_pbSummitCode_clicked, Qt::QueuedConnection);
    tableAutoResults = new QTableWidget(this);
    QStringList autotabtlename = { "测试项","测试结果","备注"};
    tableAutoResults->setColumnCount(3);
    tableAutoResults->setHorizontalHeaderLabels(autotabtlename);
    
    QHBoxLayout* bHBoxLayout = new QHBoxLayout(this);
    bHBoxLayout->addWidget(pbStartAutoTest);
    bHBoxLayout->addWidget(pbDevicesManage);
    bHBoxLayout->addWidget(pbGeneralParameter);
    bHBoxLayout->addWidget(m_cbOutTempMonitor);
    QHBoxLayout* codename = new QHBoxLayout(this);
    codename->addWidget(new QLabel("二维码:"));
    codename->addWidget(lineEditCodeIn);
    codename->addWidget(pbSummitCode);

    

    /*QLabel* ipLabelMes = new QLabel();
    ipLabelMes->setText("Mes-IP地址");
    LineEdit_IPAddr_Mes = new QLineEdit();
    LineEdit_Port_Mes = new QLineEdit();
    LineEdit_IPAddr_Mes->setText("223.223.223.5");
    LineEdit_IPAddr_Mes->setFixedWidth(120);
    LineEdit_Port_Mes->setText("1000");
    LineEdit_Port_Mes->setFixedWidth(40);
    QLabel* portLable_Mes = new QLabel();
    portLable_Mes->setText("Mes-端口");
    pbConnectPLC_Mes = new QPushButton(this);
    pbConnectPLC_Mes->setText("连接");
    pbConnectPLC_Mes->setFixedWidth(60);
    pbConnectPLC_Mes->setCheckable(true);
    connect(pbConnectPLC_Mes, &QPushButton::clicked, this, &QtCanPlatform::on_pbMESConnect);
    QHBoxLayout* ipBoxLayout_Mes = new QHBoxLayout(this);
    ipBoxLayout_Mes->addWidget(ipLabelMes);
    ipBoxLayout_Mes->addWidget(LineEdit_IPAddr_Mes);
    ipBoxLayout_Mes->addWidget(portLable_Mes);
    ipBoxLayout_Mes->addWidget(LineEdit_Port_Mes);
    ipBoxLayout_Mes->addWidget(pbConnectPLC_Mes);
    ipLabelMes->setHidden(true);
    LineEdit_IPAddr_Mes->setHidden(true);
    portLable_Mes->setHidden(true);
    LineEdit_Port_Mes->setHidden(true);
    pbConnectPLC_Mes->setHidden(true);*/

    //右侧UI部分
    QVBoxLayout* topRightUI = new QVBoxLayout(this);
    //topRightUI->addLayout(ipBoxLayout_Mes);
    topRightUI->addLayout(bHBoxLayout);
    topRightUI->addLayout(codename);
    topRightUI->addWidget(tableAutoResults);

    
    m_cbSavePeriod = new QCheckBox(this);
    m_lePeroid = new QLineEdit(this);
    m_cbSavePeriod->setText(tr("保存周期->"));
    m_lePeroid->setText(tr("1"));
    connect(m_cbSavePeriod, &QCheckBox::stateChanged, this, &QtCanPlatform::on_savePeriodCheck_Changed);
    QHBoxLayout* clay = new QHBoxLayout(this);
    clay->addWidget(pbClearText);
    clay->addWidget(checkTrace);
    clay->addWidget(m_cbSavePeriod);
    clay->addWidget(m_lePeroid);
    //clay->addSpacerItem(new QSpacerItem(20, 10));
    //topRightUI->addLayout(clay);
    QWidget* ctx = new QWidget(this);
    ctx->setLayout(clay);
    pbClearText->setText(tr("清除日志"));
    pbClearText->setFixedWidth(80);

    QGroupBox* gp = new QGroupBox(this);
    gp->setLayout(topRightUI);

    //定时界面
    m_tCaptureTimer = new QTimer(this);
    m_iTimeStopLineEdit = new QLineEdit(this);
    m_iTimeStopLineEdit->setText("120");
    m_pbStartRad = new QPushButton(this);
    m_pbStartRad->setCheckable(true);
    m_labShowTime = new QLabel(this);
    m_labShowTime->setText(tr("运行时间：0秒"));
    connect(m_tCaptureTimer, &QTimer::timeout, this, &QtCanPlatform::on_CapturePower);
    QLabel* qlabInputCapTimer = new QLabel(this);
    qlabInputCapTimer->setText(tr("捕获时间"));
    QHBoxLayout* layCap = new QHBoxLayout(this);
    layCap->addWidget(qlabInputCapTimer);
    layCap->addWidget(m_iTimeStopLineEdit);
    layCap->addWidget(m_pbStartRad);
    layCap->addWidget(m_labShowTime);
    m_pbStartRad->setText(tr("启动记录"));
    QWidget* wg = new QWidget(this);
    //根据需要，启动不同的界面
    if (m_iShowType)
    {
       // bootomright->addWidget(dCtrl);
        m_iTimeStopLineEdit->setHidden(true);
        m_pbStartRad->setHidden(true);
        m_labShowTime->setHidden(true);
        qlabInputCapTimer->setHidden(true);
        
    }
    else
    {
        
        wg->setLayout(layCap);
        bootomright->addWidget(wg);
    }
    
   // bootomright->addWidget(ctx);
    bootomright->addWidget(gp);
    bootomright->addWidget(ctx);
    
    bootomright->addWidget(textBrowser);
    bootomright->setStretchFactor(0, 8);
    bootomright->setStretchFactor(1, 1);
    bootomright->setStretchFactor(2, 6);
    
    mainBottom->setStretchFactor(0, 11);
    mainBottom->setStretchFactor(1, 3);
    QGridLayout* gg = new QGridLayout();
    gg->addWidget(mainQSpli);
    ui.centralWidget->setLayout(gg);
    ui.menu->setEnabled(true);

   
    //增加about，历史版本，关于
    initAutoResTableWidget();
    canSeting = nullptr;

}
void QtCanPlatform::initAutoResTableWidget()
{
    tableAutoResults->clearContents();
    testItemList.clear();
    testItemList << "MES允许入站" << "通信测试" << "软件版本号" << "欠压保护" << "过压保护" << "额定功率" << "过温保护" << "过温恢复" << "其它故障" << "测试结果";
    tableAutoResults->setRowCount(testItemList.size());
    for (int m = 0; m < testItemList.size(); m++)
        tableAutoResults->setItem(m, 0, new QTableWidgetItem(testItemList.at(m)));
}
void QtCanPlatform::on_action_About_triggered()
{
   
    QFile F(QApplication::QCoreApplication::applicationDirPath() + "/Data/about.kus");
    if (F.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::about(this, tr("关于"), QString(F.readAll()));
    }
    else
    {
        QMessageBox::about(this, tr("关于"), "CAN功能测试上位机");
    }
    

}
void QtCanPlatform::on_action_History_triggered()
{
    QFile F(QApplication::QCoreApplication::applicationDirPath() + "/Data/history_version.kus");
    if (F.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::about(this, tr("历史版本"),QString(F.readAll()));
    }
    else
    {
        QMessageBox::about(this, tr("历史版本"), "历史版本丢失");
    }
}
void QtCanPlatform::on_pbLogReplay_clicked()
{
    int realIndex = HashArr.at(cbSelectModel->currentIndex());
    if (!qlogp)
    {
        qlogp = new QLogPlot(realIndex,this);
        qlogp->setWindowFlags(qlogp->windowFlags() | Qt::Tool);
        connect(qlogp, &QLogPlot::sigNewMessage, this, QOverload<uint, QByteArray>::of(&QtCanPlatform::on_ReceiveData));
        connect(this, &QtCanPlatform::sigNewMessageToGraph, qlogp, &QLogPlot::proLogData);
        connect(qlogp, &QLogPlot::sigCloseWindow, this, &QtCanPlatform::on_plotWindowCLose);
    }
    qlogp->setModelIndex(realIndex);
    qlogp->show();
   
    if (LogPb) LogPb->setEnabled(false);
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
    currentTestModel = qGb->pGboleData.at(currentModel);
    bStandard = pTemp.bStandardId;
    //canIdData cTemp;
    cbBitRate->setCurrentIndex(pTemp.bundRate);
    cycle->setText(QString::number(pTemp.circle));
    for (int i = 0; i < pTemp.cItem.size(); i++)
    {
        //取出操作为发送的信号
        if (1 == pTemp.cItem.at(i).opt)
        {
            sendCanData.push_back(pTemp.cItem.at(i));
            sendCanData.at(sendCanData.size() - 1).timeAdd = pTemp.cItem.at(i).timeAdd;
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
    showTableVec.clear();
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
            if (ch == 0)
            {
                recCanData.push_back(pTemp.cItem.at(i));
                //pTemp.cItem.at(i).CanId
                showTableData dd;
                for (int mk = 0; mk < pTemp.cItem.at(i).pItem.size();mk++)
                {
                    parseData p;
                    p.name = pTemp.cItem.at(i).pItem.at(mk).bitName;
                    p.toWord = "0";
                    p.value = 0;
                    p.color.b = 255;
                    p.color.g = 255;
                    p.color.r = 255;
                    dd.Pdata.push_back(p);
                }
                dd.IdName = QString::number(pTemp.cItem.at(i).strCanId.toLong(nullptr,16));
                showTableVec.push_back(dd);
            }
                
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
    
    {
        for (int i = 0; i < sendCanData.size(); i++)
        {
        
            if (!sendCanData.at(i).isSend)
                if (!m_bGetVer)
                    continue;
            if (multiCircle > 1)
            {
                QTimer* t= dynamic_cast<QTimer*> (sender());
                if (sendCanData.at(i).timeAdd != t)
                    continue;
            }
                
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
                if (cbIsMutil->currentIndex() == 0)
                {
                    pcan->SendFrame(fream_id, s_Data, bStandard);
                    //p_dev_Handle->sendData(0, fream_id, s_Data);
                    
                }    
                else
                {
                    for(int i=0;i<4;i++)
                        if(pcanArr[i]->IsOpen())
                           pcanArr[i]->SendFrame(fream_id, s_Data, bStandard);
                }
            
            }
            
            else if (cbCanType->currentIndex() == 1)
            {
                kcan->canSendAll(fream_id, s_Data);
                
            }
            else if (cbCanType->currentIndex() == 2)
            {
                canayst->sendData(fream_id, s_Data, bStandard);
                
            }
            else if (cbCanType->currentIndex() == 3)
            {
                int other[2];
                other[0] = sendCanData.at(i).len;
                other[1] = 0;
                pHardWare->SendMessage(fream_id, s_Data, other);
                
            }
            //_sleep(20);
            QByteArray bydd;
            for (int h = 0; h < 8; h++)
            {
                bydd.append(s_Data[h]);
            }
            int period = cycle->text().toInt();

            //防止数值大，图形曲线刷新太快
            if (period >= 1000)
            {
                timeStmp_send += (period / 1000);
            }
            else if (period >= 100)
            {
                timeStmp_send += (period / 100);
            }
            else if (period >= 10)
            {
                timeStmp_send = period / 10;
            }
            else
                timeStmp_send = period;
            emit sigNewMessageToGraph(fream_id, bydd, timeStmp_send);
        }

    }
    if (cbCanType->currentIndex() == 3)
    {
        for (int k = 0; k < recCanData.size(); k++)
        {
            int other[2];
            other[0] = recCanData.at(k).len;
            other[1] = 1;
            pHardWare->SendMessage(recCanData.at(k).strCanId.toInt(NULL,16), s_Data, other);
        }
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
            sendCanData.at(k).pItem.at(i - cnum).send = tableView->item(i, 2)->text().toFloat();


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
    for (int i = 0; i < cdata.pItem.size(); i++)
    {
        const protoItem &itemp = cdata.pItem.at(i);
        int startbyte = itemp.startByte;
        int startbit = itemp.startBit;
        int lengght = itemp.bitLeng;
        int senddd = (int)(itemp.send * itemp.precision+itemp.offset);
        if (itemp.dataFrom == "CRC")
        {
            crcTemp = cdata.pItem.at(i);
            crc = true;
        }
        startbyte = YB::InRang(0, 7, startbyte);
        if (itemp.dataFrom.contains("++"))
        {
            int iTemp = itemp.dataFrom.mid(2).toInt();
            if (m_usRoll > iTemp)m_usRoll = 0;
            data[startbyte] = m_usRoll++;
            continue;
        }
        
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
    for (int i = 0; i < cdata.pItem.size(); i++)
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
    rw_Lock.lockForWrite();
 
    bool id_in = false;
    for (int i = 0; i < recCanData.size(); i++)
    {
        uint currID = recCanData.at(i).strCanId.toUInt(NULL, 16);
        if (currID != fream_id)
            continue;
        m_bCommunication++;
        id_in = true;
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
           
            
            //这个字段的数据来源其它字段的乘或除
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
            //CRC校验的异或
            else if (datafrom.contains("XOR"))
            {
                (uint8_t)data[m];
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

        getByteInfo(parseArr, 0);

        //判断map里面是否已经存在有了
        //if (YB::keyInMap(showTableD, QString::number(fream_id)))
        //{
        //    showTableD[QString::number(fream_id)] = parseArr;
        //}
        //else
        //{
        //    showTableD.insert({ QString::number(fream_id),parseArr });
        //    //tableArray[0]->clearContents();
        //}
        int i_index = YB::idNameInVector(showTableVec, QString::number(fream_id));
        if (i_index >= 0)
        {
            showTableVec.at(i_index).Pdata = parseArr;
        }
        else
        {
            showTableData temp;
            temp.IdName = QString::number(fream_id);
            temp.Pdata = parseArr;
            showTableVec.push_back(temp);
        }


    }
    m_bResetfault = true;
    rw_Lock.unlock();
    if (!id_in)return;
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

   // std::map<QString, std::vector<parseData>>::iterator iBegin = showTableD.begin();
    //std::map<QString, std::vector<parseData>>::iterator iEnd = showTableD.end();
    std::vector<showTableData>::iterator iBeginV = showTableVec.begin();
    std::vector<showTableData>::iterator iEndV = showTableVec.end();
    int cr = 0;
    
    while (iBeginV != iEndV)
    {
        
        //每加一行就要设置到表格去
        //int num = iBegin->second.size();
        int num = iBeginV->Pdata.size();
        
        int idex = 0;
        int j = 0;
        for (j = 0; j < num; j++, idex++)
        {
            if (idex > 9)   //一行最多放10个数据
            {
                //满10个，从头开始
                idex = 0;
                //下一行的下一行，也就是隔一行，要加2；
                cr += 2;
            } 
            tableArray[0]->resizeRowToContents(cr + 1);
            
            QString tnamp = iBeginV->Pdata.at(j).name;
            QString toword = iBeginV->Pdata.at(j).toWord;
            
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
                b->setBackgroundColor(QColor(iBeginV->Pdata.at(j).color.r, iBeginV->Pdata.at(j).color.g, iBeginV->Pdata.at(j).color.b));
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
        if (idex < 9)
        {
            QString tnamp = "";
            QString toword = "";
            for (; idex <= 9; idex++)
            {
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
                   
                }
                catch (const std::exception& e)
                {
                    QLOG_INFO() << "Error:" << e.what();
                }
            }
        }
        cr += 2;
        iBeginV++;

    }
    dTemp.remove(dTemp.size() - 1, 1);
    
    uint nn = (dd.toMSecsSinceEpoch() - lastTime.toMSecsSinceEpoch());
    lastTime = dd;
    

    //防止一个周期内来一帧就保存一次，一个周期内相邻帧之间的间隔应该不会超过50ms
    if (nn > 50)
    {
        countPeroid++;
        if(countPeroid>= m_iSavePeroidNum)
        {
            strSaveList.append(dTemp); //放到一个list存着
            countPeroid = 0;
        }
    }
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
    m_strVerCanMessage = binaryStr;
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
    bool id_in = false;
    rw_Lock.lockForWrite();
    for (int i = 0; i < recCanData.size(); i++)
    {
        uint currID = recCanData.at(i).strCanId.toUInt(NULL, 16);
        if (currID != fream_id)
            continue;
        id_in = true;
        m_bCommunication++;
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
            else if (datafrom.contains("XOR"))
            {
                (uint8_t)data[m];
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
            //pd.toWord = QString::number(temp,'g',2);
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
           /* for (int i = 0; i < ss.size(); i++)
            {
                if (ss.at(i).value.toInt() == temp)
                {
                    pd.color.r = ss.at(i).r;
                    pd.color.g = ss.at(i).g;
                    pd.color.b = ss.at(i).b;
                    pd.toWord = ss.at(i).toWord;
                    break;
                }

            }*/
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
                Error[0].insert(pd.toWord);
            }
        }
        getByteInfo(parseArr, 0);

        int i_index = YB::idNameInVector(showTableVec, QString::number(fream_id));
        if (i_index >= 0)
        {
            showTableVec.at(i_index).Pdata = parseArr;
        }
        else
        {
            showTableData temp;
            temp.IdName = QString::number(fream_id);
            temp.Pdata = parseArr;
            showTableVec.push_back(temp);
        }
    }
    m_bResetfault = true;
    rw_Lock.unlock();
    if (!id_in)return;
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

    std::vector<showTableData>::iterator iBeginV = showTableVec.begin();
    std::vector<showTableData>::iterator iEndV = showTableVec.end();
    int cr = 0;
    while (iBeginV != iEndV)
    {

        //每加一行就要设置到表格去
        int num = iBeginV->Pdata.size();
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
            QString tnamp = iBeginV->Pdata.at(j).name;
            QString toword = iBeginV->Pdata.at(j).toWord;
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
                b->setBackgroundColor(QColor(iBeginV->Pdata.at(j).color.r, iBeginV->Pdata.at(j).color.g, iBeginV->Pdata.at(j).color.b));
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
        if (idex < 9)
        {
            QString tnamp = "";
            QString toword = "";
            for (; idex <= 9; idex++)
            {
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

                }
                catch (const std::exception& e)
                {
                    QLOG_INFO() << "Error:" << e.what();
                }
            }
        }
        cr += 2;
        iBeginV++;

    }
    dTemp.remove(dTemp.size() - 1, 1);

    uint nn = (dd.toMSecsSinceEpoch() - lastTime.toMSecsSinceEpoch());
    lastTime = dd;
    //防止一个周期内来一帧就保存一次
    if (nn > 50)
    {
        countPeroid++;
        if (countPeroid >= m_iSavePeroidNum)
        {
            strSaveList.append(dTemp); //放到一个list存着
            countPeroid = 0;
        }
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
    m_strVerCanMessage = binaryStr;
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
    rw_Lock.lockForWrite();
    for (int i = 0; i < recCanData.size(); i++)
    {
        uint currID = recCanData.at(i).strCanId.toUInt(NULL, 16);
        if (currID != fream_id)
            continue;
        m_bCommunication++;
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
            else if (datafrom.contains("XOR"))
            {
                (uint8_t)data[m];
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
            //pd.toWord = QString::number(temp,'g',2);
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
                //rw_Lock.lockForWrite();
                Error[ch].insert(pd.toWord);
                //rw_Lock.unlock();
            }
        }
        getByteInfo(parseArr, ch);


        int i_index = YB::idNameInVector(showTableVec, QString::number(fream_id));
        if (i_index >= 0)
        {
            showTableVec.at(i_index).Pdata = parseArr;
        }
        else
        {
            showTableData temp;
            temp.IdName = QString::number(fream_id);
            temp.Pdata = parseArr;
            showTableVec.push_back(temp);
        }

    }
    m_bResetfault = true;
    rw_Lock.unlock();
    if (!tableArray[ch])
        return;


          //=================10.10===============
    QString dTime = QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss-zzz");
    QDateTime dd = QDateTime::currentDateTime();
    uint curCount = tableRollDataArray[ch]->rowCount() + 1;

    QString dTemp = QString::number(curCount) + "," + dTime + ",";

    /*std::map<QString, std::vector<parseData>>::iterator iBegin = showTableD.begin();
    std::map<QString, std::vector<parseData>>::iterator iEnd = showTableD.end();*/
    std::vector<showTableData>::iterator iBeginV = showTableVec.begin();
    std::vector<showTableData>::iterator iEndV = showTableVec.end();
    int cr = 0;
    while (iBeginV != iEndV)
    {

        //每加一行就要设置到表格去
        int num = iBeginV->Pdata.size();
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

            QString tnamp = iBeginV->Pdata.at(j).name;
            QString toword = iBeginV->Pdata.at(j).toWord;
            tableArray[ch]->setItem(cr, idex, new QTableWidgetItem(tnamp));
            QFont ff;
            ff.setBold(true);
            tableArray[ch]->item(cr, idex)->setFont(ff);
            tableArray[ch]->item(cr, idex)->setTextAlignment(Qt::AlignCenter);
            tableArray[ch]->item(cr, idex)->setBackgroundColor(recBackgroudColor);
            tableArray[ch]->item(cr, idex)->setForeground(QBrush(recFontColor));
            tableArray[ch]->setItem(cr + 1, idex, new QTableWidgetItem(toword));
            tableArray[ch]->item(cr + 1, idex)->setBackgroundColor(QColor(iBeginV->Pdata.at(j).color.r, iBeginV->Pdata.at(j).color.g, iBeginV->Pdata.at(j).color.b));
            tableArray[ch]->item(cr + 1, idex)->setTextAlignment(Qt::AlignCenter);
            dTemp += toword + ",";
        }
        if (idex < 9)
        {
            QString tnamp = "";
            QString toword = "";
            for (; idex <= 9; idex++)
            {
                try
                {
                    tableArray[ch]->setItem(cr, idex, new QTableWidgetItem(tnamp));
                    QFont ff;
                    ff.setBold(true);
                    QTableWidgetItem* b = tableArray[ch]->item(cr, idex);
                    if (!b)
                        continue;
                    b->setFont(ff);
                    b = tableArray[ch]->item(cr, idex);
                    if (!b)
                        continue;
                    b->setTextAlignment(Qt::AlignCenter);
                    b = tableArray[ch]->item(cr, idex);
                    if (!b)
                        continue;
                    b->setBackgroundColor(recBackgroudColor);
                    b = tableArray[ch]->item(cr, idex);
                    if (!b)
                        continue;
                    b->setForeground(QBrush(recFontColor));
                    tableArray[ch]->setItem(cr + 1, idex, new QTableWidgetItem(toword));
                    b = tableArray[ch]->item(cr + 1, idex);
                    if (!b)
                        continue;

                }
                catch (const std::exception& e)
                {
                    QLOG_INFO() << "Error:" << e.what();
                }
            }
        }
        cr += 2;
        iBeginV++;

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
                    countPeroid++;
                    if(countPeroid>= m_iSavePeroidNum)
                    {
                        multReceData[ch].append(dTemp);
                        countPeroid = 0;
                    }
                }
            }
            else
            {
                countPeroid++;
                if (countPeroid >= m_iSavePeroidNum)
                {
                    multReceData[ch].append(dTemp);
                    countPeroid = 0;
                }
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
    m_strVerCanMessage = binaryStr;
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
    rw_Lock.lockForWrite();
    for (int i = 0; i < recCanData.size(); i++)
    {
        uint currID = recCanData.at(i).strCanId.toUInt(NULL, 16);
        if (currID != fream_id)
            continue;
        m_bCommunication++;
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
            else if (datafrom.contains("XOR"))
            {
                (uint8_t)data[m];
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
        getByteInfo(parseArr, ch);
        //判断map里面是否已经存在有了

        int i_index = YB::idNameInVector(showTableVec, QString::number(fream_id));
        if (i_index >= 0)
        {
            showTableVec.at(i_index).Pdata = parseArr;
        }
        else
        {
            showTableData temp;
            temp.IdName = QString::number(fream_id);
            temp.Pdata = parseArr;
            showTableVec.push_back(temp);
        }

    }
    m_bResetfault = true;
    rw_Lock.unlock();
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

    /*std::map<QString, std::vector<parseData>>::iterator iBegin = showTableD.begin();
    std::map<QString, std::vector<parseData>>::iterator iEnd = showTableD.end();*/

    std::vector<showTableData>::iterator iBeginV = showTableVec.begin();
    std::vector<showTableData>::iterator iEndV = showTableVec.end();
    int cr = 0;
    while (iBeginV != iEndV)
    {
        int num = iBeginV->Pdata.size();
        int idex = 0;
        for (int j = 0; j < num; j++, idex++)
        {
            if (idex > 9)   //每行10列
            {
                idex = 0;
                cr += 2;
            }

            QString tnamp = iBeginV->Pdata.at(j).name;
            QString toword = iBeginV->Pdata.at(j).toWord;
            tableArray[ch]->setItem(cr, idex, new QTableWidgetItem(tnamp));
            QFont ff;
            ff.setBold(true);
            tableArray[ch]->item(cr, idex)->setFont(ff);
            tableArray[ch]->item(cr, idex)->setTextAlignment(Qt::AlignCenter);
            tableArray[ch]->item(cr, idex)->setBackgroundColor(recBackgroudColor);
            tableArray[ch]->item(cr, idex)->setForeground(QBrush(recFontColor));
            tableArray[ch]->setItem(cr + 1, idex, new QTableWidgetItem(toword));
            tableArray[ch]->item(cr + 1, idex)->setBackgroundColor(QColor(iBeginV->Pdata.at(j).color.r, iBeginV->Pdata.at(j).color.g, iBeginV->Pdata.at(j).color.b));
            tableArray[ch]->item(cr + 1, idex)->setTextAlignment(Qt::AlignCenter);
            dTemp += toword + ",";
        }
        if (idex < 9)
        {
            QString tnamp = "";
            QString toword = "";
            for (; idex <= 9; idex++)
            {
                try
                {
                    tableArray[ch]->setItem(cr, idex, new QTableWidgetItem(tnamp));
                    QFont ff;
                    ff.setBold(true);
                    QTableWidgetItem* b = tableArray[ch]->item(cr, idex);
                    if (!b)
                        continue;
                    b->setFont(ff);
                    b = tableArray[ch]->item(cr, idex);
                    if (!b)
                        continue;
                    b->setTextAlignment(Qt::AlignCenter);
                    b = tableArray[ch]->item(cr, idex);
                    if (!b)
                        continue;
                    b->setBackgroundColor(recBackgroudColor);
                    b = tableArray[ch]->item(cr, idex);
                    if (!b)
                        continue;
                    b->setForeground(QBrush(recFontColor));
                    tableArray[ch]->setItem(cr + 1, idex, new QTableWidgetItem(toword));
                    b = tableArray[ch]->item(cr + 1, idex);
                    if (!b)
                        continue;

                }
                catch (const std::exception& e)
                {
                    QLOG_INFO() << "Error:" << e.what();
                }
            }
        }
        cr += 2;
        iBeginV++;

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
                    countPeroid++;
                    if (countPeroid >= m_iSavePeroidNum)
                    {
                        multReceData[ch].append(dTemp);
                        countPeroid = 0;
                    }
                }
            }
            else
            {
                countPeroid++;
                if (countPeroid >= m_iSavePeroidNum)
                {
                    multReceData[ch].append(dTemp);
                    countPeroid = 0;
                }
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

void QtCanPlatform::getByteInfo(const std::vector<parseData>& parseArr,int ch)
{
    for (auto& x : parseArr)
    {

        if (x.name == "功率W" || x.name == "功率kW" || x.name == "功率" || x.name == "功率(W)")
        {
            realPower[ch] = x.value;
        }
        else if (x.name == "电压V" || x.name == "电压(V)")
        {
            realVolt[ch] = x.value;
        }
        else if (x.name == "出口温度°C" || x.name == "出口水温度°C" || x.name == "出口温度(°C)" || x.name == "出口水温度(°C)")
        {
            realWTemp[ch] = x.value;
        }
        else if (x.name == "高压异常故障" || x.name=="高压指示"|| x.name == "高压异常")
        {
            realHVErr[ch] = x.toWord;
        }
        else if (x.name == "过温保护" || x.name == "过温故障" )
        {
            realOTPro[ch] = x.toWord;
        }
        else if (x.name == "故障代码")
        {
            if (x.toWord == "高压过压" || x.toWord == "高压欠压" || x.toWord == "欠压" || x.toWord == "过压"|| x.toWord == "高压异常" || x.toWord == "高压故障")
            {
                realHVErr[ch] = x.toWord;
            }
            else if (x.toWord != "高压过压" && x.toWord != "高压欠压" && x.toWord != "欠压" && x.toWord != "过压" && x.toWord != "高压异常" && x.toWord != "高压故障")
            {
                realHVErr[ch] = "正常";
            }
            if (x.toWord == "过温保护" || x.toWord == "过温故障" || x.toWord == "过温" || x.toWord == "出水口过温")
            {
                realOTPro[ch] = x.toWord;
            }
            else if (x.toWord != "过温保护" && x.toWord != "过温故障" && x.toWord != "过温" && x.toWord != "出水口过温")
            {
                realOTPro[ch] = "正常";
            }
        }
    }
}
unsigned char QtCanPlatform::crc_high_first(const uchar data[], unsigned char len)
{
    //    unsigned char i;
    unsigned char crc = 0xFF; /* 计算的初始crc值 */
   const  uchar* ptr = &data[1];
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
unsigned char QtCanPlatform::checksumXOR(const uchar data[])
{
    return (data[1] ^ data[2] ^ data[3] ^ data[4] ^ data[5] ^ data[6] ^ data[7]);
}
void QtCanPlatform::getModelTitle()
{

    rollTitle;
}
void QtCanPlatform::on_CurrentModelChanged(int index)
{
    timeStmp = 0;
    timeStmp_send = 0;
    currentModel = HashArr.at(index);
    sendDataIntoTab();
    recDataIntoTab();
}
void QtCanPlatform::on_CurrentPlatformChanged(int index)
{
    timeStmp = 0;
    timeStmp_send = 0;
    HashArr.clear();
    if (index < 0)
        return;
    qGboleData* qGb = qGboleData::getInstance();
    if (!qGb)return;
    disconnect(cbSelectModel, SIGNAL(currentIndexChanged(int)), this, SLOT(on_CurrentModelChanged(int)));
    cbSelectModel->clear();
    for (int i = 0; i < qGb->pGboleData.size(); i++)
    {
       
        if (0 == index)
        {
            HashArr.push_back(i);
            cbSelectModel->addItem(qGb->pGboleData.at(i).modelName);
        }
        else
        {
            QStringList ls;
            switch (index)
            {
                case 1:
                    ls = qGb->pGboleData.at(i).sPlatform.split(",");
                    for(int k=0;k<ls.size();k++)
                        if (ls.at(k)=="3kW" || ls.at(k)=="Non")
                        {
                            HashArr.push_back(i);
                            cbSelectModel->addItem(qGb->pGboleData.at(i).modelName);
                            break;
                        }
                    break;
                case 2:
                    ls = qGb->pGboleData.at(i).sPlatform.split(",");
                    for (int k = 0; k < ls.size(); k++)
                        if (ls.at(k) == "4kW" || ls.at(k) == "Non")
                        {
                            HashArr.push_back(i);
                            cbSelectModel->addItem(qGb->pGboleData.at(i).modelName);
                            break;
                        }
                    break;
                case 3:
                    ls = qGb->pGboleData.at(i).sPlatform.split(",");
                    for (int k = 0; k < ls.size(); k++)
                        if (ls.at(k) == "5kW" || ls.at(k) == "Non")
                        {
                            HashArr.push_back(i);
                            cbSelectModel->addItem(qGb->pGboleData.at(i).modelName);
                            break;
                        }
                    break;
                case 4:
                    ls = qGb->pGboleData.at(i).sPlatform.split(",");
                    for (int k = 0; k < ls.size(); k++)
                        if (ls.at(k) == "7kW" || ls.at(k) == "Non")
                        {
                            HashArr.push_back(i);
                            cbSelectModel->addItem(qGb->pGboleData.at(i).modelName);
                            break;
                        }
                    break;
                case 5:
                    ls = qGb->pGboleData.at(i).sPlatform.split(",");
                    for (int k = 0; k < ls.size(); k++)
                        if (ls.at(k) == "10kW" || ls.at(k) == "Non")
                        {
                            HashArr.push_back(i);
                            cbSelectModel->addItem(qGb->pGboleData.at(i).modelName);
                            break;
                        }
                    break;
                case 6:
                    ls = qGb->pGboleData.at(i).sPlatform.split(",");
                    for (int k = 0; k < ls.size(); k++)
                        if (ls.at(k) == "15kW" || ls.at(k) == "Non")
                        {
                            HashArr.push_back(i);
                            cbSelectModel->addItem(qGb->pGboleData.at(i).modelName);
                            break;
                        }
                    break;
                case 7:
                    ls = qGb->pGboleData.at(i).sPlatform.split(",");
                    for (int k = 0; k < ls.size(); k++)
                        if (ls.at(k) == "19kW" || ls.at(k) == "Non")
                        {
                            HashArr.push_back(i);
                            cbSelectModel->addItem(qGb->pGboleData.at(i).modelName);
                            break;
                        }
                    break;
                case 8:
                    ls = qGb->pGboleData.at(i).sPlatform.split(",");
                    for (int k = 0; k < ls.size(); k++)
                        if (ls.at(k) == "CAT-MID")
                        {
                            HashArr.push_back(i);
                            cbSelectModel->addItem(qGb->pGboleData.at(i).modelName);
                            break;
                        }
                    break;
                case 9:
                    ls = qGb->pGboleData.at(i).sPlatform.split(",");
                    for (int k = 0; k < ls.size(); k++)
                        if (ls.at(k) == "8kW")
                        {
                            HashArr.push_back(i);
                            cbSelectModel->addItem(qGb->pGboleData.at(i).modelName);
                            break;
                        }
                    break;
            default:
                HashArr.push_back(i);
                cbSelectModel->addItem(qGb->pGboleData.at(i).modelName);
                break;
            }
        }
    }
    if(cbSelectModel->count()>0)
        on_CurrentModelChanged(0);
    connect(cbSelectModel, SIGNAL(currentIndexChanged(int)), this, SLOT(on_CurrentModelChanged(int)));
}
void QtCanPlatform::on_cbCanType_currentIndexChanged(int index)
{
    cbPcan->clear();
    on_pbRefreshDevice_clicked();
}
void QtCanPlatform::on_commitData(const QByteArray& byte, int id)
{
    qGboleData* gb = qGboleData::getInstance();
    if (!gb)
        return;
    struct autoTestData at = gb->getATData();
    mHttp mp(this);
    QString status = mp.sendHttpSys(byte, at.m_sOutWebAddr);
    if (status.isEmpty() || status.at(0) != "Y")
    {
        QMessageBox::warning(this, tr("警告"), tr("上传失败，没有找到相关设备所在工位信息"));
        QLOG_INFO() << "工位-" << id << ":上传数据失败";
        //ui.dCbProcess3->setChecked(false);
    }
    else if (status.at(0) == "Y")
    {
        //QMessageBox::about(this, tr("提示"), tr("出站成功"));
        //ui.dCbProcess3->setChecked(true);
        QLOG_INFO() << "工位-"<<id<<":上传数据成功";
    }
}
void QtCanPlatform::on_pbSend_clicked(bool clicked)
{

    if (clicked)
    {
        m_usRoll = 0;
        qGboleData* qGb = qGboleData::getInstance();
        if (!qGb)return;
        
        int index = cbSelectModel->currentIndex();

        if (index > qGb->pGboleData.size() - 1)
            return;
        int real = HashArr[index];

        multiCircle = 0;
        for (int k = 0; k < qGb->pGboleData.at(real).cItem.size(); k++)
            if (qGb->pGboleData.at(real).cItem.at(k).isSend && qGb->pGboleData.at(real).cItem.at(k).circle != -1) multiCircle++;
        if (multiCircle > 1)
        {
            for (int k = 0; k < qGb->pGboleData.at(real).cItem.size(); k++)
                if (qGb->pGboleData.at(real).cItem.at(k).isSend && qGb->pGboleData.at(real).cItem.at(k).circle != -1)
                {
                    QTimer* t = new QTimer(this);
                    t->start(qGb->pGboleData.at(real).cItem.at(k).circle);
                    qGb->pGboleData.at(real).cItem.at(k).timeAdd = t;
                    connect(t, &QTimer::timeout, this, &QtCanPlatform::sendData);
                }
        }
        else
        {
            _period_ = cycle->text().toInt();
            if (_period_ < 50)
                _period_ = 50;
            sendTimer->start(_period_);
        }
        
        cycle->setEnabled(false);
        cbBitRate->setEnabled(false);
        reFresh->setEnabled(false);
        runTimed = m_iTimeStopLineEdit->text().trimmed().toInt();
        m_labShowTime->setText("运行时间：0秒");
        if (m_pbStartRad->isChecked())
        {
            m_tCaptureTimer->start(1000);
        }
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

       
        if (0 == qGb->pGboleData.at(real).agreement)
        {
            pcan->setProperty("isExtend", true);
        }
        else
        {
            pcan->setProperty("isExtend", false);
        }
        sendDataIntoTab();
        if (m_cbOutTempMonitor && m_cbOutTempMonitor->isChecked())
        {
            this->on_pbStartAutoTest_clicked(true);
            this->pbStartAutoTest->setChecked(true);
        }
    }
    else
    {
        if(multiCircle>1)
        {
            qGboleData* qGb = qGboleData::getInstance();
            if (!qGb)return;
            int index = cbSelectModel->currentIndex();

            if (index > qGb->pGboleData.size() - 1)
                return;
            int real = HashArr[index];
            for (int k = 0; k < qGb->pGboleData.at(real).cItem.size(); k++)
                if (qGb->pGboleData.at(real).cItem.at(k).isSend && qGb->pGboleData.at(real).cItem.at(k).circle != -1)
                {
                    if(qGb->pGboleData.at(real).cItem.at(k).timeAdd)
                        qGb->pGboleData.at(real).cItem.at(k).timeAdd->stop();
                }
        }
        sendTimer->stop();
        m_tCaptureTimer->stop();
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
        if (m_cbOutTempMonitor && m_cbOutTempMonitor->isChecked())
        {
            this->on_pbStartAutoTest_clicked(false);
            this->pbStartAutoTest->setChecked(false);
        }
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
    else if (cbCanType->currentIndex() == 3)
    {
        QStringList n = pHardWare->GetHardWare();
        for (int m = 0; m < n.size(); m++)
        {
            cbPcan->addItem(n.at(m));
        }
    }
    
}
void QtCanPlatform::on_pbOpenPcan_clicked()
{
    if (cbCanType->currentIndex() == 0)
    {
        if (pcanIsOpen)
        {
            pcan->CloseCan();
            //p_dev_Handle->disConnect();
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
            if (multiCircle > 1)
            {
                qGboleData* qGb = qGboleData::getInstance();
                if (!qGb)return;
                int index = cbSelectModel->currentIndex();

                if (index > qGb->pGboleData.size() - 1)
                    return;
                int real = HashArr[index];
                for (int k = 0; k < qGb->pGboleData.at(real).cItem.size(); k++)
                    if (qGb->pGboleData.at(real).cItem.at(k).isSend && qGb->pGboleData.at(real).cItem.at(k).circle != -1)
                    {
                        if (qGb->pGboleData.at(real).cItem.at(k).timeAdd)
                            qGb->pGboleData.at(real).cItem.at(k).timeAdd->stop();
                    }
            }
            reFresh->setEnabled(true);
            communicaLabel->setText(tr("待机中..."));
            cbSelectModel->setEnabled(true);
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
            bundRate = bitRate;
            if (1 == cbIsMutil->currentIndex())
            {
                for (int i = 0; i < 4 && i < cbPcan->count(); i++)
                       b |= pcanArr[i]->ConnectDevice(pcanArr[i]->m_interfaces.at(i).name(), bitRate);
            }
            else if (0 == cbIsMutil->currentIndex())
            {
                b = pcan->ConnectDevice(cbPcan->currentText(), bitRate);
                
                //b = p_dev_Handle->openDevice(json);
               
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
            cbSelectModel->setEnabled(false);
            
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
            cbSelectModel->setEnabled(true);
            if (multiCircle > 1)
            {
                qGboleData* qGb = qGboleData::getInstance();
                if (!qGb)return;
                int index = cbSelectModel->currentIndex();

                if (index > qGb->pGboleData.size() - 1)
                    return;
                int real = HashArr[index];
                for (int k = 0; k < qGb->pGboleData.at(real).cItem.size(); k++)
                    if (qGb->pGboleData.at(real).cItem.at(k).isSend && qGb->pGboleData.at(real).cItem.at(k).circle != -1)
                    {
                        if (qGb->pGboleData.at(real).cItem.at(k).timeAdd)
                            qGb->pGboleData.at(real).cItem.at(k).timeAdd->stop();
                    }
            }
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
            bundRate = bitRate;
            bool b = pcan->ConnectDevice(cbPcan->currentIndex(), bitRate);
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
            cbSelectModel->setEnabled(false);
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
            cbSelectModel->setEnabled(true);
            if (multiCircle > 1)
            {
                qGboleData* qGb = qGboleData::getInstance();
                if (!qGb)return;
                int index = cbSelectModel->currentIndex();

                if (index > qGb->pGboleData.size() - 1)
                    return;
                int real = HashArr[index];
                for (int k = 0; k < qGb->pGboleData.at(real).cItem.size(); k++)
                    if (qGb->pGboleData.at(real).cItem.at(k).isSend && qGb->pGboleData.at(real).cItem.at(k).circle != -1)
                    {
                        if (qGb->pGboleData.at(real).cItem.at(k).timeAdd)
                            qGb->pGboleData.at(real).cItem.at(k).timeAdd->stop();
                    }
            }
            
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
            bundRate = bitRate;
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
            cbSelectModel->setEnabled(false);
        }
    }
    else if (cbCanType->currentIndex() == 3)
    {
        if (!pHardWare)
        {
            pHardWare = new HardWarePlin(this);
        }
        if (pcanIsOpen)
        {
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
            pHardWare->CloseHardWare();
            cbSelectModel->setEnabled(true);
            if (multiCircle > 1)
            {
                qGboleData* qGb = qGboleData::getInstance();
                if (!qGb)return;
                int index = cbSelectModel->currentIndex();

                if (index > qGb->pGboleData.size() - 1)
                    return;
                int real = HashArr[index];
                for (int k = 0; k < qGb->pGboleData.at(real).cItem.size(); k++)
                    if (qGb->pGboleData.at(real).cItem.at(k).isSend && qGb->pGboleData.at(real).cItem.at(k).circle != -1)
                    {
                        if (qGb->pGboleData.at(real).cItem.at(k).timeAdd)
                            qGb->pGboleData.at(real).cItem.at(k).timeAdd->stop();
                    }
            }
            
        }
        else
        {
            int curindex = cbBitRate->currentIndex();
            int bitRate = 19200;
            switch (curindex)
            {
            case 4:
                bitRate = 2400; break;
            case 5:
                bitRate = 9600; break;
            case 6:
                bitRate = 10400; break;
            case 7:
                bitRate = 19200; break;
            default:
                bitRate = 19200;
                break;
            }
            QString dev = cbPcan->currentIndex() == 0 ? "1" : "2";
            //打开PLIN硬件
            bool b = pHardWare->OpenHardWare(dev, bitRate, 1);
            
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
            connect(pHardWare, &HardWarePlin::newMessage, this, &QtCanPlatform::on_ReceiveDataLIN);
            cbSelectModel->setEnabled(false);
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
        memcpy(m_arrVerCanMessage, data, 8);
        return;

    }
    //没有按下发送就不要接收数据
    //才改一天，这新源动力又说这不行，，，，shit
    
    if (m_iRecOnNoSend&&!pbSend->isChecked())
        return;
    communicaLabel->setText(tr("通信正常"));
    communicaLabel->setStyleSheet("background-color:green");
    lostQTimer->start(lostTimeOut);
    //从型号里读出索引
    int realIndex = cbSelectModel->currentIndex();
    //转换索引，转换后的才是vector中的正在索引
    int index = HashArr.at(realIndex);
   
    qGboleData* qGb = qGboleData::getInstance();
    if (!qGb)return;
    if (index > qGb->pGboleData.size()-1)
        return;
    if (m_bParseVer)
    {
        if (currentTestModel.ats.m_strVerRecID.toUInt(nullptr, 16) == fream_id)
        {
            QMutex mu;
            mu.lock();
            memcpy(m_arrVerCanMessage, data, 8);
            mu.unlock();
        }
    }
    if (0 == qGb->pGboleData.at(index).agreement)
    {
        //intel protocol
        recAnalyseIntel(fream_id, data);
    }
    else
    {
        //moto protocol
        recAnalyseMoto(fream_id, data);
    }
   
    int period = cycle->text().toInt();

    //防止数值大，图形曲线刷新太快
    if (period >= 1000)
    {
        timeStmp += (period / 1000);
    }
    else if(period >= 100)
    {
        timeStmp += (period / 100);
    }
    else
    {
        timeStmp = period;
    }
    //把数据传给图形化处理
    if(pbSend->isChecked())
        emit sigNewMessageToGraph(fream_id, data, timeStmp);
}
void QtCanPlatform::on_ReceiveDataLIN(uint frame_id, QByteArray data, int reserve)
{
    tabRollBox->tabBar()->setTabIcon(0, QApplication::style()->standardIcon((QStyle::StandardPixmap)31));
    tabRecBox->tabBar()->setTabIcon(0, QApplication::style()->standardIcon((QStyle::StandardPixmap)31));
    lostQTimerArr[0]->start(lostTimeOut);
   // for(int i=0;i<)
    on_ReceiveData(frame_id, data);
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
//void QtCanPlatform::on_ReceiveData2(ushort ch, uint frame_id, uchar data[8])
//{
//    QByteArray by;
//    for (int i = 0; i < 8; i++)
//    {
//        by.append(data[i]);
//    }
//    on_ReceiveData(ch, frame_id, by);
//}
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
        memcpy(m_arrVerCanMessage, data, 8);
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

    if (m_bParseVer)
    {
        if (currentTestModel.ats.m_strVerRecID.toUInt(nullptr, 16) == fream_id)
        {
            memcpy(m_arrVerCanMessage, data, 8);
        }
    }
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
    disconnect(tableView, SIGNAL(cellChanged(int, int)), this, SLOT(on_tableClicked(int, int)));
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
    if ((cbCanType->currentIndex() == 0 || cbCanType->currentIndex() == 3) && cbIsMutil->currentIndex() == 0)
        saveCanData();
    else if (cbCanType->currentIndex() == 1|| cbCanType->currentIndex() ==2|| cbIsMutil->currentIndex() == 1)
        saveCanDataMult();
}
void QtCanPlatform::on_pbClearCanData_clicked()
{
    multErr.clear();
    
    //recDataIntoTab();
    labelVer1.clear();
    labelVer2.clear();
    labelVer3.clear();
    labelVer4.clear();
    showTableVec.clear();
    for (int k = 0; k < 4; k++)
    {
        int rcount = tableRollDataArray[k]->rowCount();
        for (int m = 0; m < rcount; m++)
            tableRollDataArray[k]->removeRow(rcount - m - 1);
    }
    recDataIntoTab();
    strSaveList.clear();
    multReceData.clear();
    
    
}
void QtCanPlatform::on_CapturePower()
{
    countRunTime++;
    m_labShowTime->setText("运行时间：" + QString::number(countRunTime) + "秒");
    if (countRunTime >= runTimed)
    {
        on_pbSend_clicked(false);
        pbSend->setChecked(false);

        m_tCaptureTimer->stop();
        countRunTime = 0;
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
   /* if (isLogin)
    {
        on_canSettingShowFromLogin(this->loginType);
        return;
    }*/
    if (!canSeting)
    {
        canSeting = new Login(this);
        connect(canSeting, &Login::sigLogin, this, &QtCanPlatform::on_canSettingShowFromLogin);
    }
    canSeting->show();
    canSeting->setModal(true);
    
}
void QtCanPlatform::on_canSettingShowFromLogin(int n)
{
   /* this->isLogin = true;
    this->loginType = n;*/
    if (!canSetting) {
        canSetting = new QCanSetting();
        connect(canSetting, &QCanSetting::settingWidowsClose, this, &QtCanPlatform::on_SettingWidowsClose);
    }
    canSetting->setUserType(n);
    canSetting->reInitUI();
    canSetting->show();
}
void QtCanPlatform::readSetFile()
{
    
    qGboleData* gb = qGboleData::getInstance();
    if (!gb)
        return;
    gb->read_ini();
    struct autoTestData at = gb->getATData();
    //lowVolt = at.m_iLowVolt;
    //highVolt = at.m_iHeightVolt;
    //m_iHeatTempture = at.m_iHeatTempture;
    //m_iPowerTempture = at.m_iPowerTempture;
    //m_iOverTime = at.m_iOverTime;
    //m_iVoltError = at.m_iVoltError;
    //m_iVoltStep = at.m_iVoltError;
    m_sInWebAddr = at.m_sInWebAddr;
    m_sOutWebAddr = at.m_sOutWebAddr;
    m_iShowType = at.m_iShowType;
    m_iRecOnNoSend = at.m_iRecOnNoSend;
    saveListNum = at.m_iSaveListNum;
    if (saveListNum < 600)
        saveListNum = 600;

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
    
   /* setf->setValue("lowVolt", lowVolt);
    setf->setValue("highVolt", highVolt);
    setf->setValue("avgPower", avgPower);
    setf->setValue("m_iHeatTempture", m_iHeatTempture);
    setf->setValue("m_iPowerTempture", m_iPowerTempture);
    setf->setValue("m_iOverTime", m_iOverTime);
    
    setf->setValue("rmFirstFream", rmFirstFream);*/
    //setf->setValue("agvPowerFream", agvPowerFream);
}
void QtCanPlatform::on_pbDevicesManage_clicked()
{
    if (!autoDevMan)
    {
        autoDevMan = new AutoDeviceManage(this);
        autoDevMan->setWindowFlags(autoDevMan->windowFlags()|Qt::Tool);
        
        connect(autoDevMan, &AutoDeviceManage::sigMesNewData, this, &QtCanPlatform::on_sigFroMesNewData);
        connect(autoDevMan, &AutoDeviceManage::sigPowerNewData, this, &QtCanPlatform::on_sigFromPowerNewData);
    }
    autoDevMan->show();
}
void QtCanPlatform::on_pbGeneralParameter_clicked()
{

}
QStringList QtCanPlatform::ucharToBinay(const uchar data[8])
{
    QStringList binaryStr;
    //QString hex;
    for (int k = 0; k < 8; ++k)
    {
        QString str = QString("%1").arg((uint8_t)data[k], 8, 2, QLatin1Char('0'));
        binaryStr.append(str);

       // hex += QString("%1").arg(str.toInt(NULL, 2), 2, 16, QLatin1Char('0')).toUpper() + " ";
    }
    return binaryStr;
}
void QtCanPlatform::getVerAuto(const AutoTestStruct& at)
{
    memset(m_arrVerCanMessage, 0, 8);
    emit showAutoTestStep(2, "获取版本号中...", "");
    int startBit = at.m_usIDByteStartbit;
    int verLen = at.m_usIDBytelen;
    QString std_ver = at.m_strVer;
    m_bGetVer = true;
    m_bParseVer = true;
    while (runStep != -1)
    {
        QStringList list = ucharToBinay(m_arrVerCanMessage);
        if(verLen==1)
        {
            if (list.size()==8 && startBit <= 56)
            {
                
                int ver = list[startBit / 8].mid(startBit % 8, 8).toInt(nullptr, 2);
                if (ver != 0)
                {
                    QString verser =  QString("%1").arg(ver, 0, 16, QLatin1Char('0'));
                    float vvf = verser.toInt() / 10.0;
                    QString toVer = QString::number(vvf);
                    up_mes_var.m_strVer = toVer;
                    if (toVer == std_ver)
                        emit sigAutoTestSend(2, toVer);
                    else
                        emit sigAutoTestSend(3, toVer);
                    break;
                }
            }
            else
            {
                QString toVer = "0.0";
                up_mes_var.m_strVer = toVer;
                emit sigAutoTestSend(3, toVer);
                break;
            }
            QThread::msleep(100);
        }
        else if (verLen == 2)
        {
            if (list.size() == 8 && startBit <= 48)
            {
                int ver1 = list[startBit / 8].mid(startBit % 8, 8).toInt(nullptr, 2);
                int ver2 = list[startBit / 8 + 1].mid(startBit % 8, 8).toInt(nullptr, 2);
                QString toVer = QString::number(ver1)+"."+ QString::number(ver2);
                up_mes_var.m_strVer = toVer;
                if (toVer == std_ver)
                    emit sigAutoTestSend(2, toVer);
                else
                    emit sigAutoTestSend(3, toVer);
                break;
            }
            else
            {
                QString toVer = "0.0";
                up_mes_var.m_strVer = toVer;
                emit sigAutoTestSend(3, toVer);
                break;
            }
        }
        else if (verLen == 4)
        {
            if (list.size() == 8 && startBit<=32)
            {
                int ver1 = list[startBit / 8].mid(startBit % 8, 8).toInt(nullptr,2);
                int ver2 = list[startBit / 8+1].mid(startBit % 8, 8).toInt(nullptr, 2);
                int ver3 = list[startBit / 8+2].mid(startBit % 8, 8).toInt(nullptr, 2);
                int ver4 = list[startBit / 8+3].mid(startBit % 8, 8).toInt(nullptr, 2);
                if (ver1 && ver3 && ver3!=40)
                {
                    QString verser = "M:" + QString::number(ver1) + "." + QString::number(ver2) + "-S:" + QString::number(ver3) + "." + QString::number(ver4);
                    up_mes_var.m_strVer = verser;
                    if(verser==std_ver)
                        emit sigAutoTestSend(2, verser);
                    else
                        emit sigAutoTestSend(3, verser);
                    break;
                }
            }
            else
            {
                QString verser = "M:0.0-S:0.0";
                up_mes_var.m_strVer = verser;
                emit sigAutoTestSend(3, verser);
                break;
            }
            QThread::msleep(200);
        }
    }
    m_bGetVer = false;
    m_bParseVer = false;
}
void QtCanPlatform::setPowerSupply(const AutoTestStruct& at,int op)
{
    QString reqV;
    QString cur = "Current:lim 60\n";
    //autoDevMan->requireTcpPower(cur);
    on_sigFromThisPowerSet(cur);
    QTime m_t = QTime::currentTime().addMSecs(500);
    while (QTime::currentTime() < m_t)
        QApplication::processEvents();
    if (op == -1)
    {
        reqV = "Voltage " + QString::number(at.m_iLowVoltage) + ";Output on\n";
    }
    else if (op == -2)
    {
        reqV = "Voltage " + QString::number(at.m_iLowVoltageRe) + ";Output on\n";
    }
    else if(op == 1)
    {
        reqV = "Voltage " + QString::number(at.m_iOverVoltage) + ";Output on\n";
    }
    else if (op == 2)
    {
        reqV = "Voltage " + QString::number(at.m_iOverVoltageRe) + ";Output on\n";
    }
    else if (op == 0)
    {
        reqV = "Voltage " + QString::number(at.m_iRatedVolt) + ";Output on\n";
    }  
    else if (op == -99)
    {
        reqV = "Voltage " + QString::number(0) + ";Output off\n";
    }
   //QLOG_INFO() << reqV;
    on_sigFromThisPowerSet(reqV);
    //setHeatint(at, 0);
}
void QtCanPlatform::setHeatint(const AutoTestStruct& at, float value)
{
    int en_row = at.m_iEnableInLine;
    int pw_row = at.m_iPowerInLine;
    QTableWidgetItem* item = tableView->item(pw_row, 2);
    if (!item) { return; }
    item->setText(QString::number(value));

    QComboBox* cb = dynamic_cast<QComboBox*> (tableView->cellWidget(en_row, 1));
    if (!cb) { return; }
    cb->setCurrentIndex(at.m_iEnOp);
}
void QtCanPlatform::setCancelHeatint(const AutoTestStruct& at, float value)
{
    int en_row = at.m_iEnableInLine;
    int pw_row = at.m_iPowerInLine;
    QTableWidgetItem* item = tableView->item(pw_row, 2);
    if (!item) { return; }
    item->setText(QString::number(0));

    QComboBox* cb = dynamic_cast<QComboBox*> (tableView->cellWidget(en_row, 1));
    if (!cb) { return; }
    cb->setCurrentIndex(0);
}
void QtCanPlatform::getAveragePW(const AutoTestStruct& at)
{
    int lestCount = 0;
    PowerArr[0].clear();
    int tempture = currentTestModel.ats.m_usRatedPWTemp;
    QLOG_INFO() << "tempture:" << tempture;
    while (runStep != -1)
    {
        if (realWTemp[0] == tempture)
        {
            while (realWTemp[0] == tempture)
            {
                PowerArr[0].push_back(realPower[0]);
                if (PowerArr[0].size() >= agvPowerFream)
                    break;
                QThread::msleep(_period_ + 50);
            }
        }
        if (realWTemp[0] > tempture)
        {
            lestCount++;
            if (lestCount > 3)
                break;
        }
        if (PowerArr[0].size() >= agvPowerFream)
            break;
    }
    float sum=0;
    for (int i = 0; i < PowerArr[0].size(); i++)
        sum += PowerArr[0].at(i);
    sum = sum / PowerArr[0].size();
    QStringList tolerance = at.m_strTolerance.split(";");
    float nagetive = 0.1;
    float postive = 0.1;
    if (tolerance.size() == 2)
    {
        postive = tolerance.at(0).mid(1).toFloat() / 100.0;
        nagetive = tolerance.at(1).mid(1).toFloat() / 100.0;
    }
    if (sum < at.m_fRatedPW)
    {
        if (sum >= (at.m_fRatedPW - at.m_fRatedPW * nagetive))
        {
            emit sigAutoTestSend(20, QString::number(sum));
            //showAutoTestStep(5, QString::number(sum), "OK");
        }
        else
        {
            emit sigAutoTestSend(21, QString::number(sum));
           // showAutoTestStep(5, QString::number(sum), "NG");
            up_mes_var.m_strTestResult = "9";
        }
    }
    else
    {
        if (sum <= (at.m_fRatedPW + at.m_fRatedPW * postive))
        {
            //showAutoTestStep(5, QString::number(sum), "OK");
            emit sigAutoTestSend(20, QString::number(sum));
        }
        else
        {
            emit sigAutoTestSend(21, QString::number(sum));
            //showAutoTestStep(5, QString::number(sum), "NG");
            up_mes_var.m_strTestResult = "9";
        }
    }
    up_mes_var.m_strRatedPW = QString::number(sum);
}
void QtCanPlatform::on_pbMESConnect(bool b)
{
    if (!b)
    {
        if (!postMes)return;
        postMes->disconnectFromHost();

    }
    else
    {
        postMes = new QTcpSocket(this);
        connect(postMes, &QTcpSocket::stateChanged, this, &QtCanPlatform::on_TCPStateChanged);
        connect(postMes, &QTcpSocket::readyRead, this, &QtCanPlatform::on_TCPDataReadyRec);
        connect(this, &QtCanPlatform::sigSendMesState, this, &QtCanPlatform::on_SendMesState);
    }
    if (postMes->state() == QAbstractSocket::ConnectedState)
        return;
    postMes->connectToHost(LineEdit_IPAddr_Mes->text().trimmed(), LineEdit_Port_Mes->text().toInt());

}
void QtCanPlatform::on_TCPStateChanged(QAbstractSocket::SocketState state)
{
    QTcpSocket* tcp = dynamic_cast<QTcpSocket*>(sender());
    if (state == QAbstractSocket::UnconnectedState)
    {
       if (tcp && tcp == postMes)
        {
            QLOG_WARN() << "MES Connect break";
        }

    }
    else if (state == QAbstractSocket::ConnectedState)
    {
        if (tcp && tcp == postMes)
        {
            QLOG_INFO() << "MES Connnected!";
        }
    }
}
void QtCanPlatform::on_TCPDataReadyRec()
{
    QTcpSocket* tcp = dynamic_cast<QTcpSocket*>(sender());
   if (tcp && tcp == postMes)
    {
        if (postMes->bytesAvailable() > 0)
        {
            QByteArray rec = postMes->readAll();
            //QLOG_INFO() << rec;
            m_Mutex_Mes.lock();
            m_MESData.append(rec);
            m_Mutex_Mes.unlock();
            _bIsRec_Mes = true;
        }
    }

}
void QtCanPlatform::on_sigFromThisPowerSet(QString data)
{
    autoDevMan->requireTcpPower(data);
}
void QtCanPlatform::on_savePeriodCheck_Changed(int n)
{
    if (n == 0)
    {
        m_iSavePeroidNum = 1;
        m_lePeroid->setEnabled(true);
    }
    else
    {
        m_iSavePeroidNum = m_lePeroid->text().toUInt();
        m_lePeroid->setEnabled(false);
    }
}
void QtCanPlatform::on_outTempMonitor_Changed(int n)
{
    if (!pbStartAutoTest)return;
    if (n == 0)
    {
        
         pbStartAutoTest->setText("开启自动测试");
    }
    else
    {
       
         pbStartAutoTest->setText("开启出口温度监控");

    }
}
void QtCanPlatform::on_plotWindowCLose()
{
    if (LogPb)
        LogPb->setEnabled(true);
}
void QtCanPlatform::on_TCPDataSend_MES(QByteArray data)
{
   /* if (!postMes)return;
    postMes->write(data);*/
    autoDevMan->requireTcpMes(data);
}
void QtCanPlatform::on_pbSummitCode_clicked()
{
    m_strPHUCode = lineEditCodeIn->text();
    CodeOk = true;
    lineEditCodeIn->setEnabled(false);
    
    return;
    //=======下面是单MES的=========
    QString text = lineEditCodeIn->text();
    if (text.isEmpty())
    {
        QMessageBox::warning(this, "警告", "二维码为空！");
        return;
    }
    lineEditCodeIn->setEnabled(false);
    
    QtConcurrent::run(this, &QtCanPlatform::runPostMes, text);
    
    lineEditCodeIn->clear();
    
    
}
float QtCanPlatform::getPowerResponed(QString str)
{
    QByteArray sendData;
    sendData.append(str);
    _bIsRec_Pow = false;
    emit sigSendPutPowData(sendData);
    QLOG_INFO() << str;
    QTime tt = QTime::currentTime().addMSecs(10000);
    while (QTime::currentTime() < tt)
    {
        QThread::msleep(1);
        if (_bIsRec_Pow) break;
    }
    if (QTime::currentTime() > tt)
    {

        QLOG_WARN() << "communication overtime";
        return false;
    }
    if (m_PowerData.size() <= 0) return false;
    m_Mutex_Pow_Pop.lock();
    QString dTemp = m_PowerData.front();
    m_PowerData.pop_front();
    m_Mutex_Pow_Pop.unlock();

    return dTemp.toFloat();

}
bool QtCanPlatform::getMesResponed(QString str)
{
    QByteArray sendData;
    sendData.append(str);
    _bIsRec_Mes = false;
    emit sigSendPutMesData(sendData);
    QLOG_INFO() << str;
    QTime tt = QTime::currentTime().addMSecs(10000);
    while (QTime::currentTime() < tt)
    {
        QThread::msleep(1);
        if (_bIsRec_Mes) break;
    }
    if (QTime::currentTime() > tt)
    {
       
        QLOG_WARN() << "communication overtime";
        return false;
    }
    if (m_MESData.size() <= 0) return false;
    m_Mutex_Mes_Pop.lock();
    QString dTemp = m_MESData.front();
    m_MESData.pop_front();
    m_Mutex_Mes_Pop.unlock();
    if ("Y" != parserMESStr(dTemp))
        return false;
    return true;

}
QString QtCanPlatform::parserMESStr(QString str)
{
    QStringList dlist = str.split(";");
    if (dlist.size() < 2)
    {
        return "N";
    }
    if (dlist.at(1).size() < 1 || dlist.at(1).at(0) != "Y")
    {
        QLOG_INFO() << dlist.at(1);
        return "N";
    }
    QLOG_INFO() << dlist.at(1);
    return "Y";
}
void QtCanPlatform::on_SendMesState(int n, QString)
{
    if (n == 0)
    {
        QMessageBox::information(this, "Tips", "出站成功,该产品已过站");
    }
    else if (n == 1)
    {
        QMessageBox::information(this, "Tips", "入站失败,请检查条码或者MES连接");
    }
    else if (n == 2)
    {
        QMessageBox::information(this, "Tips", "出站失败,请检查条码或者MES连接");
    }
    lineEditCodeIn->setEnabled(true);
}
void QtCanPlatform::runPostMes(QString text)
{
    
    QString senddata = "#A101;" + text + "|PHU7KW-007|N$";
    bool b = getMesResponed(senddata);
    if (!b)
    {
        sigSendMesState(1, "入站失败");
        return;
    }
    
    QThread::msleep(2000);
    senddata = "#A102;" + text + "|PHU7KW-007|1.0,12.0,20.0,500K,12.0,V1.0,10.0,10.0,10.0,10.0,100.0,10.0,10.0,0,1$";
    b = getMesResponed(senddata);
    if (!b)
    {
        sigSendMesState(2, "出站失败");
        return;
    }
    sigSendMesState(0, "出站成功");
}
void QtCanPlatform::on_lineEditCodeIn_editingFinished()
{
    pbSummitCode->setFocus();
   
}
void QtCanPlatform::on_sigFromPowerNewData(QString data)
{
    m_Mutex_Pow.lock();
    m_PowerData.append(data);
    _bIsRec_Pow = true;
    m_Mutex_Pow.unlock();
}
void QtCanPlatform::on_sigFroMesNewData(QString data)
{
   
    m_Mutex_Mes.lock();
    m_MESData.append(data);
    _bIsRec_Mes = true;
    m_Mutex_Mes.unlock();
   
}
void QtCanPlatform::on_pbStartAutoTest_clicked(bool b)
{
    
   
    if (!b)
    {
        runStep = -1;
        return;
    }
    if (m_cbOutTempMonitor && m_cbOutTempMonitor->isChecked())
    {
        runStep = 0;
        QtConcurrent::run(this, &QtCanPlatform::runOutMonitor);
        return;
    }
    else
    {
        initAutoResTableWidget();
        QString str;
        if(cbSelectModel)
            str = cbSelectModel->currentText();
       int res =  QMessageBox::information(this, "Tips", "是否对：" + str + " 型号进行测试?",QMessageBox::Ok| QMessageBox::No);
       if (res != QMessageBox::Ok)
       {
           pbStartAutoTest->setChecked(false);
           runStep = -1;
           return;
       }
       if (!autoDevMan)
       {
           autoDevMan = new AutoDeviceManage(this);
           autoDevMan->setWindowFlags(autoDevMan->windowFlags() | Qt::Tool);
           connect(this, &QtCanPlatform::sigAutoTestSend, this, &QtCanPlatform::on_processAutoTestSignal);
           connect(autoDevMan, &AutoDeviceManage::sigMesNewData, this, &QtCanPlatform::on_sigFroMesNewData);
           connect(autoDevMan, &AutoDeviceManage::sigPowerNewData, this, &QtCanPlatform::on_sigFromPowerNewData);
       }
       autoDevMan->initializeDev();
       QTime tempT = QTime::currentTime().addMSecs(1500);
       while (QTime::currentTime()< tempT)
       {
           QApplication::processEvents();
       }
       int n = autoDevMan->getDeviceState();
       if (-1 == n)
       {
           pbStartAutoTest->setChecked(false);
           QMessageBox::information(this, "Tips", "电源和冷水机没有初始化，请检测");
           runStep = -1;
           return;
       }
       else if (1 == n)
       {
           pbStartAutoTest->setChecked(false);
           QMessageBox::information(this, "Tips", "冷水机没有初始化，请检测");
           runStep = -1;
           return;
       }
       else if (2 == n)
       {
           pbStartAutoTest->setChecked(false);
           QMessageBox::information(this, "Tips", "电源没有初始化，请检测");
           runStep = -1;
           return;
       }
       else if (3 == n)
       {
           pbStartAutoTest->setChecked(false);
           QMessageBox::information(this, "Tips", "PLC没有初始化，请检测");
           runStep = -1;
           return;
       }
       QLOG_INFO() << "Devices Initialize Over";
       lineEditCodeIn->setToolTip("请在此录入PHU二维码");
       lineEditCodeIn->setFocus();
       QPoint pos = lineEditCodeIn->mapToGlobal(QPoint(0, 0));
       QPoint pPos = this->mapFromGlobal(pos);
       QToolTip::showText(pPos, "请在此录入二维码", lineEditCodeIn);
      
       runStep = 0;
       QtConcurrent::run(this, &QtCanPlatform::workAutoTest);
    }
}
void QtCanPlatform::runOutMonitor()
{
    int outTempture = currentTestModel.ats.m_iOutTempStop;
    while (runStep != -1)
    {
        if (realWTemp[0] >= outTempture && (realWTemp[0] != 205 && realWTemp[0] != 215 && realWTemp[0] != 255))
        {
            emit sigAutoTestSend(23, "关闭加热");
            QLOG_INFO() << "出口温度大于" << outTempture << "°，停止加热";
            QThread::msleep(5000);
        }
        QThread::msleep(100);
    }
}
void QtCanPlatform::showAutoTestStep(int n, QString data, QString remake)
{
    int row = tableAutoResults->rowCount();
    QTableWidgetItem* item;
    QTableWidgetItem* item2;
    if (row - 1 >= n)
    {
        item = tableAutoResults->item(row, 1);
        item2 = tableAutoResults->item(row, 2);
        if (item) { item->setText(data); }
        else {
            item = new QTableWidgetItem(data);
        }

        if (item2) { item2->setText(remake); }
        else {
            item2 = new QTableWidgetItem(remake);
        }
        if(remake=="OK")
            item->setBackgroundColor(QColor(20, 240, 10));
        else if(remake=="NG")
            item->setBackgroundColor(QColor(240, 10, 10));
        tableAutoResults->setItem(n, 1, item);
        tableAutoResults->setItem(n, 2, item2);
    }
}
void QtCanPlatform::on_processAutoTestSignal(int n, QString str)
{
    switch (n)
    {
    case -2:
        autoDevMan->setCoolantTemp(currentTestModel.ats.m_usCoolTemp, currentTestModel.ats.m_usRatedPWFlow, true, false);   //冷水机开制冷
        break;
    case -1:
        autoDevMan->on_pbPureWater_2_clicked(false);
        autoDevMan->on_pbColdWater_2_clicked(true);
        autoDevMan->setCoolantTemp(currentTestModel.ats.m_usCoolTemp, currentTestModel.ats.m_usRatedPWFlow, true, true);   //冷水机开制冷
        QLOG_INFO() << "开冷水机";
        break;
    case 0:
        m_bCommunication = 0;
        on_pbSend_clicked(true); pbSend->setChecked(true);
        showAutoTestStep(1, str, "");
        break;
    case 1:
        if(str=="通信正常")
            showAutoTestStep(1, str, "OK");     //通信OK
        else
            showAutoTestStep(1, str, "NG");     //通信OK
        break;
    case 2:
        if(str.contains("版本号"))
            showAutoTestStep(2, str, "");       //版本号？
        else
            showAutoTestStep(2, str, "OK");       //版本号？
        break;
    case 3:
        showAutoTestStep(2, str, "NG");
        break;
    case 4:
        showAutoTestStep(3, str, "");
        setPowerSupply(currentTestModel.ats, -1);   //设置电压，极限电流
        setHeatint(currentTestModel.ats, 0);        //设置使能但功率为0
        QLOG_INFO() << "case 4";
        break;
    case 5:
        showAutoTestStep(3, str, "");
        setPowerSupply(currentTestModel.ats, -2);   //设置电压，极限电流
        setHeatint(currentTestModel.ats, 0);        //设置使能但功率为0
        QLOG_INFO() << "case 5";
        break;
    case 6:
        if(str.contains("NG"))
            showAutoTestStep(3, str, "NG");
        else
            showAutoTestStep(3, QString::number(currentTestModel.ats.m_iLowVoltage)+"/"+ QString::number(currentTestModel.ats.m_iLowVoltageRe), "OK");
        break;
    case 7://过压保护测试
        showAutoTestStep(4, str, "");
        setPowerSupply(currentTestModel.ats, 1);
       
        break;
    case 8://过压恢复测试
        showAutoTestStep(4, str, "");
        setPowerSupply(currentTestModel.ats, 2);
        
        break;
    case 9:
        if (str.contains("NG"))
            showAutoTestStep(4, str, "NG");
        else
            showAutoTestStep(4, QString::number(currentTestModel.ats.m_iOverVoltage)+"/"+ QString::number(currentTestModel.ats.m_iOverVoltageRe), "OK");
        setPowerSupply(currentTestModel.ats, 0);
        setCancelHeatint(currentTestModel.ats, 0);        //设置不使能，中华汽车只要使能就立马加热
        break;
    case 10://使能加热
        autoDevMan->setCoolantTemp(currentTestModel.ats.m_usCoolTemp, currentTestModel.ats.m_usRatedPWFlow, !(currentTestModel.ats.m_bTurnOffCool), true);
        setPowerSupply(currentTestModel.ats, 0);
        setHeatint(currentTestModel.ats, currentTestModel.ats.m_fRequirePW);
        break;
    case 11:
        showAutoTestStep(8, str, "NG");//其它故障
        break;
    case 12:
        autoDevMan->setCoolantTemp(currentTestModel.ats.m_usCoolTemp, currentTestModel.ats.m_usRatedPWFlow, !(currentTestModel.ats.m_bTurnOffCool), !(currentTestModel.ats.m_bTurnOffFlow));
        showAutoTestStep(6, str, "");
        break;
    case 13:
        autoDevMan->setCoolantTemp(currentTestModel.ats.m_usCoolTemp, currentTestModel.ats.m_usRatedPWFlow, true, true);
        if(str.contains("NG"))
            showAutoTestStep(6, str, "NG");
        else
            showAutoTestStep(6, str + "°C", "OK");
        break;
    case 14:
        showAutoTestStep(7, str, "");
        break;
    case 15:
        if (str.contains("NG"))
            showAutoTestStep(7, str, "NG");
        else
            showAutoTestStep(7, str + "°C", "OK");
        break;
    case 18:
        setPowerSupply(currentTestModel.ats, -99);
        if(str!="1")
            showAutoTestStep(9, str, "NG");
        else
            showAutoTestStep(9, str, "OK");
        break;
    case 20:
        showAutoTestStep(5, str, "OK");
        break;
    case 21:
        showAutoTestStep(5, str, "NG");
        break;
    case 23:
        setCancelHeatint(currentTestModel.ats, 0);
        break;
    case 30:
        showAutoTestStep(0, str, "NG");
        QMessageBox::information(this, "入站失败", "检测该产品的二维是否在系统？", QMessageBox::Ok | QMessageBox::No);
        lineEditCodeIn->setEnabled(true);
        break;
    case 31:
        showAutoTestStep(0, str, "OK");
        if (QMessageBox::Ok == QMessageBox::information(this, "入站成功", "请放好产品后再点OK进行测试", QMessageBox::Ok | QMessageBox::No))
        {
            runStep = 0;
        }
        else
        {
            runStep = -1;
        }
        lineEditCodeIn->clear();
        break;
    case 33:
        showAutoTestStep(0, str, "NG");
        QMessageBox::information(this, "出站失败", "检测该产品的二维是否在系统？", QMessageBox::Ok | QMessageBox::No);
        lineEditCodeIn->setEnabled(true);
        break;
    case 34:
        showAutoTestStep(0, str, "OK");
        lineEditCodeIn->setEnabled(true);
        break;
    case 36:
        autoDevMan->on_pbBlowWater_clicked(true);
        break;
    case 38:
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
        break;
    case 39:
        progress->setValue(str.toInt());
        if (str.toInt() >= 120)
        {
            progress->setLabelText(tr("吹水完成，请关闭冷水阀,进行后续工作"));
            autoDevMan->on_pbBlowWater_clicked(false);
            autoDevMan->on_pbColdWater_2_clicked(false);
        }
        break;
    case -99:
        pbStartAutoTest->setChecked(false);
        lineEditCodeIn->setEnabled(true);
        setCancelHeatint(currentTestModel.ats, 0);
        setPowerSupply(currentTestModel.ats, -99);
        break;
    case -100:
        lineEditCodeIn->setEnabled(true);
        setPowerSupply(currentTestModel.ats, -99);
        break;
    case -3:
        initAutoResTableWidget();
        break;
    default:
        break;
    }
}
bool QtCanPlatform::upMesOutData()
{
    QString res_mes = "#A102;" + m_strPHUCode + "|PHU7KW-007|";
    QString t_data = up_mes_var.m_strRatedTemp + "," +
        up_mes_var.m_strRatedVoltage + "," +
        up_mes_var.m_strRatedFlow + "," +
        up_mes_var.m_strBundrate + "," +
        up_mes_var.m_strLowVoltage + "," +
        up_mes_var.m_strVer + "," +
        up_mes_var.m_strLowProtected + "," +
        up_mes_var.m_strLowProtectedRe + "," +
        up_mes_var.m_strHighProtected + "," +
        up_mes_var.m_strHighProtectedRe + "," +
        up_mes_var.m_strRatedPW + "," +
        up_mes_var.m_strOverTempProtected + "," +
        up_mes_var.m_strOverTempProtectedRe + "," +
        up_mes_var.m_strOtherFault + "," +
        up_mes_var.m_strTestResult + "$";
    bool b4 = getMesResponed(res_mes + t_data);
    if (!b4)
    {
        emit sigAutoTestSend(33, "出站失败");
    }
    else
    {
        emit sigAutoTestSend(34, "出站成功");
    }
    return b4;

}
void clear_up_mes_var(struct UpMesData* up_mes_var)
{
    if (!up_mes_var)return;
    up_mes_var->m_strRatedTemp = "-1";
    up_mes_var->m_strRatedVoltage = "-1";
    up_mes_var->m_strRatedFlow = "-1";
    up_mes_var->m_strBundrate = "-1";
    up_mes_var->m_strLowVoltage = "-1";
    up_mes_var->m_strVer = "-1";
    up_mes_var->m_strLowProtected = "-1";
    up_mes_var->m_strLowProtectedRe = "-1";
    up_mes_var->m_strHighProtected = "-1";
    up_mes_var->m_strHighProtectedRe = "-1";
    up_mes_var->m_strRatedPW = "-1";
    up_mes_var->m_strOverTempProtected = "-1";
    up_mes_var->m_strOverTempProtectedRe = "-1";
    up_mes_var->m_strOtherFault = "-1";
    up_mes_var->m_strTestResult = "9";
}
void QtCanPlatform::workAutoTest()
{
    while (runStep != -1)
    {


        while (runStep != -1)
        {
            
            runStep = 99;
            bool eachResult = true;
            //Step0
        //检测测试条件
#ifdef _NEED_CODE_
    //等待扫码请求MES。
            while (runStep != -1)
            {
                while (!CodeOk)
                {
                    QThread::msleep(100);
                    continue;
                }
                QString sendstr = "#A101;" + m_strPHUCode + "|PHU7KW-007|N$";
                bool b4 = getMesResponed(sendstr);
                emit sigAutoTestSend(-3, "清除结果");
                CodeOk = false;
                if (!b4)
                {
                    emit sigAutoTestSend(30, "入站失败");
                    continue;
                }
                emit sigAutoTestSend(31, "入站成功");
                while (runStep != 0)
                {
                    QThread::msleep(10);
                }
                break;
            }
#endif // _NEED_CODE_
            
            while (runStep != 0)
            {
                QThread::msleep(5);
                if (runStep == -1)
                {
                    continue;
                }
            }
            clear_up_mes_var(&up_mes_var);
            if (currentTestModel.bundRate >= 0 && currentTestModel.bundRate < 8)
                up_mes_var.m_strBundrate = strBundRate[currentTestModel.bundRate];
            else
                up_mes_var.m_strBundrate = "0";
            up_mes_var.m_strLowVoltage = QString::number(24.0);
            up_mes_var.m_strRatedVoltage = QString::number(currentTestModel.ats.m_iRatedVolt);
            up_mes_var.m_strRatedTemp = QString::number(currentTestModel.ats.m_usRatedPWTemp);
            up_mes_var.m_strRatedFlow = QString::number(currentTestModel.ats.m_usRatedPWFlow);
            up_mes_var.m_strTestResult = "1";
            //检测退出
            if (runStep == -1) { upMesOutData(); emit sigAutoTestSend(-99, "人工退出测试"); QLOG_INFO() << "退出测试"; return; }

            //   
           
            emit sigAutoTestSend(-1, "冷水机开");
            QThread::msleep(10);
            emit sigAutoTestSend(-1, "冷水机开");
            //检测退出
            if (runStep == -1) { emit sigAutoTestSend(-99, "人工退出测试"); upMesOutData();  QLOG_INFO() << "退出测试"; return; }
            //    //Step1
                //检测通信是否OK
            m_bCommunication = 0;
            emit sigAutoTestSend(0, "开始检测通信");
            QTime overTimeM = QTime::currentTime().addMSecs(10000);
            while (m_bCommunication < 5)
            {
                QThread::msleep(10);
                if (runStep == -1)
                {
                    up_mes_var.m_strTestResult = "9";
                    emit sigAutoTestSend(-99, "通信出错"); upMesOutData();  QLOG_INFO() << "退出测试"; return;
                    return;
                }
                if (QTime::currentTime() > overTimeM)
                {
                    emit sigAutoTestSend(1, "通信异常");
                    up_mes_var.m_strTestResult = "9";
                    up_mes_var.m_strOtherFault = "*通讯异常";
                    emit sigAutoTestSend(18, up_mes_var.m_strTestResult); upMesOutData();  QLOG_INFO() << "退出测试";;
                    break;
                }
            }
            if (QTime::currentTime() > overTimeM)
            {
                break;
            }
            //检测退出
            if (runStep == -1) { emit sigAutoTestSend(-99, "人工退出测试"); upMesOutData();  QLOG_INFO() << "退出测试"; return; }
            emit sigAutoTestSend(1, "通信正常");

            emit sigAutoTestSend(2, "获取版本号中");
            getVerAuto(currentTestModel.ats);
            //检测退出
            if (runStep == -1) { emit sigAutoTestSend(-99, "人工退出测试"); upMesOutData();  QLOG_INFO() << "退出测试"; return; }
            //Step2
            //测试低压欠压
            Error[0].clear();
            isRecordError = true;
            emit sigAutoTestSend(4, "欠压保护测试中");
            QThread::msleep(1000);
            float vot = getPowerResponed("MEAS:VOLT?\n");
            if (abs(vot - currentTestModel.ats.m_iLowVoltage) > 10)
            {
                emit sigAutoTestSend(11, "电源电压异常");
                up_mes_var.m_strOtherFault += "电源输出电压与设置不一致";
                up_mes_var.m_strTestResult = "9";
                emit sigAutoTestSend(-99, "退出测试"); upMesOutData();  QLOG_INFO() << "退出测试"; return;
            }
            QTime ovTimeVolt = QTime::currentTime().addMSecs(5000);
            eachResult = true;
            while (runStep != -1)
            {
                if (realHVErr[0] == ("高压异常") || realHVErr[0] == ("高压欠压") || realHVErr[0] == ("欠压") || realHVErr[0] == ("高压故障"))
                    break;
                QThread::msleep(500);
                if (QTime::currentTime() > ovTimeVolt)
                {
                    up_mes_var.m_strLowProtected = QString::number(currentTestModel.ats.m_iLowVoltage);
                    up_mes_var.m_strOtherFault = "*欠压保护不合格";
                    eachResult = false;
                    up_mes_var.m_strTestResult = "9";
                    break;
                }
            }
            up_mes_var.m_strLowProtected = QString::number(currentTestModel.ats.m_iLowVoltage);
            //检测退出
            if (runStep == -1) { emit sigAutoTestSend(-99, "人工退出测试"); upMesOutData();  QLOG_INFO() << "退出测试"; return; }
            //m_bResetfault = true;
            //检查结果
            if (eachResult)
            {
                emit sigAutoTestSend(5, "欠压保护恢复测试中");
            }
            else
            {
                emit sigAutoTestSend(5, "欠压保护NG");
                upMesOutData();  QLOG_INFO() << "本次测试结束";
                emit sigAutoTestSend(18, up_mes_var.m_strTestResult);
                break;
            }
            QThread::msleep(1000);
            vot = getPowerResponed("MEAS:VOLT?\n");
            if (abs(vot - currentTestModel.ats.m_iLowVoltageRe) > 10)
            {
                emit sigAutoTestSend(11, "电源电压异常");
                up_mes_var.m_strOtherFault += "*电源输出电压与设置不一致";
                up_mes_var.m_strTestResult = "9";
                emit sigAutoTestSend(-99, "退出测试"); upMesOutData();  QLOG_INFO() << "退出测试"; return;
            }
            ovTimeVolt = QTime::currentTime().addMSecs(5000);
            eachResult = true;
            while (runStep != -1)
            {
                if (realHVErr[0] != ("高压异常") && realHVErr[0] != ("高压欠压") && realHVErr[0] != ("欠压") && realHVErr[0] != ("高压故障"))
                    break;
                QThread::msleep(500);

                if (QTime::currentTime() > ovTimeVolt)
                {
                    up_mes_var.m_strLowProtectedRe = QString::number(currentTestModel.ats.m_iLowVoltageRe);
                    up_mes_var.m_strOtherFault = "*欠压恢复不合格";
                    eachResult = false;
                    up_mes_var.m_strTestResult = "9";
                    break;
                }
            }
            up_mes_var.m_strLowProtectedRe = QString::number(currentTestModel.ats.m_iLowVoltageRe);
            //检测退出
            if (runStep == -1) { emit sigAutoTestSend(-99, "人工退出测试"); upMesOutData();  QLOG_INFO() << "退出测试"; return; }
            if (eachResult)
                emit sigAutoTestSend(6, "欠压保护OK");
            else
            {
                emit sigAutoTestSend(6, "欠压恢复NG");
                upMesOutData();  QLOG_INFO() << "本次测试结束";
                emit sigAutoTestSend(18, up_mes_var.m_strTestResult);
                break;
            }
            //测试高压过压
            emit sigAutoTestSend(7, "过压保护测试中");
            m_bResetfault = false;
            isRecordError = false;
            QThread::msleep(500);
            Error[0].clear();
            isRecordError = true;
            //Step3

            QThread::msleep(500);
            vot = getPowerResponed("MEAS:VOLT?\n");
            if (abs(vot - currentTestModel.ats.m_iOverVoltage) > 10)
            {
                emit sigAutoTestSend(11, "电源电压异常");
                up_mes_var.m_strOtherFault += "电源输出电压与设置不一致";
                up_mes_var.m_strTestResult = "9";
                emit sigAutoTestSend(-99, "退出测试"); upMesOutData();  QLOG_INFO() << "退出测试"; break;
            }
            ovTimeVolt = QTime::currentTime().addMSecs(5000);
            eachResult = true;
            while (runStep != -1)
            {
                if (realHVErr[0] == ("高压异常") || realHVErr[0] == ("高压过压") || realHVErr[0] == ("过压") || realHVErr[0] == ("高压故障"))
                    break;
                QThread::msleep(500);

                if (QTime::currentTime() > ovTimeVolt)
                {
                    up_mes_var.m_strHighProtected = QString::number(currentTestModel.ats.m_iOverVoltage);
                    up_mes_var.m_strOtherFault = "*过压保护不合格";
                    eachResult = false;
                    up_mes_var.m_strTestResult = "9";
                    break;
                }
            }
            up_mes_var.m_strHighProtected = QString::number(currentTestModel.ats.m_iOverVoltage);
            //检测退出
            if (runStep == -1) { emit sigAutoTestSend(-99, "人工退出测试"); upMesOutData();  QLOG_INFO() << "退出测试"; return; }
            m_bResetfault = true;
            if(eachResult)
            {
                emit sigAutoTestSend(8, "过压保护恢复测试中");
            }
            else
            {
                emit sigAutoTestSend(9, "过压保护NG");
                upMesOutData();  QLOG_INFO() << "本次测试结束";
                emit sigAutoTestSend(18, up_mes_var.m_strTestResult);
                break;

            }
            QThread::msleep(1000);
            vot = getPowerResponed("MEAS:VOLT?\n");
            if (abs(vot - currentTestModel.ats.m_iOverVoltageRe) > 10)
            {
                emit sigAutoTestSend(11, "电源电压异常");
                up_mes_var.m_strOtherFault = "*电源输出电压与设置不一致";
                up_mes_var.m_strTestResult = "9";
                emit sigAutoTestSend(-99, "退出测试"); upMesOutData();  QLOG_INFO() << "退出测试";  break;;
            }
            ovTimeVolt = QTime::currentTime().addMSecs(5000);
            eachResult = true;
            while (runStep != -1)
            {
                if (realHVErr[0] != ("高压异常") && realHVErr[0] != ("高压过压") && realHVErr[0] != ("过压"))
                    break;
                QThread::msleep(500);
                if (QTime::currentTime() > ovTimeVolt)
                {
                    up_mes_var.m_strHighProtected = QString::number(currentTestModel.ats.m_iOverVoltage);
                    up_mes_var.m_strOtherFault = "*过压恢复不合格";
                    eachResult = false;
                    up_mes_var.m_strTestResult = "9";
                    break;
                }
            }
            up_mes_var.m_strHighProtectedRe = QString::number(currentTestModel.ats.m_iOverVoltageRe);
            //检测退出
            if (runStep == -1) { emit sigAutoTestSend(-99, "人工退出测试"); upMesOutData();  QLOG_INFO() << "退出测试"; return; }
            if(eachResult)
            {
                emit sigAutoTestSend(9, "过压保护OK");
            }
            else
            {
                emit sigAutoTestSend(9, "过压保护NG");
                upMesOutData();  QLOG_INFO() << "本次测试结束";
                emit sigAutoTestSend(18, up_mes_var.m_strTestResult);
                break;

            }
            QThread::msleep(500);
            //emit sigAutoTestSend(23, "关闭加热");
            isRecordError = false;
            m_bResetfault = false;
            int tempture = currentTestModel.ats.m_usHeatTemp;
            //Step4
            //检测水温，准备功率测试
            while (runStep != -1)
            {
                if (realWTemp[0] <= tempture)
                {
                    break;
                }
                QThread::msleep(500);
            }
            //检测退出
            if (runStep == -1) { emit sigAutoTestSend(-99, "人工退出测试"); upMesOutData();  QLOG_INFO() << "退出测试"; return; }
            //Step5
            //开启加热,同时检测水温到0度，检测有无故障
            emit sigAutoTestSend(10, "加热");
            QThread::msleep(10);

            QThread::msleep(3000);
            Error[0].clear();
            isRecordError = true;
            vot = getPowerResponed("MEAS:VOLT?\n");
            if (abs(vot - currentTestModel.ats.m_iRatedVolt) > 10)
            {
                emit sigAutoTestSend(11, "电源电压异常");
                up_mes_var.m_strOtherFault = "*电源输出电压与设置不一致";
                up_mes_var.m_strTestResult = "9";
                emit sigAutoTestSend(-99, "退出测试"); upMesOutData();  QLOG_INFO() << "退出测试"; return;
            }
            //Step6
            //等待到达0度，读取功率
            getAveragePW(currentTestModel.ats);
            //检测退出
            if (runStep == -1) { emit sigAutoTestSend(-99, "人工退出测试"); upMesOutData();  QLOG_INFO() << "退出测试"; return; }
            if (Error[0].size() != 0)
            {
                QList<QString> strlist = Error[0].toList();
                QString all;
                for (int g = 0; g < strlist.size(); g++)
                    all += "*" + strlist.at(g);
                emit sigAutoTestSend(11, all);
                up_mes_var.m_strOtherFault = all;
                up_mes_var.m_strTestResult = "9";
            }
            //检测退出
            if (runStep == -1) { emit sigAutoTestSend(-99, "人工退出测试"); upMesOutData();  QLOG_INFO() << "退出测试"; return; }
            //Step7
            //关闭冷水机循环，测试过温保护
            emit sigAutoTestSend(12, "过温保护测试中");
            QThread::msleep(500);
            Error[0].clear();
            isRecordError = true;
            while (runStep != -1)
            {

                bool b = false;
                if (realOTPro[0] == "过温" || realOTPro[0] == "出水口过温" || realOTPro[0] == "过温故障" || realOTPro[0] == "过温保护")
                {
                    b = true;
                }
                float outTemp = realWTemp[0];
                if (b)
                {
                    up_mes_var.m_strOverTempProtected = QString::number(outTemp);
                    break;
                }
                if (outTemp - currentTestModel.ats.m_iOverTemperature > currentTestModel.ats.m_iOverTempTolerance + 2)
                {
                    emit sigAutoTestSend(13, QString::number(outTemp) + "过温无保护NG");
                    up_mes_var.m_strOtherFault = "*过温无保护NG";
                    up_mes_var.m_strOverTempProtected = QString::number(outTemp);
                    up_mes_var.m_strTestResult = "9";
                    break;
                }
                QThread::msleep(500);
            }

            emit sigAutoTestSend(23, "关闭加热");
            if (!up_mes_var.m_strOtherFault.contains("过温无保护NG"))
            {
                emit sigAutoTestSend(13, up_mes_var.m_strOverTempProtected);
            }
            //检测退出
            if (runStep == -1) { emit sigAutoTestSend(-99, "人工退出测试"); upMesOutData();  QLOG_INFO() << "退出测试"; return; }
            //判断 runStep == -1 ?

            //Step8
            //开冷水机，降温
            isRecordError = true;

            QThread::msleep(500);
            emit sigAutoTestSend(14, "过温恢复测试中");

            //检测退出
            if (runStep == -1) { emit sigAutoTestSend(-99, "人工退出测试"); upMesOutData();  QLOG_INFO() << "退出测试"; return; }
            QThread::msleep(2500);
            //过温恢复
            while (runStep != -1)
            {
                bool b = false;
                if (realOTPro[0]=="正常"||(realOTPro[0] != "过温" && realOTPro[0] != "出水口过温" && realOTPro[0] != "过温故障" && realOTPro[0] != "过温保护"))
                {
                    b = true;
                }
                QThread::msleep(50);
                if (b)
                {
                    up_mes_var.m_strOverTempProtectedRe = QString::number(realWTemp[0]);
                    break;
                }
                //如果当前温度小于我设定的恢复温度，退出
                if (realWTemp[0] < currentTestModel.ats.m_iOverTempRe - currentTestModel.ats.m_iOverTempTolerance)
                {
                    up_mes_var.m_strOverTempProtectedRe = QString::number(realWTemp[0]);
                    break;
                }
                //QThread::msleep(500);
            }
            if (abs(currentTestModel.ats.m_iOverTempRe - realWTemp[0]) > currentTestModel.ats.m_iOverTempTolerance)
            {
                emit sigAutoTestSend(15, up_mes_var.m_strOverTempProtectedRe + "NG");
                up_mes_var.m_strTestResult = "9";
            }
            else
            {
                emit sigAutoTestSend(15, up_mes_var.m_strOverTempProtectedRe);
            }

            //检测退出
            if (runStep == -1) { emit sigAutoTestSend(-99, "人工退出测试"); upMesOutData();  QLOG_INFO() << "退出测试"; return; }
            //Step9
            //关闭使能，查看有无功率
            emit sigAutoTestSend(23, "关闭使能");
            QThread::msleep(2000);
            if (realPower[0] != 0.0)
            {
                emit sigAutoTestSend(11, "IGBT短路");
                up_mes_var.m_strOtherFault += "*IGBT短路";
                up_mes_var.m_strTestResult = "9";
            }


            emit sigAutoTestSend(18, up_mes_var.m_strTestResult);
            //Step9
            //保存测试结果,上传MES
            upMesOutData();
            //正常测试完成的
            if (runStep != -1)
            {
                emit sigAutoTestSend(36, "吹水");
                emit sigAutoTestSend(38, "开启进度条");
                int inp = 0;
                //开启进度条
                while (runStep != -1 && inp < 120)
                {
                    inp++;
                    //更新进度条
                    emit sigAutoTestSend(39, QString::number(inp));
                    Sleep(1000);
                }
            }
            QThread::msleep(500);
            emit sigAutoTestSend(-2, "冷水机开");
            emit sigAutoTestSend(-100, "条码输入开");
            //break;
        }//end big while
    }
}

