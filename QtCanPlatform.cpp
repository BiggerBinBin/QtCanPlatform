#include "QtCanPlatform.h"
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHBoxLayout>
#include "qGboleData.h"
#include <QLabel>
#include <QMessageBox>
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
    tableView = new QTableView();
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
    QPushButton* pbAddModel = new QPushButton("添加Can解析");
    connect(pbAddModel, &QPushButton::clicked, this, &QtCanPlatform::qCanSettingShow);
    //添加个水平的布局
    QHBoxLayout* hLayout = new QHBoxLayout();
    //把按钮丢进去
    hLayout->addWidget(pbAddModel);
   
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
    bool res = false;
    for (int i = 0; i < pTemp.cItem.size(); i++)
    {
        //取出操作为发送的信号
        if (1 == pTemp.cItem.at(i).opt)
        {
            cTemp.push_back(pTemp.cItem.at(i));
            break;
        }
    }
    if (cTemp.size() <= 0)
    {
        QMessageBox::warning(this, tr("warning"), tr("该型号没有发送信号，请添加再操作"));
        return false;
    }
    return true;
}
void QtCanPlatform::on_CurrentModelChanged(int index)
{
    currentModel = index;
    
}
void QtCanPlatform::qCanSettingShow()
{
    if (!canSetting) {
        canSetting = new QCanSetting();
    }
    canSetting->show();
}