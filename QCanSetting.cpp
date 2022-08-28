#include "QCanSetting.h"
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QComboBox>
#include <qmessagebox.h>
#include <QDir>
#include <QJsonDocument>	// 解析Json
#include <QJsonArray>		// [ ]
#include <QJsonObject>		// { }
#include <QJsonValue>		// int float double null { } [ ]
#include <QJsonParseError>
#include <QTextStream>
#include "qGboleData.h"

QCanSetting::QCanSetting(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//让widget模态运行
	this->setWindowModality(Qt::ApplicationModal);
	InitUi();
	InitpGboleData();
}

QCanSetting::~QCanSetting()
{
	if (modelView)
	{
		delete modelView; modelView = nullptr;
	}
	if (canIdView)
	{
		delete canIdView; canIdView = nullptr;
	}
	if (tableView)
	{
		delete tableView; tableView = nullptr;
	}
}

void QCanSetting::InitUi()
{
	QStringList listname;
	listname << "名称" << "协议";
	QPushButton* pbAddModel = new QPushButton("添加型号");
	pbAddModel->setMinimumWidth(60);
	connect(pbAddModel, &QPushButton::clicked, this, &QCanSetting::on_pbAddModel_clicked);
	QPushButton* pbMoveUpModel = new QPushButton("上移");
	pbMoveUpModel->setMinimumWidth(50);
	connect(pbMoveUpModel, &QPushButton::clicked, this, &QCanSetting::on_pbMoveUpModel_clicked);
	QPushButton* pbMoveDownModel = new QPushButton("下移");
	pbMoveDownModel->setMinimumWidth(50);
	connect(pbMoveDownModel, &QPushButton::clicked, this, &QCanSetting::on_pbMoveDownModel_clicked);
	QPushButton* pbDelModel = new QPushButton("删除");
	pbDelModel->setMinimumWidth(50);
	connect(pbDelModel, &QPushButton::clicked, this, &QCanSetting::on_pbDelModel_clicked);
	QPushButton* pbSaveModel = new QPushButton("保存");
	pbSaveModel->setMinimumWidth(50);
	connect(pbSaveModel, &QPushButton::clicked, this, &QCanSetting::on_pbSaveModel_clicked);
	pbSaveModel->setMinimumWidth(50);
	QHBoxLayout* hLayoutMdeol = new QHBoxLayout();
	hLayoutMdeol->addWidget(pbAddModel);
	hLayoutMdeol->addWidget(pbMoveUpModel);
	hLayoutMdeol->addWidget(pbMoveDownModel);
	hLayoutMdeol->addWidget(pbDelModel);
	hLayoutMdeol->addWidget(pbSaveModel);
	hLayoutMdeol->addSpacerItem(new QSpacerItem(60, 20, QSizePolicy::Expanding));
	hLayoutMdeol->setSpacing(0);
	modelView = new QTableWidget();
	modelView->setColumnCount(2);
	modelView->setHorizontalHeaderLabels(listname);
	modelView->setSelectionBehavior(QAbstractItemView::SelectRows);
	modelView->setMaximumWidth(250);
	connect(modelView, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(on_modelView_doubleClicked(int, int)));
	connect(modelView, SIGNAL(cellClicked(int, int)), this, SLOT(on_modelView_Clicked(int, int)));
	
	QVBoxLayout* vLayoutModel = new QVBoxLayout();
	vLayoutModel->addLayout(hLayoutMdeol);
	vLayoutModel->addWidget(modelView);
	vLayoutModel->addSpacerItem(new QSpacerItem(20, 80, QSizePolicy::Expanding));

	listname.clear();
	listname << "CanId" << "接收/发送";
	QPushButton* pbAddCanId = new QPushButton("添加ID");
	connect(pbAddCanId, &QPushButton::clicked, this, &QCanSetting::on_pbAddCanId_clicked);
	QPushButton* pbMoveUpCanId = new QPushButton("上移");
	connect(pbMoveUpCanId, &QPushButton::clicked, this, &QCanSetting::on_pbMoveUpCanId_clicked);
	QPushButton* pbMoveDownCanId = new QPushButton("下移");
	connect(pbMoveDownCanId, &QPushButton::clicked, this, &QCanSetting::on_pbMoveDownCanId_clicked);
	QPushButton* pbDelCanId = new QPushButton("删除");
	connect(pbDelCanId, &QPushButton::clicked, this, &QCanSetting::on_pbDelCanId_clicked);
	QPushButton* pbSaveCanId = new QPushButton("保存");
	connect(pbSaveCanId, &QPushButton::clicked, this, &QCanSetting::on_pbSaveCanId_clicked);
	pbAddCanId->setMinimumWidth(60);
	pbMoveUpCanId->setMinimumWidth(50);
	pbMoveDownCanId->setMinimumWidth(50);
	pbDelCanId->setMinimumWidth(50);
	pbSaveCanId->setMinimumWidth(50);
	QHBoxLayout* hLayoutCanId = new QHBoxLayout();
	hLayoutCanId->addWidget(pbAddCanId);
	hLayoutCanId->addWidget(pbMoveUpCanId);
	hLayoutCanId->addWidget(pbMoveDownCanId);
	hLayoutCanId->addWidget(pbDelCanId);
	hLayoutCanId->addWidget(pbSaveCanId);
	hLayoutCanId->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));
	hLayoutCanId->setSpacing(0);
	canIdView = new QTableWidget();
	canIdView->setColumnCount(2);
	canIdView->setHorizontalHeaderLabels(listname);
	canIdView->setSelectionBehavior(QAbstractItemView::SelectRows);
	canIdView->setMaximumWidth(250);
	connect(canIdView, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(on_canIdView_doubleClicked(int, int)));
	connect(canIdView, SIGNAL(cellClicked(int, int)), this, SLOT(on_canIdView_Clicked(int, int)));
	QVBoxLayout* vLayoutCanId = new QVBoxLayout();
	vLayoutCanId->addLayout(hLayoutCanId);
	vLayoutCanId->addWidget(canIdView);
	
	//vLayoutCanId->addSpacerItem(new QSpacerItem(20, 80, QSizePolicy::Expanding));

	QPushButton* pbAddIteam = new QPushButton("添加项");
	connect(pbAddIteam, &QPushButton::clicked, this, &QCanSetting::on_pbAddIteam_clicked);
	QPushButton* pbMoveUpIteam = new QPushButton("上移");
	connect(pbMoveUpIteam, &QPushButton::clicked, this, &QCanSetting::on_pbMoveUpIteam_clicked);
	QPushButton* pbMoveDownIteam = new QPushButton("下移");
	connect(pbMoveDownIteam, &QPushButton::clicked, this, &QCanSetting::on_pbMoveDownIteam_clicked);
	QPushButton* pbDelIteam = new QPushButton("删除");
	connect(pbDelIteam, &QPushButton::clicked, this, &QCanSetting::on_pbDelIteam_clicked);
	QPushButton* pbSaveIteam = new QPushButton("保存");
	connect(pbSaveIteam, &QPushButton::clicked, this, &QCanSetting::on_pbSaveIteam_clicked);
	QHBoxLayout* hLayout = new QHBoxLayout();
	hLayout->addWidget(pbAddIteam);
	hLayout->addWidget(pbMoveUpIteam);
	hLayout->addWidget(pbMoveDownIteam);
	hLayout->addWidget(pbDelIteam);
	hLayout->addWidget(pbSaveIteam);
	hLayout->setSpacing(0);
	//hLayout->addSpacerItem(new QSpacerItem(20, 80, QSizePolicy::Expanding));
	listname.clear();
	listname << tr("字段名称") << tr("起止字节") << tr("起止位") << tr("长度") << tr("精度") << tr("偏移量")<<tr("属性");
	
	tableView = new QTableWidget();
	tableView->setColumnCount(7);
	tableView->setHorizontalHeaderLabels(listname);
	tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	connect(tableView, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(on_tableView_doubleCLicked(int, int)));
	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->addLayout(hLayout);
	vLayout->addWidget(tableView);
	
	//新建一个水平layout
	QHBoxLayout* hLayoutAll = new QHBoxLayout();
	hLayoutAll->addLayout(vLayoutModel);
	hLayoutAll->addLayout(vLayoutCanId);
	hLayoutAll->addLayout(vLayout);
	//设置比例，1：1：3
	hLayoutAll->setStretch(0, 1);
	hLayoutAll->setStretch(1, 1);
	hLayoutAll->setStretch(2, 3);
	this->setLayout(hLayoutAll);
	
}
void QCanSetting::on_pbAddModel_clicked()
{
	
	if (!modelView)
		return;
	disconnect(modelView, SIGNAL(itemChanged(cellChanged(int, int))), this, SLOT(on_modelView_cellChanged(int, int)));
	short int row = modelView->rowCount();
	modelView->setRowCount(row + 1);
	QComboBox* proto = new QComboBox();
	proto->addItem(tr("Intel"));
	proto->addItem(tr("Motorola"));
	connect(proto, SIGNAL(currentIndexChanged(int)), this, SLOT(on_modelView_currentIndexChanged(int)));
	modelView->setItem(row, 0, new QTableWidgetItem(tr("型号")+QString::number(row)));
	modelView->setCellWidget(row, 1, proto);
	struct protoData data;
	data.modelName = tr("型号") + QString::number(row);
	data.agreement = 0;
	qGboleData* qGb = qGboleData::getInstance();
	if (!qGb)return;
	qGb->pGboleData.push_back(data);
	
}

void QCanSetting::savepGboleData()
{
	qGboleData* qGb = qGboleData::getInstance();
	if (!qGb)return;
	qGb->save();
}

void QCanSetting::InitpGboleData()
{
	qGboleData* qGb = qGboleData::getInstance();
	if (!qGb)return;
	if (!qGb->getIsInit())
		qGb->read();
	//初始化表格
	SetTableData();
}

void QCanSetting::SetTableData()
{
	qGboleData* qGb = qGboleData::getInstance();
	if (!qGb)return;
	if (qGb->pGboleData.size() < 0)
		return;
	for (int i = 0; i < qGb->pGboleData.size(); ++i)
	{
		
		modelView->setRowCount(i + 1);
		QComboBox* proto = new QComboBox();
		proto->addItem(tr("Intel"));
		proto->addItem(tr("Motorola"));
		connect(proto, SIGNAL(currentIndexChanged(int)), this, SLOT(on_modelView_currentIndexChanged(int)));
		modelView->setItem(i, 0, new QTableWidgetItem(qGb->pGboleData.at(i).modelName));
		modelView->setCellWidget(i, 1, proto);
		proto->setCurrentIndex(qGb->pGboleData.at(i).agreement);
	}
}

void QCanSetting::on_pbMoveUpModel_clicked()
{
	if (!modelView)
		return;
	short int index = modelView->currentIndex().row();
	if (index == 0)
		return;
	
}

void QCanSetting::on_pbMoveDownModel_clicked()
{
}

void QCanSetting::on_pbDelModel_clicked()
{
	if (!modelView)
		return;
	short int index = modelView->currentIndex().row();
	if (index < 0)return;
	modelView->removeRow(index);
	qGboleData* qGb = qGboleData::getInstance();
	if (!qGb)return;
	if (qGb->pGboleData.size() - 1 < index)
		return;
	std::vector<protoData>::iterator iterb = qGb->pGboleData.begin();
	std::vector<protoData>::iterator itere = qGb->pGboleData.end();
	int n = 0;
	while (iterb != itere)
	{
		if (n == index)
		{
			qGb->pGboleData.erase(iterb);
			break;
		}
		else
		{
			iterb++;
			n++;
		}
			
	}
}

void QCanSetting::on_pbSaveModel_clicked()
{
	savepGboleData();
}

void QCanSetting::on_pbAddCanId_clicked()
{
	qGboleData* qGb = qGboleData::getInstance();
	if (!qGb)return;
	if (!canIdView)
		return;
	if (!modelView)
		return;
	if (modelView->rowCount() < 1)
		return;
	short int curSelectRow = modelView->currentRow();
	if (curSelectRow<0 || curSelectRow>qGb->pGboleData.size() - 1)
	{
		QMessageBox::warning(this, tr("warning"), tr("未选中型号，不能添加CanID"));
		return;
	}
		
	short int row = canIdView->rowCount();
	canIdView->setRowCount(row + 1);
	QComboBox* proto = new QComboBox();
	proto->addItem(tr("接收"));
	proto->addItem(tr("发送"));
	connect(proto, SIGNAL(currentIndexChanged(int)), this, SLOT(on_canIdView_currentIndexChanged(int)));

	canIdView->setItem(row, 0, new QTableWidgetItem(tr("123456")));
	canIdView->setCellWidget(row, 1, proto);
	canIdData cdata;
	cdata.CanId = canIdView->item(row,0)->text().toInt(NULL,16);
	cdata.opt = 0;
	qGb->pGboleData.at(curSelectRow).cItem.push_back(cdata);
}

void QCanSetting::on_pbMoveUpCanId_clicked()
{
}

void QCanSetting::on_pbMoveDownCanId_clicked()
{
}

void QCanSetting::on_pbDelCanId_clicked()
{
	if (!canIdView)
		return;
	
	short int index = canIdView->currentIndex().row();
	if (index < 0)return;
	canIdView->removeRow(index);
	qGboleData* qGb = qGboleData::getInstance();
	if (!qGb)return;
	short int mr = modelView->currentRow();
	if (mr<0 || mr>qGb->pGboleData.size() - 1)
		return;
	if (index > qGb->pGboleData.at(mr).cItem.size() - 1)
		return;
	std::vector<canIdData>::iterator itbe = qGb->pGboleData.at(mr).cItem.begin();
	std::vector<canIdData>::iterator itend = qGb->pGboleData.at(mr).cItem.end();
	int n = 0;
	while (itbe != itend)
	{
		if (n == index)
		{
			qGb->pGboleData.at(mr).cItem.erase(itbe);
			break;
		}
		else
		{
			n++;
			itbe++;
		}
	}
}

void QCanSetting::on_pbSaveCanId_clicked()
{
	savepGboleData();
}

void QCanSetting::on_pbAddIteam_clicked()
{
	qGboleData* qGb = qGboleData::getInstance();
	if (!qGb)return;
	if (!modelView)
		return;
	if (!tableView)
		return;
	if (!canIdView)
		return;
	if (modelView->rowCount() < 1)
		return;
	if (canIdView->rowCount() < 1)
		return;
	short int curSelectRow = modelView->currentRow();
	if (curSelectRow<0 || curSelectRow>qGb->pGboleData.size() - 1)
	{
		QMessageBox::warning(this, tr("warning"), tr("未选中型号，不能添加，要先选中型号，然后再选中CanID，再添加"));
		return;
	}
	short int curSelectCanRow = canIdView->currentRow();
	if (curSelectCanRow<0 || curSelectCanRow>qGb->pGboleData.at(curSelectRow).cItem.size() - 1)
	{
		QMessageBox::warning(this, tr("warning"), tr("未选中CanId，不能添加，要先选中型号，然后再选中CanID，再添加"));
		return;
	}
	short int row = tableView->rowCount();
	tableView->setRowCount(row + 1);
	tableView->setItem(row, 0, new QTableWidgetItem(tr("字段")+QString::number(row)));
	tableView->setItem(row, 1, new QTableWidgetItem(QString::number(0)));
	tableView->setItem(row, 2, new QTableWidgetItem(QString::number(1)));
	tableView->setItem(row, 3, new QTableWidgetItem(QString::number(1)));
	tableView->setItem(row, 4, new QTableWidgetItem(QString::number(1)));
	tableView->setItem(row, 5, new QTableWidgetItem(QString::number(0)));
	QPushButton* property = new QPushButton(tr("属性"));
	connect(property, SIGNAL(clicked()), this, SLOT(on_property_clicked()));
	tableView->setCellWidget(row, 6, property);
	protoItem pItem;
	pItem.bitName	= tableView->item(row, 0)->text();
	pItem.startByte = tableView->item(row, 1)->text().toInt(NULL, 10);
	pItem.startBit	= tableView->item(row, 2)->text().toInt(NULL, 10);
	pItem.bitLeng	= tableView->item(row, 3)->text().toInt(NULL, 10);
	pItem.precision = tableView->item(row, 4)->text().toInt(NULL, 10);
	pItem.offset	= tableView->item(row, 5)->text().toInt(NULL, 10);
	pItem.send = 0;
	try
	{
		qGb->pGboleData.at(curSelectRow).cItem.at(curSelectCanRow).pItem.push_back(pItem);
	}
	catch (const std::exception&e)
	{
		QMessageBox::warning(this, tr("warning"), e.what());
	}
	catch(...) {
		QMessageBox::warning(this, tr("warning"),tr("pGboleData.at(curSelectRow).cItem.at(curSelectCanRow).pItem.push_back(pItem)->error"));
	}
	
}

void QCanSetting::on_pbMoveUpIteam_clicked()
{
}

void QCanSetting::on_pbMoveDownIteam_clicked()
{
}
//第三层表格，删除
void QCanSetting::on_pbDelIteam_clicked()
{
	if (!tableView)
		return;
	short int index = tableView->currentIndex().row();
	if (index < 0)return;
	tableView->removeRow(index);

	qGboleData* qGb = qGboleData::getInstance();
	if (!qGb)return;
	if (!modelView)
		return;
	
	if (!canIdView)
		return;
	//若第一层表格没有东西，直接return
	if (modelView->rowCount() < 1)
		return;
	//叵第二层表格没有东西，同样return
	if (canIdView->rowCount() < 1)
		return;
	//超出范围
	short int curSelectRow = modelView->currentRow();
	if (curSelectRow<0 || curSelectRow>qGb->pGboleData.size() - 1)
	{
		QMessageBox::warning(this, tr("warning"), tr("未选中型号，不能删除，要先选中型号，然后再选中CanID，再删除"));
		return;
	}
	//超出范围
	short int curSelectCanRow = canIdView->currentRow();
	if (curSelectCanRow<0 || curSelectCanRow>qGb->pGboleData.at(curSelectRow).cItem.size() - 1)
	{
		QMessageBox::warning(this, tr("warning"), tr("未选中CanId，不能删除，要先选中型号，然后再选中CanID，再删除"));
		return;
	}
	//超出范围
	if (index<0 || index>qGb->pGboleData.at(curSelectRow).cItem.at(curSelectCanRow).pItem.size())
	{
		QMessageBox::warning(this, tr("warning"), tr("数据删除出错，不在Vector里面"));
		return;
	}
	//使用迭代器
	std::vector<protoItem>::iterator itbegin= qGb->pGboleData.at(curSelectRow).cItem.at(curSelectCanRow).pItem.begin();
	std::vector<protoItem>::iterator itend= qGb->pGboleData.at(curSelectRow).cItem.at(curSelectCanRow).pItem.end();
	int n = 0;
	while (itbegin != itend)
	{
		//找到所在位置,因为vector是有序的，添加行的同时也push到vector了，行的位置就是vector的位置
		if (n == index)
		{
			//使用erase移除
			qGb->pGboleData.at(curSelectRow).cItem.at(curSelectCanRow).pItem.erase(itbegin);
			break;
		}
		else
		{
			itbegin++;
			n++;
		}
	}
}

void QCanSetting::on_pbSaveIteam_clicked()
{
	savepGboleData();
}

void QCanSetting::on_modelView_currentIndexChanged(int index)
{
	qGboleData* qGb = qGboleData::getInstance();
	if (!qGb)return;
	//取出发出者的指针
	QComboBox* cbx = qobject_cast<QComboBox*>(sender());
	if (cbx == nullptr)
		return;
	//取按钮所在行列
	QModelIndex idx = modelView->indexAt(QPoint(cbx->frameGeometry().x(), cbx->frameGeometry().y()));
	int row = idx.row();
	if (row<0 || row>qGb->pGboleData.size() - 1)
	{
		QMessageBox::warning(this, tr("警告"), tr("数据超出范围"));
		return;
	}
	qGb->pGboleData.at(row).agreement = cbx->currentIndex();
}

void QCanSetting::on_modelView_cellChanged(int row, int col)
{
	qGboleData* qGb = qGboleData::getInstance();
	if (!qGb)return;
	if (row<0 || row>qGb->pGboleData.size() - 1||col!=0)
	{
		return;
	}
	qGb->pGboleData.at(row).modelName = modelView->item(row,col)->text();
	//关掉这个，防止添加行setData的时候，触发这个信号
	disconnect(modelView, SIGNAL(itemChanged(cellChanged(int, int))), this, SLOT(on_modelView_cellChanged(int, int)));
}

void QCanSetting::on_modelView_doubleClicked(int, int)
{
	//注意QT的这个cellChanged信号，他会当setData的时候触发这个信号
	//所以曲线救国，在双击的时候才连接这个槽，
	connect(modelView, SIGNAL(cellChanged(int, int)), this, SLOT(on_modelView_cellChanged(int, int)));
}
/***********************************
*@brief:列表在初始化时，只会初始化型号列表的
*而型号下面的CanId vector数据，只有鼠标在点击某个型号时
* 才会初始化后面的
* @return: void
************************************/
void QCanSetting::on_modelView_Clicked(int row, int col)
{
	qGboleData* qGb = qGboleData::getInstance();
	if (!qGb)return;
	if (!canIdView)
		return;
	if (!modelView)
		return;
	if (row < 0)
		return;
	int rowCount = canIdView->rowCount();
	for (int m = 0; m < rowCount; m++)
		canIdView->removeRow(rowCount - m - 1);

	if (row > qGb->pGboleData.size() - 1)
		return;
	try
	{
		for (int i = 0; i < qGb->pGboleData.at(row).cItem.size(); ++i)
		{
			canIdView->setRowCount(i + 1);
			QComboBox* proto = new QComboBox();
			proto->addItem(tr("接收"));
			proto->addItem(tr("发送"));
			proto->setCurrentIndex(qGb->pGboleData.at(row).cItem.at(i).opt);
			QString mt = "0x" + QString("%1").arg(qGb->pGboleData.at(row).cItem.at(i).CanId, QString::number(qGb->pGboleData.at(row).cItem.at(i).CanId, 16).length(), 16).toUpper().trimmed();
			canIdView->setItem(i, 0, new QTableWidgetItem(mt));
			canIdView->setCellWidget(i, 1, proto);
			connect(proto, SIGNAL(currentIndexChanged(int)), this, SLOT(on_canIdView_currentIndexChanged(int)));
		}
	}
	catch (const std::exception&e)
	{
		QMessageBox::warning(this, tr("warning"), QString(tr("Vector超出:") + e.what()));
	}
	canIdView->setCurrentCell(0, 0);
	
	if (!tableView)
		return;
	on_canIdView_Clicked(0, 0);
}

void QCanSetting::on_canIdView_currentIndexChanged(int index)
{
	qGboleData* qGb = qGboleData::getInstance();
	if (!qGb)return;
	QComboBox* cb = dynamic_cast<QComboBox*>(sender());
	if (!cb)
		return;
	if (!modelView) return;
	int mCurRow = modelView->currentRow();
	if (mCurRow < 0 || mCurRow>qGb->pGboleData.size() - 1) {
		QMessageBox::warning(this, tr("warning"), tr("未选中型号， 不能修改CanID"));
		return;
	}
	if (!canIdView) return;
	QModelIndex pIndex = canIdView->indexAt(QPoint(cb->frameGeometry().x(), cb->frameGeometry().y()));
	int row = pIndex.row();

	if (row > qGb->pGboleData.at(mCurRow).cItem.size() - 1 || row < 0 )
	{
		QMessageBox::warning(this, tr("warning"), tr("修改的位置超出范围"));
		return;
	}
	try
	{
		qGb->pGboleData.at(mCurRow).cItem.at(row).opt = cb->currentIndex();
	}
	catch (const std::exception&e)
	{
		QMessageBox::warning(this, tr("warning"), QString(tr("Vector超出:") + e.what() + "Infunction:on_canIdView_currentIndexChanged"));
	}

}

void QCanSetting::on_canIdView_cellChanged(int row, int col)
{
	qGboleData* qGb = qGboleData::getInstance();
	if (!qGb)return;
	if (!modelView) return;
	int mCurRow = modelView->currentRow();
	if (mCurRow < 0 ||mCurRow>qGb->pGboleData.size()-1) {
		QMessageBox::warning(this, tr("warning"), tr("未选中型号， 不能修改CanID"));
		return;
	}
	if (!canIdView) return;
	try
	{
		if (row > qGb->pGboleData.at(mCurRow).cItem.size() - 1 || row <0||col!=0)
		{
			QMessageBox::warning(this, tr("warning"), tr("修改的位置超出范围"));
			return;
		}
		qGb->pGboleData.at(mCurRow).cItem.at(row).CanId = canIdView->item(row,col)->text().trimmed().toInt(nullptr, 16);
	}
	catch (const std::exception&e)
	{
		QMessageBox::warning(this, tr("warning"), QString(tr("Vector超出:") + e.what() + "Infunction:on_canIdView_cellChanged"));
	}
	//关掉这个，防止添加行setData的时候，触发这个信号
	disconnect(canIdView, SIGNAL(itemChanged(cellChanged(int, int))), this, SLOT(on_canIdView_cellChanged(int, int)));
}

void QCanSetting::on_canIdView_doubleClicked(int, int)
{
	//注意QT的这个cellChanged信号，他会当setData的时候触发这个信号
	//所以曲线救国，在双击的时候才连接这个槽，
	connect(canIdView, SIGNAL(cellChanged(int, int)), this, SLOT(on_canIdView_cellChanged(int, int)));
}
/*
* @brief: 在tableView这个表格上显示canId对应的项
* @return: void
*/
void QCanSetting::on_canIdView_Clicked(int row, int col)
{
	qGboleData* qGb = qGboleData::getInstance();
	if (!qGb)return;
	if (!tableView)
		return;
	if (!canIdView)
		return;
	if (!modelView)
		return;
	if (row < 0)
		return;
	//获取当前选中的型号，若没选中，则不管
	short mRow = modelView->currentRow();
	if (mRow < 0 || mRow > qGb->pGboleData.size() - 1)
		return;
	//清空之前表格的内容
	int rowCount = tableView->rowCount();
	for (int m = 0; m < rowCount; m++)
		tableView->removeRow(rowCount - m - 1);
	int count = qGb->pGboleData.at(mRow).cItem.size()-1;
	if (row > count)
		return;
	try
	{
		//显示对应CanID的项
		for (int i = 0; i < qGb->pGboleData.at(mRow).cItem.at(row).pItem.size(); i++)
		{
			//short int crow = tableView->rowCount();
			tableView->setRowCount(i + 1);
			tableView->setItem(i, 0, new QTableWidgetItem(qGb->pGboleData.at(mRow).cItem.at(row).pItem.at(i).bitName));
			tableView->setItem(i, 1, new QTableWidgetItem(QString::number(qGb->pGboleData.at(mRow).cItem.at(row).pItem.at(i).startByte)));
			tableView->setItem(i, 2, new QTableWidgetItem(QString::number(qGb->pGboleData.at(mRow).cItem.at(row).pItem.at(i).startBit)));
			tableView->setItem(i, 3, new QTableWidgetItem(QString::number(qGb->pGboleData.at(mRow).cItem.at(row).pItem.at(i).bitLeng)));
			tableView->setItem(i, 4, new QTableWidgetItem(QString::number(qGb->pGboleData.at(mRow).cItem.at(row).pItem.at(i).precision)));
			tableView->setItem(i, 5, new QTableWidgetItem(QString::number(qGb->pGboleData.at(mRow).cItem.at(row).pItem.at(i).offset)));
			QPushButton* property = new QPushButton(tr("属性"));
			connect(property, SIGNAL(clicked()), this, SLOT(on_property_clicked()));
			tableView->setCellWidget(i, 6, property);
		}
	}
	catch (const std::exception&e)
	{
		QMessageBox::warning(this, tr("warning"), QString(tr("Vector超出:")+e.what()+ "Infunction:on_canIdView_Clicked"));
		return;
	}
}

void QCanSetting::on_tableView_doubleCLicked(int row , int col)
{
	//建立编辑后的信号槽
	connect(tableView, SIGNAL(cellChanged(int, int)), this, SLOT(on_tableView_cellChanged(int, int)));
}

void QCanSetting::on_tableView_cellChanged(int row, int col)
{
	qGboleData* qGb = qGboleData::getInstance();
	if (!qGb)return;
	if (!tableView)
		return;
	if (!canIdView)
		return;
	if (!modelView)
		return;
	if (row < 0 || col < 0)
		return;
	try
	{
		int mCurRow = modelView->currentRow();
		if (mCurRow<0 || mCurRow>qGb->pGboleData.size() - 1)
		{
			QMessageBox::warning(this, tr("warning"), QString(tr("未选中型号，不能修改")));
			return;
		}
		int cCurRow = canIdView->currentRow();
		if (cCurRow<0 || cCurRow>qGb->pGboleData.at(mCurRow).cItem.size() - 1)
		{
			QMessageBox::warning(this, tr("warning"), QString(tr("未选中型号，不能修改")));
			return;
		}
		if (row > qGb->pGboleData.at(mCurRow).cItem.at(cCurRow).pItem.size() - 1)
		{
			QMessageBox::warning(this, tr("warning"), QString(tr("字段vector超出范围：\nrow > pGboleData.at(mCurRow).cItem.at(cCurRow).pItem.size() - 1")));
			return;
		}
		//获取修改的的值
		switch (col)
		{
		case 0:
			qGb->pGboleData.at(mCurRow).cItem.at(cCurRow).pItem.at(row).bitName = tableView->item(row, col)->text();
			break;
		case 1:
			qGb->pGboleData.at(mCurRow).cItem.at(cCurRow).pItem.at(row).startByte = tableView->item(row, col)->text().toInt();
			break;
		case 2:
			qGb->pGboleData.at(mCurRow).cItem.at(cCurRow).pItem.at(row).startBit = tableView->item(row, col)->text().toInt();
			break;
		case 3:
			qGb->pGboleData.at(mCurRow).cItem.at(cCurRow).pItem.at(row).bitLeng = tableView->item(row, col)->text().toInt();
			break;
		case 4:
			qGb->pGboleData.at(mCurRow).cItem.at(cCurRow).pItem.at(row).precision = tableView->item(row, col)->text().toInt();
			break;
		case 5:
			qGb->pGboleData.at(mCurRow).cItem.at(cCurRow).pItem.at(row).offset = tableView->item(row, col)->text().toInt();
			break;
		default:
			break;
		}
	}
	catch (const std::exception&e)
	{
		QMessageBox::warning(this, tr("warning"), QString(tr("Vector超出:") + e.what()+"Infunction:on_tableView_cellChanged"));
		//return;
	}
	//后面要把这个关掉，不然用户在添加的项时候，会触发这个槽函数
	disconnect(tableView, SIGNAL(cellChanged(int, int)), this, SLOT(on_tableView_cellChanged(int, int)));
}

void QCanSetting::on_property_clicked()
{
	QPushButton* pb = dynamic_cast<QPushButton*>(sender());
	if (!pb)
		return;
	if (!tableView)
		return;
	if (!canIdView)
		return;
	if (!modelView)
		return;
	QModelIndex inedx = tableView->indexAt(QPoint(pb->geometry().x(), pb->geometry().y()));
	int row = inedx.row();
	qGboleData* qGb = qGboleData::getInstance();
	if (!qGb)return;
	
	try
	{
		//判断第一层表格是否选中
		int mCurRow = modelView->currentRow();
		if (mCurRow<0 || mCurRow>qGb->pGboleData.size() - 1)
		{
			QMessageBox::warning(this, tr("warning"), QString(tr("未选中型号，不能修改")));
			return;
		}
		//判断第二层表格是否选中
		int cCurRow = canIdView->currentRow();
		if (cCurRow<0 || cCurRow>qGb->pGboleData.at(mCurRow).cItem.size() - 1)
		{
			QMessageBox::warning(this, tr("warning"), QString(tr("未选中型号，不能修改")));
			return;
		}
		//判断Button是否在第三层表格的数据范围内
		if (row > qGb->pGboleData.at(mCurRow).cItem.at(cCurRow).pItem.size() - 1 || row <0)
		{
			QMessageBox::warning(this, tr("warning"), QString(tr("字段vector超出范围：\nrow > pGboleData.at(mCurRow).cItem.at(cCurRow).pItem.size() - 1")));
			return;
		}
		if (!pp)
		{
			pp = new QSetProperty();
		}
		//取出该项的map
		std::map<QString, cellProperty>&mapMM = qGb->pGboleData.at(mCurRow).cItem.at(cCurRow).pItem.at(row).itemProperty;
		//丢个指针进去（C/C++指针真TMD的好）
		pp->setIntoMap(&mapMM);
		pp->setWindowFlag(Qt::Window);
		pp->show();
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(this, tr("warning"), QString(tr("Vector超出:") + e.what() + "Infunction:on_property_clicked"));
		//return;
	}

	
}

