#include "QtCanPlatform.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHBoxLayout>
QtCanPlatform::QtCanPlatform(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    initUi();
    initData();
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
    //添加个按钮
    QPushButton* pbAddModel = new QPushButton("添加Can解析");
    connect(pbAddModel, &QPushButton::clicked, this, &QtCanPlatform::qCanSettingShow);
    //添加个水平的布局
    QHBoxLayout* hLayout = new QHBoxLayout();
    //把按钮丢进去
    hLayout->addWidget(pbAddModel);
    //把弹簧也丢进去
    hLayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));
    //定义一个垂直布局
    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->addLayout(hLayout);
    vLayout->addWidget(tableView);
    vLayout->addWidget(textBrowser);
    ui.centralWidget->setLayout(vLayout);

}

void QtCanPlatform::initData()
{
    
}
void QtCanPlatform::qCanSettingShow()
{
    if (!canSetting) {
        canSetting = new QCanSetting();
    }
    canSetting->show();
}