#include "QtCanPlatform.h"
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHBoxLayout>
#include "qGboleData.h"
#include <QLabel>
#include <QMessageBox>
#include <QCheckBox>
QtCanPlatform::QtCanPlatform(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    initUi();
}

QtCanPlatform::~QtCanPlatform()
{
    if (canSetting) { delete canSetting; canSetting = nullptr; }
    if (tableView) { delete tableView; tableView = nullptr; }
    if (textBrowser) { delete textBrowser; textBrowser = nullptr; }
}

void QtCanPlatform::initUi()
{
    //这个是显示内容的
    tableView = new QTableWidget();
    tableView->setColumnCount(6);
    QStringList header;
    header << tr("操作")<<tr("地址") << tr("名称") << tr("起止字节") << tr("起止位") << tr("长度");
    tableView->setHorizontalHeaderLabels(header);
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
    QPushButton* pbSend = new QPushButton(tr("发送"));
    pbSend->setCheckable(true);
    connect(pbSend, SIGNAL(clicked(bool)),this,SLOT(on_pbSend_clicked(bool)));
    //添加个水平的布局
    QHBoxLayout* hLayout = new QHBoxLayout();
    //把按钮丢进去
    hLayout->addWidget(pbAddModel);
    hLayout->addWidget(pbSend);
   
    //把弹簧也丢进去
    hLayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));
    QLabel* mLabel = new QLabel();
    mLabel->setText(tr("当前型号"));
    hLayout->addWidget(mLabel);
    hLayout->addWidget(cbSelectModel);
    //定义一个垂直布局
    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->addLayout(hLayout);
    vLayout->addWidget(tableView);
    vLayout->addWidget(textBrowser);
    ui.centralWidget->setLayout(vLayout);

}

void QtCanPlatform::initData()
{
    qGboleData* qGb = qGboleData::getInstance();
    if (!qGb)return;
    if(!qGb->getIsInit())
        qGb->read();
}
bool QtCanPlatform::sendDataIntoTab()
{
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
    std::vector<canIdData>cTemp;
    for (int i = 0; i < pTemp.cItem.size(); i++)
    {
        //取出操作为发送的信号
        if (1 == pTemp.cItem.at(i).opt)
        {
            cTemp.push_back(pTemp.cItem.at(i));
           // break;
        }
    }
    if (cTemp.size() <= 0)
    {
        QMessageBox::warning(this, tr("warning"), tr("该型号没有发送信号，请添加再操作"));
        return false;
    }
   
    for (int i = 0; i < cTemp.size(); i++)
    {
        int num = cTemp.at(i).pItem.size();
       
        for (int j = 0; j < num; j++)
        {
            QCheckBox* cb = new QCheckBox();
            int cr = tableView->rowCount();
            tableView->setRowCount(cr+1);
            tableView->setCellWidget(cr, 0, cb);
            QString mt = "0x"+QString("%1").arg(cTemp.at(i).CanId, QString::number(cTemp.at(i).CanId).length(), 16).toUpper().trimmed();
            tableView->setItem(cr, 1, new QTableWidgetItem(mt));
            tableView->setItem(cr, 2, new QTableWidgetItem(cTemp.at(i).pItem.at(j).bitName));
            tableView->setItem(cr, 3, new QTableWidgetItem(QString::number(cTemp.at(i).pItem.at(j).startByte)));
            tableView->setItem(cr, 4, new QTableWidgetItem(QString::number(cTemp.at(i).pItem.at(j).startBit)));
            tableView->setItem(cr, 5, new QTableWidgetItem(QString::number(cTemp.at(i).pItem.at(j).bitLeng)));
        }
   }
    return true;
}
void QtCanPlatform::on_CurrentModelChanged(int index)
{
    currentModel = index;
    sendDataIntoTab();
}
void QtCanPlatform::on_pbSend_clicked(bool clicked)
{
   
}
void QtCanPlatform::qCanSettingShow()
{
    if (!canSetting) {
        canSetting = new QCanSetting();
    }
    canSetting->show();
}