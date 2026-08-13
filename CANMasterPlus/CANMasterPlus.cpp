#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QTextBrowser>
#include <QTableWidget>
#include <QDir>
#include <QtDBus/qdbusmessage.h>
#include <QtDBus/qdbusconnection.h>
#include <QtConcurrent>
#include "CANMasterPlus.h"
#include "communicate.h"
#include "../ModelDataManage/modeldatamanage.h"
#include "messageview.h"
#include "RequestTableManage.h"
#include "ReceiveTableManage.h"
#include "RollShowManage.h"
#include "../DeviceIO/3rd/QsLog/include/QsLog.h"
#include "../DeviceIO/3rd/QsLog/include/QsLogDest.h"
#include "../../HILGUI/HilGui/HilGui.h"
#include "../ModelDataManage/Unities/qGboleData.h"
#include "SysSetting.h"
#include "Login.h"
#include <qthread.h>

//static variate assignment
QStringList CANMasterPlus::baudRate = {"125k/s","250k/s","500k/s","800k/s","2400b/s","9600b/s","10400b/s","19200b/s"};
QStringList CANMasterPlus::ptctype = {"Non","WPTC","APTC"};
QStringList CANMasterPlus::platform = {"Non","2kW","3kW","4kW","5kW","6kW","7kW","8kW","10kW","15kW","19kW","20kW","25kW"};
static QString standby_color = "background-color:rgb(250,250,0)";
static QString readly_color = "background-color:rgb(10,250,0)";
static QString noreapond_color = "background-color:rgb(250,10,0)";
using namespace QsLogging;
CANMasterPlus::CANMasterPlus(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    m_CommuniPtr.reset(new Communicate);
    m_connect_status_timer = new QTimer(this);
    connect(m_connect_status_timer, &QTimer::timeout, this, &CANMasterPlus::on_timeout_communicate);
    
    InitGUI();
    initLogger();
    QString path = QApplication::applicationDirPath() + "/app-logo.ico";
    this->setWindowIcon(QIcon(path));
    
    
    
}

CANMasterPlus::~CANMasterPlus()
{
    if (nullptr != modelManage)
    {
        delete modelManage;
        modelManage = nullptr;
    }
    HashArr.clear();
    destroyLogger();
}

void CANMasterPlus::logSlot(QString str, int level)
{
    if (QsLogging::Level::WarnLevel == level)
    {
        str = "<font color=\"#f1c232\">" + str + "</font>";
    }
    else if (QsLogging::Level::InfoLevel == level)
    {
        str = "<font color=\"#6aa84f\">" + str + "</font>";
    }
    else if (QsLogging::Level::ErrorLevel == level)
    {
        str = "<font color=\"#980000\">" + str + "</font>";
    }
    if (logBrower)
        logBrower->append(str);
}

void CANMasterPlus::tastItemInit()
{
    if (!testCaseConfigGui)return;
    const std::vector<struct testList>& list = testCaseConfigGui->getTestRunList();
    
    int row_c = testStatusTable->rowCount();
    for (int i = 0; i < row_c; i++)
        testStatusTable->removeRow(row_c - i - 1);

    testStatusTable->setRowCount(list.size() - 2);

    for (int i = 2; i < list.size(); i++)
        testStatusTable->setItem(i - 2, 0, new QTableWidgetItem(list.at(i).discription));

}

void CANMasterPlus::tastcastStatus(int index, int status)
{
    if (index<0 || index >testStatusTable->rowCount() - 1)
        return;
    QString res;
    QColor c;
    switch (status)
    {
    case 0:
        res = "Fali"; c = QColor(255,0,0); break;
    case 1:
        res = "Pass"; c = QColor(0, 255, 0); break;
    case -1:
        res = "Test break"; QColor(255, 255, 0); break;
    default:
        break;
    }
    auto it = testStatusTable->item(index, 1);
    if (!it)
    {
        testStatusTable->setItem(index, 1, new QTableWidgetItem(res));
        it = testStatusTable->item(index, 1);
    }
    it->setBackgroundColor(c);
    it->setText(res);
}

void CANMasterPlus::on_timeout_communicate()
{
    m_connect_status.setText(tr("Loss"));
    m_connect_status.setStyleSheet(noreapond_color);
}

void CANMasterPlus::on_actionDataSave_triggered()
{
    if (!sysSetGui)
    {
        sysSetGui = std::make_shared<SysSetting>();
    }
    sysSetGui->show();
}

void CANMasterPlus::on_actionLogin_triggered()
{
    if (!loginGui)
    {
        loginGui = std::make_shared<Login>();
        connect(loginGui.get(), &Login::sigUserTypeChanged, this, &CANMasterPlus::on_actionUserTypeChanged);
    }
    loginGui->show();
}

void CANMasterPlus::on_actionLogout_triggered()
{
    if (!loginGui)
        return;
    loginGui->setUserType(0);
}

void CANMasterPlus::on_actionUserTypeChanged(int usertype)
{
    if (0 == usertype)
    {
        //pbModelManage->setEnabled(false);
        this->ui.actionProtocolEdit->setEnabled(false);
        this->ui.actionDataSave->setEnabled(false);
    }
    else
    {
        //pbModelManage->setEnabled(true);
        this->ui.actionProtocolEdit->setEnabled(true);
        this->ui.actionDataSave->setEnabled(true);
    }
}

void CANMasterPlus::on_actionProtocolEdit_triggered()
{
    if (nullptr == modelManage)
    {
        modelManage = new ModelDataManage();
    }
    modelManage->show();
}

void CANMasterPlus::on_actionTestCaseEdit_triggered()
{
    if (!testCaseConfigGui)
    {
        testCaseConfigGui = new HilGui(this);
        connect(testCaseConfigGui, &HilGui::upTestCaseItem, this, &CANMasterPlus::tastItemInit);
        connect(testCaseConfigGui, &HilGui::upTestCaseItemResult, this, &CANMasterPlus::tastcastStatus);
    }

    testCaseConfigGui->show();
}

void CANMasterPlus::on_actionPluginDevices_triggered()
{
    if (!tp700)
    {
        tp700 = new TP700TemperatureModel();
        this->recTableManage->setTeamperaturePointer(tp700);
        this->rollTabManage->setTeamperaturePointer(tp700);
    }
    tp700->show();
}

void CANMasterPlus::on_actionHistory_triggered()
{
    // 创建一个对话框
    QDialog dialog(this);
    dialog.setWindowTitle(tr("History Version"));

    dialog.resize(800, 600);
    // 创建一个布局
    QVBoxLayout* layout = new QVBoxLayout(&dialog);

    // 创建一个标签
    QLabel* label = new QLabel(tr("History Version Details:"));
    layout->addWidget(label);

    // 创建一个文本编辑框用于显示文件内容
    QTextEdit* textEdit = new QTextEdit();
    textEdit->setReadOnly(true); // 设置为只读
    layout->addWidget(textEdit);

    // 读取文件内容并显示在文本编辑框中
    QFile F(QApplication::applicationDirPath() + "/Data/history_version.kus");
    if (F.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&F);
        QString content = in.readAll();
        textEdit->setText(content);
        F.close();
    }
    else
    {
        textEdit->setText(tr("History Version Loss"));
    }

    // 创建一个按钮用于关闭对话框
    QPushButton* closeButton = new QPushButton(tr("Close"));
    layout->addWidget(closeButton);

    // 连接按钮的点击信号到对话框的关闭槽
    QObject::connect(closeButton, &QPushButton::clicked, &dialog, &QDialog::close);

    // 显示对话框
    dialog.exec();
}

void CANMasterPlus::on_actionCalculate_triggered()
{
    //mdFiveCalculate* thisMdFiveCalculate = new mdFiveCalculate(this);
    //thisMdFiveCalculate->show();
}

void CANMasterPlus::InitGUI()
{
    //******************top tools bar
    
    //devcies group
    QLabel* labelSeletcDev = new QLabel(this);
    labelSeletcDev->setText(tr("DevType:"));
    this->comboxDevType = new QComboBox(this);
    QLabel* labelChannel = new QLabel(this);
    labelChannel->setText(tr(" Channel:"));
    this->comboxDevChannel = new QComboBox(this);
    this->pbRefreshDev = new QPushButton(this);
    this->pbRefreshDev->setText(tr("<-Refresh"));

    QLabel* labelBaudRate = new QLabel(this);
    labelBaudRate->setText(tr("BaudRate:"));
    this->comboxBaudRate = new QComboBox(this);
    this->pbOpenDevice = new QPushButton(this);
    this->pbOpenDevice->setCheckable(true);
    this->pbOpenDevice->setText(tr("Connect"));
    this->pbSendMsg = new QPushButton(this);
    this->pbSendMsg->setText(tr("Send"));
    this->pbSendMsg->setCheckable(true);
    m_connect_status.setText(tr("standby"));
    m_connect_status.setStyleSheet(standby_color);
    m_connect_status.setMinimumWidth(80);
    QHBoxLayout* devLayout = new QHBoxLayout(this);
    devLayout->addWidget(labelSeletcDev);
    devLayout->addWidget(comboxDevType);
    devLayout->addWidget(labelChannel);
    devLayout->addWidget(comboxDevChannel);
    devLayout->addWidget(pbRefreshDev);
    devLayout->addWidget(labelBaudRate);
    devLayout->addWidget(comboxBaudRate);
    devLayout->addWidget(pbOpenDevice);
    devLayout->addWidget(pbSendMsg);
    devLayout->addWidget(&m_connect_status);
    QGroupBox* devGrounp = new QGroupBox(this);
    devGrounp->setLayout(devLayout);
    devGrounp->setContentsMargins(0, 0, 0, 0);
    devGrounp->setFixedHeight(50);

    //model group
    this->comboxPlatform = new QComboBox(this);
    this->comboxModel = new QComboBox(this);

    //this->comboxPTCType = new QComboBox(this);
    QHBoxLayout* modelLayout = new QHBoxLayout(this);
    //modelLayout->addWidget(new QLabel(tr("PTCType:")));
    //modelLayout->addWidget(comboxPTCType);
    modelLayout->addWidget(new QLabel(tr("Platform:")));
    modelLayout->addWidget(comboxPlatform);
    modelLayout->addWidget(new QLabel(tr(" Model:")));
    modelLayout->addWidget(comboxModel);

    this->devLineEdit = new QLineEdit(this);
    this->devLineEdit->setMinimumSize(100, 30);
    this->pbSearch = new QPushButton(this);
    this->pbSearch->setText("Search");
    this->pbPicture = new QPushButton(this);
    this->pbPicture->setText("SaveJPG");
    modelLayout->addWidget(devLineEdit);
    modelLayout->addWidget(pbSearch);
    modelLayout->addWidget(pbPicture);
    QGroupBox* modelGrounp = new QGroupBox(this);
    modelGrounp->setLayout(modelLayout);
    connect(pbSearch, &QPushButton::clicked, this, &CANMasterPlus::on_pbSearch_clicked);
    connect(pbPicture, &QPushButton::clicked, this, &CANMasterPlus::on_pbPicture_clicked);
    this->ui.actionProtocolEdit->setEnabled(false);

    
    modelGrounp->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout* topToolsBarLayout = new QVBoxLayout(this);
    topToolsBarLayout->addWidget(modelGrounp);
    topToolsBarLayout->addWidget(devGrounp);
    topToolsBarLayout->addSpacerItem(new QSpacerItem(100, 10, QSizePolicy::MinimumExpanding));

    
    QStringList devList = m_CommuniPtr->devTypeList();
    for(auto x:devList)
    {
        this->comboxDevType->addItem(x);
        //set item data
        this->comboxDevType->setItemData(this->comboxDevType->count()-1,QVariant(x));
    }
    QStringList dl = m_CommuniPtr->devList();
    this->comboxDevChannel->addItems(dl);
    
    for(QString x:baudRate)
    {
        this->comboxBaudRate->addItem(x);
        int baud =  x.left(x.length()-2).toInt();
        //set item data,it's very useful to get item data
        this->comboxBaudRate->setItemData(this->comboxBaudRate->count()-1,baud);
    }
    this->comboxPlatform->addItems(platform);
    //this->comboxPTCType->addItems(ptctype);
    qGboleData* qgb = qGboleData::getInstance();
    if (qgb)
    {
        if (!qgb->getIsInit())
        {
            qgb->read();
        }
        for (int i = 0; i < qgb->pGboleData.size(); i++)
        {
            this->comboxModel->addItem(qgb->pGboleData.at(i).modelName,i);
        }
        if (qgb->pGboleData.size() > 0)
        {
            this->comboxBaudRate->setCurrentIndex(qgb->pGboleData.at(0).bundRate);
            m_currModelData = qgb->pGboleData.at(0);
        }
     
    }
    this->comboxModel->setMinimumWidth(0);
    //************End top bar*********************//

    //************second right*******************//
    messageview = new MessageView(this);
    logBrower = new QTextBrowser(this);
    logStatusTab = new QTabWidget(this);
    testStatusTable = new QTableWidget(this);
    testStatusTable->setColumnCount(2);
    QStringList tst{ "TestCase","Result" };
    testStatusTable->setHorizontalHeaderLabels(tst);
    logStatusTab->addTab(logBrower, tr("log"));
    logStatusTab->addTab(testStatusTable, tr("testStatus"));
    logStatusTab->setCurrentIndex(1);
    QVBoxLayout* secondRightLayout = new QVBoxLayout(this);
    /*secondRightLayout->addWidget(messageview->getTabWidget());
    secondRightLayout->addWidget(logStatusTab);*/
    QSplitter* splitter_r = new QSplitter();
    splitter_r->setOrientation(Qt::Vertical);
    splitter_r->addWidget(messageview->getTabWidget());
    splitter_r->addWidget(logStatusTab);
    splitter_r->setStretchFactor(0, 3);
    splitter_r->setStretchFactor(1, 7);
    secondRightLayout->addWidget(splitter_r);

    reqTableManage = new RequestTableManage(this);
    QHBoxLayout* midleLayout = new QHBoxLayout(this);
    midleLayout->addWidget(reqTableManage->getTableWidget());
    midleLayout->setMargin(0);
    recTableManage = new ReceiveTableManage(this);
    QHBoxLayout* secondLayout = new QHBoxLayout(this);
    secondLayout->addWidget(recTableManage->getTableWidget());
    secondLayout->setMargin(0);
    rollTabManage = new RollShowManage(this);
    QToolButton* savedata = new QToolButton();
    savedata->setIcon(QIcon(":/CANMasterPlus/save.ico"));
    savedata->setToolTip(tr("Save Data"));
    connect(savedata, &QToolButton::clicked, this, &CANMasterPlus::on_pbSaveData_clicked);
    //savedata->setStyleSheet(":/CANMasterPlus/Qtoolbuuton.qss");
    QToolButton* cleardata = new QToolButton();
    cleardata->setIcon(QIcon(":/CANMasterPlus/clear.ico"));
    cleardata->setToolTip(tr("Clear UI Data"));
    connect(cleardata, &QToolButton::clicked, this, &CANMasterPlus::on_pbClearData_clicked);
    //cleardata->setStyleSheet(":/CANMasterPlus/Qtoolbuuton.qss");
    QHBoxLayout* b_layout = new QHBoxLayout();
    b_layout->addWidget(savedata);
    b_layout->addWidget(cleardata);
    b_layout->addSpacerItem(new QSpacerItem(200, 50, QSizePolicy::Expanding));

    QVBoxLayout* BottomLayout = new QVBoxLayout(this);
    BottomLayout->addLayout(b_layout);
    BottomLayout->addWidget(rollTabManage->getRollShowLayout());
    BottomLayout->setMargin(0);
    BottomLayout->setContentsMargins(0, 0, 0, 0);
    QWidget* w1 = new QWidget();
    QWidget* w2 = new QWidget();
    QWidget* w3 = new QWidget();

    w1->setLayout(midleLayout);
    //w1->setStyleSheet("background-color:#0078d7");
    w2->setLayout(secondLayout);
    w3->setLayout(BottomLayout);
    
    //w3->setContentsMargins(0, 0, 0, 0);
    QSplitter* splitter = new QSplitter();
    splitter->addWidget(w1);
    splitter->addWidget(w2);
    splitter->addWidget(w3);
    splitter->setOrientation(Qt::Vertical);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 5);
    splitter->setStretchFactor(2, 5);
    splitter->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout* secondLeftLayout = new QVBoxLayout(this);
    secondLeftLayout->addWidget(splitter);

    
    //The interface on the left and right sides below

    QWidget* wl=new QWidget();
    QWidget* wr=new QWidget();

    QHBoxLayout* downMidelLayout = new QHBoxLayout(this);
    wl->setLayout(secondLeftLayout);
    secondLeftLayout->setSpacing(0);
    wr->setLayout(secondRightLayout);
    QSplitter* splitter_second = new QSplitter();
    splitter_second->addWidget(wl);
    splitter_second->addWidget(wr);
    splitter_second->setStretchFactor(0, 6);
    splitter_second->setStretchFactor(1, 4);
    /*downMidelLayout->addLayout(secondLeftLayout);
    downMidelLayout->addLayout(secondRightLayout);*/
    downMidelLayout->addWidget(splitter_second);
    splitter_second->setOpaqueResize(false);
    splitter_second->setHandleWidth(1); // 设置分割条的宽度
    //global layout
    QVBoxLayout* globalLayout = new QVBoxLayout(this);
    globalLayout->addLayout(topToolsBarLayout);
    globalLayout->addLayout(downMidelLayout);

    globalLayout->setStretch(0, 1);
    globalLayout->setStretch(1, 10);

    this->centralWidget()->setLayout(globalLayout);

    QFile f(":/CANMasterPlus/Qtoolbuuton.qss");
    if (f.open(QFile::ReadOnly))
    {
        this->setStyleSheet(f.readAll());
    }
    ConnectWidget();
   
    qGboleData* qGb = qGboleData::getInstance();
    if (!qGb)return;
    HashArr.clear();
    for (int i = 0; i < qGb->pGboleData.size(); i++)
    {
        HashArr.push_back(qGb->pGboleData.at(i).modelName);
    }

    this->ui.actionDataSave->setEnabled(false);
}

void CANMasterPlus::ConnectWidget()
{
    connect(comboxDevType, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CANMasterPlus::on_comboxDevType_IndexChanged);
    connect(comboxDevChannel, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CANMasterPlus::on_comboxDevChannel_IndexChanged);
    connect(comboxBaudRate, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CANMasterPlus::on_comboxBaudRate_IndexChanged);
    connect(comboxPlatform, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CANMasterPlus::on_comboxPlatform_IndexChanged);
    connect(comboxModel, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CANMasterPlus::on_comboxModel_IndexChanged);
    connect(this, &CANMasterPlus::sigCurrModelChanged, reqTableManage, &RequestTableManage::on_currentModelIndex_Changed);
    connect(this, &CANMasterPlus::sigCurrModelChanged, recTableManage, &ReceiveTableManage::on_currentModelIndex_Changed);
    connect(this, &CANMasterPlus::sigCurrModelChanged, rollTabManage, &RollShowManage::on_currentModelIndex_Changed);
    connect(recTableManage, &ReceiveTableManage::on_sigClearUIData, rollTabManage, &RollShowManage::on_slotClearData);
    connect(pbRefreshDev, &QPushButton::clicked, this, &CANMasterPlus::on_pbRefreshDev_Clicked);
    connect(pbOpenDevice, &QPushButton::clicked, this, &CANMasterPlus::on_pbOpenDevice_Clicked);
    connect(pbSendMsg, &QPushButton::clicked, this, &CANMasterPlus::on_pbSendMsg_Clicked);

    QDBusConnection::sessionBus().connect("com.Service.TestCaseRun", "/home/hil/object", "com.TestCaseRun.value", "onSigNewMessage", this, SLOT(transferMsg(int,unsigned int,QByteArray)));
    if (!testCaseConfigGui)
    {
        testCaseConfigGui = new HilGui(this);
        connect(testCaseConfigGui, &HilGui::upTestCaseItem, this, &CANMasterPlus::tastItemInit);
        connect(testCaseConfigGui, &HilGui::upTestCaseItemResult, this, &CANMasterPlus::tastcastStatus);
    }
    testCaseConfigGui->shootCallback(recTableManage->getTableWidget(),0);
    testCaseConfigGui->shootCallback(messageview->getTabWidget(), 1);
    testCaseConfigGui->shootClearCallback(messageview);
}

void CANMasterPlus::initLogger()
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
    //这个是文件流，搞到文件里面去的
    DestinationPtr fileDestination(DestinationFactory::MakeFileDestination(
        pp, EnableLogRotation, MaxSizeBytes(1024 * 10), MaxOldLogCount(100)));
    //这个是调试时输出的
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
}

void CANMasterPlus::destroyLogger()
{
    Logger::destroyInstance();
}

void CANMasterPlus::on_comboxDevChannel_IndexChanged(int index)
{
}


void CANMasterPlus::on_pbPicture_clicked()
{
    // 获取并创建图片保存目录
    QString picturesDir = QApplication::applicationDirPath() + "/Pictures";
    QDir dir(picturesDir);
    if (!dir.exists())
    {
        if (!dir.mkpath(picturesDir)) {
            qWarning() << "Failed to create directory:" << picturesDir;
            return;
        }
    }

    // 获取主屏幕
    QScreen* screen = QGuiApplication::primaryScreen();
    if (!screen) {
        qWarning("Unable to get primary screen");
        return;
    }

    // 捕获屏幕截图
    QPixmap screenshot = screen->grabWindow(0);
    if (screenshot.isNull()) {
        qWarning("Failed to capture screenshot");
        return;
    }

    // 生成文件名和完整路径
    QString fileName = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmsszzz") + ".png";
    QString filePath = QDir(picturesDir).filePath(fileName);

    // 保存截图
    if (!screenshot.save(filePath, "PNG")) {
        qWarning() << "Failed to save screenshot to:" << filePath;
        return;
    }

    qDebug() << "Screenshot saved successfully:" << filePath;

    // 按钮变红
    this->pbPicture->setStyleSheet("background-color: red;");

    // 500ms后恢复按钮颜色
    QTimer::singleShot(500, this, [this]() {
        pbPicture->setStyleSheet(""); // 恢复默认样式
        });
}

void CANMasterPlus::on_pbSearch_clicked()
{
    if (this->pbOpenDevice->isChecked())
    {
        QMessageBox::warning(this,
            tr("InPut Error"),
            tr("Please disconnect the device first!"),
            QMessageBox::Ok);
        return;
    }
    QString devName = this->devLineEdit->text().trimmed();
    if (devName.isEmpty())
    {
        // 使用QMessageBox提示用户输入为空
        QMessageBox::warning(this,
            tr("InPut Error"),
            tr("Model name cannot be empty, please enter model name!"),
            QMessageBox::Ok);

        // 可选：将焦点设置回输入框
        this->devLineEdit->setFocus();
        return;
    }
    qDebug() << "Searching for device:" << devName;
    QStringList currentNameList;
    for (auto it= HashArr.begin();it!= HashArr.end();it++)
    {
        if (it->contains(devName, Qt::CaseInsensitive))
        {
            currentNameList.push_back(*it);
        }
    }
    qDebug() << currentNameList.size();
    reGetPTCType(currentNameList);
    
}

void CANMasterPlus::reGetPTCType(QStringList ls)
{
   
    //this->comboxModel->addItems(ls);
    this->comboxModel->blockSignals(true);
    this->comboxModel->clear();

    qGboleData* qgb = qGboleData::getInstance();
    if (!qgb)return;
    QString plat = this->comboxPlatform->currentText();
    int model_item_data_index = 0;
    for (int i = 0; i < qgb->pGboleData.size(); i++)
    {
        if (ls.contains(qgb->pGboleData.at(i).modelName, Qt::CaseInsensitive))
        {
            this->comboxModel->addItem(qgb->pGboleData.at(i).modelName);
            //这个很重要，把当前的型号下标放到这个选项去
            this->comboxModel->setItemData(model_item_data_index++, QVariant(i));
        }
    }
    //获取第一个选项的型号下标
    int m_index = this->comboxModel->itemData(0).toInt();

    if (m_index < 0 || m_index >= qgb->pGboleData.size())
        return;
    //波特率选项也要更新
    this->comboxBaudRate->setCurrentIndex(qgb->pGboleData.at(m_index).bundRate);
    this->on_comboxModel_IndexChanged(0);
    this->comboxModel->blockSignals(false);
}

void CANMasterPlus::on_comboxModel_IndexChanged(int index)
{
    int i = this->comboxModel->itemData(index).toInt();
    if (i < 0)
        return;
    
    qGboleData* qgb = qGboleData::getInstance();
    if (!qgb)return;
    if (!qgb->getIsInit())qgb->read();
    this->m_currModelData = qgb->pGboleData.at(i);

    //设置界面上的波特率
    this->comboxBaudRate->setCurrentIndex(qgb->pGboleData.at(i).bundRate);
    //广播给其它组件，型号变化了
    emit sigCurrModelChanged(i);
}
void CANMasterPlus::on_comboxDevType_IndexChanged(int index)
{
    on_pbRefreshDev_Clicked(true);
}
void CANMasterPlus::on_pbRefreshDev_Clicked(bool b)
{
    QStringList devList = m_CommuniPtr->setDevType(comboxDevType->currentIndex());
    int cout = comboxDevChannel->count();
    for (int i = 0; i <cout; i++)
        comboxDevChannel->removeItem(cout-i-1);
    comboxDevChannel->addItems(devList);

}

void CANMasterPlus::on_comboxBaudRate_IndexChanged(int index)
{

}

void CANMasterPlus::on_pbOpenDevice_Clicked(bool b)
{
    if(b)
    {
        DeviceParameter dev;
        dev.baudRate = comboxBaudRate->currentIndex();
        dev.devType = comboxDevType->currentIndex();
        dev.devChannel = comboxDevChannel->currentIndex();
        if (dev.devType == 8)//tomosslin需要先知道PTC发的ID
        {
            std::vector<int>idvec;
            int i = 0;
            for (std::vector<QString>::iterator it = HashArr.begin(); it != HashArr.end(); it++)
            {
                if (*it == this->comboxModel->currentText())
                {
                    break;
                }
                i++;
            }
            qGboleData* qGb = qGboleData::getInstance();
            if (!qGb)return;
            /*int index = this->comboxModel->currentIndex();

            if (index > qGb->pGboleData.size() - 1)
                return;*/
            dev.devModel = i;
            //int real = HashArr[index];
            for (int k = 0; k < qGb->pGboleData.at(dev.devModel).cItem.size(); k++)
            {
                if (!qGb->pGboleData.at(dev.devModel).cItem.at(k).opt)
                    idvec.push_back(qGb->pGboleData.at(dev.devModel).cItem.at(k).strCanId.toInt(nullptr, 16));
            }
            dev.v = idvec;
        }
        if (!m_CommuniPtr->openDevice(dev))
        {
            QLOG_WARN() << "Open" << comboxDevType->currentText() << "fail!";
            this->pbOpenDevice->setChecked(false);
            return;
        }
        QLOG_INFO() << "Open" << comboxDevType->currentText() << "successed!";
        connect(m_CommuniPtr.data(), &Communicate::on_sendNewMsg, this, &CANMasterPlus::transferMsg);
    }
    else
    {
        disconnect(m_CommuniPtr.data(), &Communicate::on_sendNewMsg, this, &CANMasterPlus::transferMsg);
        m_CommuniPtr->closeDevice();
    }
}

void CANMasterPlus::on_pbSendMsg_Clicked(bool b)
{
    if (b)
    {
        m_connect_status_timer->start(communicate_loss_time);
        //LIN
        if (m_currModelData.isLIN)
        {
            if (!this->m_timerSingelCycle)
            {
                this->m_timerSingelCycle = new QTimer(this);
                connect(this->m_timerSingelCycle, &QTimer::timeout, this, &CANMasterPlus::on_timeout_SendMsg);
            }
            this->m_timerSingelCycle->start(this->m_currModelData.circle);
        }
        //CAN
        else
        {
            this->m_multiCycle = 0;
            for (int i = 0; i < m_currModelData.cItem.size(); i++)
            {
                if (m_currModelData.cItem.at(i).circle != -1)
                {
                    this->m_multiCycle++;
                }
            }
            if (this->m_multiCycle > 0)
            {
                for (int i = 0; i < m_currModelData.cItem.size(); i++)
                {
                    if (m_currModelData.cItem.at(i).opt == 1 && m_currModelData.cItem.at(i).circle != -1)
                    {
                        QTimer* t = new QTimer(this);
                        t->setTimerType(Qt::TimerType::PreciseTimer);
                        connect(t, &QTimer::timeout, this, &CANMasterPlus::on_timeout_SendMsg);
                        m_currModelData.cItem.at(i).timeAdd = t;
                        auto _m = reqTableManage->getReqArray();
                        for (auto v=_m->begin();v!=_m->end();v++)
                        {
                            if (v.key() == m_currModelData.cItem.at(i).strCanId.toInt(nullptr, 16))
                            {
                                v.value().t = t;
                                break;
                            }
                        }
                        t->start(m_currModelData.cItem.at(i).circle);
                    }
                }
            }
            else
            {
                if (!this->m_timerSingelCycle)
                {
                    this->m_timerSingelCycle = new QTimer(this);
                    connect(this->m_timerSingelCycle, &QTimer::timeout, this, &CANMasterPlus::on_timeout_SendMsg);
                }
                this->m_timerSingelCycle->start(this->m_currModelData.circle);
            }
        }
    }
    else
    {
        m_connect_status_timer->stop();
        m_connect_status.setText(tr("standby"));
        m_connect_status.setStyleSheet(standby_color);
        //单个周期的
        if (this->m_timerSingelCycle)this->m_timerSingelCycle->stop();

        //多个周期的
        if (this->m_multiCycle > 0)
        {
            for (int i = 0; i < m_currModelData.cItem.size(); i++)
            {
                if (m_currModelData.cItem.at(i).opt == 1 && m_currModelData.cItem.at(i).circle != -1)
                    if (m_currModelData.cItem.at(i).timeAdd != nullptr)
                    {
                        m_currModelData.cItem.at(i).timeAdd->stop();
                        delete  m_currModelData.cItem.at(i).timeAdd;
                        m_currModelData.cItem.at(i).timeAdd = nullptr;
                    }

            }
        }
    }
}

void CANMasterPlus::on_comboxPlatform_IndexChanged(int index)
{
    this->comboxModel->blockSignals(true);
    this->comboxModel->clear();

    qGboleData* qgb = qGboleData::getInstance();
    if (!qgb)return;
    QString plat = this->comboxPlatform->currentText();
    int model_item_data_index = 0;
    for (int i = 0; i < qgb->pGboleData.size(); i++)
    {
        if (qgb->pGboleData.at(i).sPlatform.toUpper().contains(plat.toUpper())|| plat.toUpper()=="NON")
        {
            this->comboxModel->addItem(qgb->pGboleData.at(i).modelName);
            //这个很重要，把当前的型号下标放到这个选项去
            this->comboxModel->setItemData(model_item_data_index++, QVariant(i));
        }
    }
    //获取第一个选项的型号下标
    int m_index = this->comboxModel->itemData(0).toInt();

    if (m_index < 0 || m_index >= qgb->pGboleData.size())
        return;
    //波特率选项也要更新
    this->comboxBaudRate->setCurrentIndex(qgb->pGboleData.at(m_index).bundRate);
    this->on_comboxModel_IndexChanged(0);
    this->comboxModel->blockSignals(false);
}

void CANMasterPlus::on_pbModelManage_clicked(bool b)
{
    if (nullptr == modelManage)
    {
        modelManage = new ModelDataManage();
    }
    modelManage->show();
}

void CANMasterPlus::on_pbTestCaseConfigManage_clicked(bool b)
{
    
    if (!testCaseConfigGui)
    {
        testCaseConfigGui = new HilGui(this);
        connect(testCaseConfigGui, &HilGui::upTestCaseItem, this, &CANMasterPlus::tastItemInit);
        connect(testCaseConfigGui, &HilGui::upTestCaseItemResult, this, &CANMasterPlus::tastcastStatus);
    }

    testCaseConfigGui->show();
}

//void CANMasterPlus::transferMsg(int ch, unsigned int frame_id, QByteArray data)
//{
//    QMutexLocker looock(&this->m_mutex);
//    MessageStr tms;
//    tms.isFresh = true;
//    tms.msg = data;
//
//    this->m_msgMap.insert(frame_id, tms);
//
//    return;
//    m_connect_status.setText(tr("connected"));
//    m_connect_status.setStyleSheet(readly_color);
//    m_connect_status_timer->start(communicate_loss_time);
//    recTableManage->transferMsg(ch, frame_id, data);
//    rollTabManage->transferMsg(ch, frame_id, data);
//    if (messageview)
//    {
//        messageview->newMsgReceive(frame_id, data, 0);
//    }
//}

void CANMasterPlus::transferMsg(int ch, unsigned int frame_id, QByteArray data)
{
    m_connect_status.setText(tr("connected"));
    m_connect_status.setStyleSheet(readly_color);
    m_connect_status_timer->start(communicate_loss_time);
    recTableManage->transferMsg(ch, frame_id, data);
    rollTabManage->transferMsg(ch, frame_id, data);
    if (messageview)
    {
        messageview->newMsgReceive(frame_id, data, 0);
    }
}

void CANMasterPlus::on_timeout_SendMsg()
{
    QTimer* timer = dynamic_cast<QTimer*>(sender());
    //LIN
    if (this->m_currModelData.isLIN)
    {
        //QtConcurrent::run([&]()
        {
            auto _m = reqTableManage->getReqArray();
            for (auto v = _m->begin(); v != _m->end(); v++)
            {
                m_CommuniPtr->sendData(this->m_dev_channel, v.key(), v.value().d, m_currModelData.bStandardId,v.value().r_len, DeviceIO::Master);
                if (messageview)
                {
                    messageview->newMsgReceive(v.key(), v.value().d, v.value().r_len, 1);
                }
                QThread::msleep(this->m_iLINDelayTime);
            }
            uchar temp_d[8]{ 0 };
            for (int k = 0; k < m_currModelData.cItem.size(); k++)
            {
                if (!m_currModelData.cItem.at(k).opt)
                {
                    m_CommuniPtr->sendData(this->m_dev_channel, m_currModelData.cItem.at(k).strCanId.toUInt(nullptr,16), temp_d, m_currModelData.bStandardId, m_currModelData.cItem.at(k).len, DeviceIO::Slave);
                    QThread::msleep(this->m_iLINDelayTime);
                }
            }
        };
    }
    //CAN
    else
    {
        if (this->m_multiCycle > 0)
        {
            auto _m = reqTableManage->getReqArray_Pure();
            for (auto v = _m->begin(); v != _m->end(); v++)
            {
                QTimer* t = dynamic_cast<QTimer*> (sender());
                if (t != v.value().t)
                    continue;
                reqTableManage->doSelfOperator(v.key());
                m_CommuniPtr->sendData(this->m_dev_channel, v.key(), v.value().d, m_currModelData.bStandardId);
                if (messageview)
                {
                    messageview->newMsgReceive(v.key(), v.value().d,8, 1);
                }
                break;
            }
        }
        else
        {
            if (!reqTableManage)return;
            auto _m = reqTableManage->getReqArray();
            for (auto v=_m->begin();v!=_m->end();v++)
            {
                m_CommuniPtr->sendData(this->m_dev_channel, v.key(), v.value().d, m_currModelData.bStandardId);
                if (messageview)
                {
                    messageview->newMsgReceive(v.key(), v.value().d, 8, 1);
                }
                QEventLoop loop;
                QTimer timer;
                connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
                timer.start(10); // 50毫秒后触发timeout信号
                loop.exec(); // 等待50毫秒，然后退出事件循环

            }
        }
    }
}

void CANMasterPlus::on_pbSaveData_clicked()
{
    if (this->recTableManage)
    {
        this->recTableManage->saveCanData();
    }
    this->rollTabManage->clearData();
}

void CANMasterPlus::on_pbClearData_clicked()
{
    this->recTableManage->clearData();
    this->rollTabManage->clearData();
}
