#include "QProcessBuild.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
QProcessBuild::QProcessBuild(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	InitUI();
}

QProcessBuild::~QProcessBuild()
{
	
}
/*
* @brief:	初始化检测流程的界面,主要由按钮和table构成
*			
* @param:	void
* 
* @return:	void
*/
void QProcessBuild::InitUI()
{
	addButton = new QPushButton(tr("添加流程"));
	removeButton = new QPushButton(tr("移除流程"));
	upButton = new QPushButton(tr("上移"));
	downButton = new QPushButton(tr("下移"));

	connect(addButton, &QPushButton::clicked, this, &QProcessBuild::on_addButton_clicked);
	connect(removeButton, &QPushButton::clicked, this, &QProcessBuild::on_removeButton_clicked);
	connect(upButton, &QPushButton::clicked, this, &QProcessBuild::on_upButton_clicked);
	connect(downButton, &QPushButton::clicked, this, &QProcessBuild::on_downButton_clicked);
	QHBoxLayout* vBoxLButton = new QHBoxLayout();
	vBoxLButton->addWidget(addButton);
	vBoxLButton->addWidget(removeButton);
	vBoxLButton->addWidget(upButton);
	vBoxLButton->addWidget(downButton);
	vBoxLButton->addSpacerItem(new QSpacerItem(20, 20,QSizePolicy::Expanding));

	processTable = new QTableWidget();
	QStringList title;
	title << tr("来源") << tr("选择") << tr("字段/传感") << tr("比对方式") << tr("对比值") << tr("条件来源") << tr("条件选择") << tr("字段/传感") << tr("比对方式") << tr("对比值");
	processTable->setColumnCount(10);
	processTable->setHorizontalHeaderLabels(title);
	QHBoxLayout* vBoxLTable = new QHBoxLayout();
	vBoxLTable->addWidget(processTable);
	QVBoxLayout* hBoxLWid = new QVBoxLayout();
	hBoxLWid->addLayout(vBoxLButton);
	hBoxLWid->addLayout(vBoxLTable);
	hBoxLWid->setStretch(0, 1);
	hBoxLWid->setStretch(1, 10);
	this->setLayout(hBoxLWid);
}
void QProcessBuild::on_addButton_clicked()
{

}
void QProcessBuild::on_removeButton_clicked()
{
}

void QProcessBuild::on_upButton_clicked()
{
}

void QProcessBuild::on_downButton_clicked()
{
}
