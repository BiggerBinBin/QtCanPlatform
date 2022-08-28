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
QtCanPlatform::QtCanPlatform(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    initUi();
    sendTimer = new QTimer();
    connect(sendTimer, &QTimer::timeout, this, &QtCanPlatform::sendData);
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
}

void QtCanPlatform::closeEvent(QCloseEvent* event)
{
    if (pcan)
    {
        pcan->CloseCan();
    }
    event->accept();
}

void QtCanPlatform::initUi()
{
    //这个是显示内容的
    tableView = new QTableWidget();
    tableView->setColumnCount(6);
    QStringList header;
    header << tr("操作")<<tr("数值") << tr("地址") << tr("名称") << tr("起止字节") << tr("起止位") << tr("长度");
    tableView->setHorizontalHeaderLabels(header);
    connect(tableView, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(on_tableDoubleClicked(int, int)));
    textBrowser = new QTextBrowser();
    initData();
    QComboBox* cbSelectModel = new QComboBox();
    
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
    QLabel* mLabel = new QLabel();
    mLabel->setText(tr("当前型号"));
    hLayout->addWidget(mLabel);
    hLayout->addWidget(cbSelectModel);

    tableRecView = new QTableWidget();
    //设置表格为10列，不加这个内容不会显示的
    tableRecView->setColumnCount(10);
    //定义一个垂直布局
    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->addLayout(hLayout);
    vLayout->addWidget(tableView);
    vLayout->addWidget(tableRecView);
    vLayout->addWidget(textBrowser);
    ui.centralWidget->setLayout(vLayout);

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
            QCheckBox* cb = new QCheckBox();
            int cr = tableView->rowCount();
            tableView->setRowCount(cr+1);
            tableView->setCellWidget(cr, 0, cb);
            QString mt = "0x"+QString("%1").arg(sendCanData.at(i).CanId, QString::number(sendCanData.at(i).CanId).length(), 16).toUpper().trimmed();
            tableView->setItem(cr, 1, new QTableWidgetItem("0"));
            tableView->setItem(cr, 2, new QTableWidgetItem(mt));
            tableView->setItem(cr, 3, new QTableWidgetItem(sendCanData.at(i).pItem.at(j).bitName));
            tableView->setItem(cr, 4, new QTableWidgetItem(QString::number(sendCanData.at(i).pItem.at(j).startByte)));
            tableView->setItem(cr, 5, new QTableWidgetItem(QString::number(sendCanData.at(i).pItem.at(j).startBit)));
            tableView->setItem(cr, 6, new QTableWidgetItem(QString::number(sendCanData.at(i).pItem.at(j).bitLeng)));
        }
   }
    return true;
}

bool QtCanPlatform::recDataIntoTab()
{
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

    for (int i = 0; i < recCanData.size(); i++)
    {
        int num = recCanData.at(i).pItem.size();
        int cr = tableRecView->rowCount();
        for (int j = 0; j <num; j++)
        {

            //每5个换一行
            if ((j + 1) % 6 == 0)
                ++cr;
            //每加一行就要设置到表格去
            tableRecView->setRowCount(cr + 1);
            tableRecView->setItem(cr, 2*(j%5), new QTableWidgetItem(recCanData.at(i).pItem.at(j).bitName));
            
        }
    }
    return true;
}
void QtCanPlatform::sendData()
{
    uchar s_Data[8];
    memset(s_Data, 0, 8 * sizeof(uchar));
    for (int i = 0; i < sendCanData.size(); i++)
    {
        unsigned int fream_id;
        bool b= intelProtocol(sendCanData.at(i), s_Data, fream_id);
        if (!b)
            continue;
        pcan->SendFrame(fream_id, s_Data);
    }

}
bool QtCanPlatform::intelProtocol(canIdData& cdata,uchar data[], unsigned int& fream_id)
{
    if (cdata.pItem.size() <= 0)
        return false;
    fream_id = cdata.CanId;
    for (int i = 0; i < cdata.pItem.size() && i < 8; i++)
    {
        const protoItem &itemp = cdata.pItem.at(i);
        int startbyte = itemp.startByte;
        int startbit = itemp.startBit;
        int lengght = itemp.bitLeng;
        int senddd = itemp.send;
        if (lengght <= 8)
        {
            int pos = startbit % 8;             //起止位，模8，1字节8位，uchar是1节长度的
            uchar m_send = senddd << pos &0xff; //左移起止位，再&0xff，保证数据是不超过255
            data[i] += m_send;                  //加上去，有可能其它的数据也在这个字节里
        }
        else if(lengght<=16)
        {
            int pos = startbit % 8;
            uchar m_send = senddd << pos & 0xff; //低8位
            data[i] += m_send;
            m_send = senddd >> 8 & 0xff;         //高8位
            data[i+1] += m_send;
        }
    }
    return true;
}
bool QtCanPlatform::motoProtocol(canIdData& cdata,uchar data[], unsigned int& fream_id)
{
    return false;
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
void QtCanPlatform::on_tableDoubleClicked(int, int)
{
    connect(tableView, SIGNAL(cellChanged(int, int)), this, SLOT(on_tableClicked(int, int)));
}
void QtCanPlatform::on_tableClicked(int row, int col)
{
    int rows = tableView->rowCount();
    if (row > rows - 1)
    {
        disconnect(tableView, SIGNAL(cellChanged(int, int)), this, SLOT(on_tableClicked(int, int)));
        return;
    }
    QString sendId = tableView->item(row, 2)->text();
    QString senddt = tableView->item(row, 1)->text();
    for (int i = 0; i < sendCanData.size(); i++)
    {
        if (sendId.trimmed().toInt(NULL,16) == sendCanData.at(i).CanId)
        {
            if (row > sendCanData.at(i).pItem.size() - 1)
            {
                disconnect(tableView, SIGNAL(cellChanged(int, int)), this, SLOT(on_tableClicked(int, int)));
                return;
            }
            sendCanData.at(i).pItem.at(row).send = senddt.toInt();
        }
    }
    disconnect(tableView, SIGNAL(cellChanged(int, int)), this, SLOT(on_tableClicked(int, int)));
       
}
void QtCanPlatform::qCanSettingShow()
{
    if (!canSetting) {
        canSetting = new QCanSetting();
    }
    canSetting->show();
}