#include "QSetProperty.h"
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <qcolordialog.h>
#include <qcolor.h>
#include <QMessageBox>
#include "AlgorithmSet.h"
QSetProperty::QSetProperty(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	InitUI();
	this->setWindowModality(Qt::ApplicationModal);
}

QSetProperty::~QSetProperty()
{
	
}


void QSetProperty::InitUI()
{
	QPushButton* pbAddItem = new QPushButton(tr("添加"));
	connect(pbAddItem, SIGNAL(clicked()), this, SLOT(on_pbAddItem_clicked()));
	QPushButton* pbDelItem = new QPushButton(tr("删除"));
	connect(pbDelItem, SIGNAL(clicked()), this, SLOT(on_pbDelItem_clicked()));
	QPushButton* pbSaveItem = new QPushButton(tr("保存"));
	connect(pbSaveItem, SIGNAL(clicked()), this, SLOT(on_pbSaveItem_clicked()));
	QHBoxLayout* pbLayout = new QHBoxLayout();
	pbLayout->addWidget(pbAddItem);
	pbLayout->addWidget(pbDelItem);
	pbLayout->addWidget(pbSaveItem);
	pbLayout->addSpacerItem(new QSpacerItem(60, 20, QSizePolicy::Expanding));
	table = new QTableWidget(this);
	QStringList header;
	header << tr("返回值") << tr("对应名称") << tr("显示颜色");
	table->setColumnCount(3);
	table->setHorizontalHeaderLabels(header);
	table->setSelectionBehavior(QAbstractItemView::SelectRows);
	QLabel* tips = new QLabel(tr("设置此字段返回的值对应显示的名称,以前它显示的颜色"));
	QVBoxLayout* vb = new QVBoxLayout();
	vb->addLayout(pbLayout);
	vb->addWidget(table);
	vb->addWidget(tips);
	vb->setStretch(0, 1);
	vb->setStretch(1, 5);
	vb->setStretch(2, 1);
	this->setLayout(vb);

}

void QSetProperty::on_pbAddItem_clicked()
{
	if (!table) return;
	int rows = table->rowCount();
	table->setRowCount(rows + 1);
	table->setItem(rows, 0, new QTableWidgetItem(QString::number(rows)));
	table->setItem(rows, 1, new QTableWidgetItem(tr("正常")));
	QPushButton* pbColor = new QPushButton(tr("背景色"));
	connect(pbColor, SIGNAL(clicked()), this, SLOT(getColor()));
	table->setCellWidget(rows, 2, pbColor);
	if (!mcp)
	{
		QMessageBox::warning(NULL, tr("提示"), tr("数据指针未初始化，该数据不会保存"));
		return;
	}
	cellProperty cp;
	cp.color = QColor(240, 240, 240);
	cp.toWord = "正常";
	//插入
	mcp->insert({ QString::number(rows), cp });
		
}

void QSetProperty::on_pbDelItem_clicked()
{
}

void QSetProperty::on_pbSaveItem_clicked()
{
}
void QSetProperty::on_itemDoubleClicked()
{
	//要编辑必须双击，所以在双击的时候把cell变化的信号建立起来，就是编辑之后的信号了
	connect(table, SIGNAL(cellChanged(int, int)), this, SLOT(on_table_cellChanged(int, int)));
}
void QSetProperty::on_table_cellChanged(int row,int col)
{

	//关掉它，不然在添加行的时候会触发cellChanged这个信号
	disconnect(table, SIGNAL(cellChanged(int, int)), this, SLOT(on_table_cellChanged(int, int)));
}
void QSetProperty::getColor()
{
	QPushButton* send = dynamic_cast<QPushButton*>(sender());
	if (!send)
		return;
	QColor color = QColorDialog::getColor(Qt::white, this, tr("选择颜色"));
	//color.rgba();
	QString rgba = "background-color:#"+QString::number(color.rgba(), 16);
	send->setStyleSheet(rgba);
	QModelIndex index = table->indexAt(QPoint(send->geometry().x(), send->geometry().y()));
	int row = index.row();
	if (!YB::keyInMap(*mcp, QString::number(row)))
	{
		return;
	}
	(*mcp)[QString::number(row)].color = color;
}