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
#include <QCHeckBox>
#include "qGboleData.h"
#include "QsLog.h"
#include <QSplitter>
#include <qfiledialog.h>
#include "dbcparser.h"
#include <qlabel.h>
#include "CustomTableWidgetDelegate.h"
QCanSetting::QCanSetting(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//让widget模态运行
	this->setWindowModality(Qt::ApplicationModal);
	this->userType = 0;
	InitUi();
	InitpGboleData();
	this->showMaximized();
	this->setWindowTitle(tr("CAN协议设置"));
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
	for (int m = 0; m < modelPb.size(); m++)
	{
		delete modelPb.at(m);
		
	}
	for (int m = 0; m < canIDPb.size(); m++)
	{
		delete canIDPb.at(m);

	}
	for (int m = 0; m < itemdataPb.size(); m++)
	{
		delete itemdataPb.at(m);

	}
}

void QCanSetting::closeEvent(QCloseEvent* event)
{
	event->accept();
	emit settingWidowsClose();
	QLOG_INFO() << "设备窗口关闭";
}
void QCanSetting::showEvent(QShowEvent* event)
{
	event->accept();
	
}
void QCanSetting::InitUi()
{
	QLOG_INFO() << "正在初始化设置界面……";
	QStringList listname = { tr("名称") , tr("协议") , tr("波特率") , tr("周期ms"),tr("标准帧"),tr("平台")};
	//listname << tr("名称") << tr("协议")<<tr("波特率")<<tr("周期ms");
	QPushButton* pbImportDBC = new QPushButton(tr("导入DBC"));
	connect(pbImportDBC, &QPushButton::clicked, this, &QCanSetting::on_importDBC_clicked);
	QPushButton* pbAddModel = new QPushButton("添加型号");
	pbAddModel->setFixedWidth(60);
	connect(pbAddModel, &QPushButton::clicked, this, &QCanSetting::on_pbAddModel_clicked);
	QPushButton* pbMoveUpModel = new QPushButton("上移");
	pbMoveUpModel->setFixedWidth(40);
	connect(pbMoveUpModel, &QPushButton::clicked, this, &QCanSetting::on_pbMoveUpModel_clicked);
	QPushButton* pbMoveDownModel = new QPushButton("下移");
	pbMoveDownModel->setFixedWidth(40);
	connect(pbMoveDownModel, &QPushButton::clicked, this, &QCanSetting::on_pbMoveDownModel_clicked);
	QPushButton* pbDelModel = new QPushButton("删除");
	pbDelModel->setFixedWidth(40);
	connect(pbDelModel, &QPushButton::clicked, this, &QCanSetting::on_pbDelModel_clicked);
	QPushButton* pbSaveModel = new QPushButton("保存");
	pbSaveModel->setFixedWidth(40);
	connect(pbSaveModel, &QPushButton::clicked, this, &QCanSetting::on_pbSaveModel_clicked);
	QPushButton* pbCopy = new QPushButton("复制");
	connect(pbCopy, &QPushButton::clicked, this, &QCanSetting::on_pbCopyModel_clicked);
	pbCopy->setFixedWidth(40);

	//pbSaveModel->setMinimumWidth(40);
	QHBoxLayout* hLayoutMdeol = new QHBoxLayout();
	hLayoutMdeol->addWidget(pbImportDBC);
	hLayoutMdeol->addWidget(pbAddModel);
	hLayoutMdeol->addWidget(pbMoveUpModel);
	hLayoutMdeol->addWidget(pbMoveDownModel);
	hLayoutMdeol->addWidget(pbDelModel);
	hLayoutMdeol->addWidget(pbSaveModel);
	hLayoutMdeol->addWidget(pbCopy);
	hLayoutMdeol->addSpacerItem(new QSpacerItem(60, 20, QSizePolicy::Expanding));
	hLayoutMdeol->setSpacing(0);
	modelPb.append(pbImportDBC);
	modelPb.append(pbAddModel);
	modelPb.append(pbMoveUpModel);
	modelPb.append(pbMoveDownModel);
	modelPb.append(pbDelModel);
	modelPb.append(pbCopy);

	modelView = new QTableWidget();
	modelView->setColumnCount(listname.size());
	modelView->setHorizontalHeaderLabels(listname);
	modelView->setSelectionBehavior(QAbstractItemView::SelectRows);
	//modelView->setMinimumWidth(400);
	connect(modelView, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(on_modelView_doubleClicked(int, int)));
	connect(modelView, SIGNAL(cellClicked(int, int)), this, SLOT(on_modelView_Clicked(int, int)));
	
	QVBoxLayout* vLayoutModel = new QVBoxLayout();
	vLayoutModel->addLayout(hLayoutMdeol);
	vLayoutModel->addWidget(modelView);
	//vLayoutModel->addSpacerItem(new QSpacerItem(20, 80, QSizePolicy::Expanding));

	listname.clear();
	listname << "CanId" << "接收/发送"<<"启用(Send时)"<<"DLC"<<"Circle";
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
	pbAddCanId->setFixedWidth(60);
	pbMoveUpCanId->setFixedWidth(40);
	pbMoveDownCanId->setFixedWidth(40);
	pbDelCanId->setFixedWidth(40);
	pbSaveCanId->setFixedWidth(40);
	QHBoxLayout* hLayoutCanId = new QHBoxLayout();
	hLayoutCanId->addWidget(pbAddCanId);
	hLayoutCanId->addWidget(pbMoveUpCanId);
	hLayoutCanId->addWidget(pbMoveDownCanId);
	hLayoutCanId->addWidget(pbDelCanId);
	hLayoutCanId->addWidget(pbSaveCanId);
	hLayoutCanId->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));
	hLayoutCanId->setSpacing(0);
	canIDPb.append(pbAddCanId);
	canIDPb.append(pbMoveUpCanId);
	canIDPb.append(pbMoveDownCanId);
	canIDPb.append(pbDelCanId);
	canIdView = new QTableWidget();
	canIdView->setColumnCount(5);
	canIdView->setHorizontalHeaderLabels(listname);
	canIdView->setSelectionBehavior(QAbstractItemView::SelectRows);
	canIdView->setMinimumWidth(350);
	connect(canIdView, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(on_canIdView_doubleClicked(int, int)));
	connect(canIdView, SIGNAL(cellClicked(int, int)), this, SLOT(on_canIdView_Clicked(int, int)));

	//自动化测试参数部分
	paramView = new QTableWidget(this);
	listname.clear();
	listname << tr("参数名称") << tr("参数值");
	paramView->setColumnCount(2);
	paramView->setHorizontalHeaderLabels(listname);
	connect(paramView, SIGNAL(cellChanged(int, int)), this, SLOT(on_paramView_cellChanged(int, int)));
	connect(paramView, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(on_paramView_doubleCLicked(int, int)));
	QLabel* autoTitle = new QLabel(this);
	autoTitle->setText("自动测试参数");

	QVBoxLayout* vLayoutCanId = new QVBoxLayout();
	vLayoutCanId->addLayout(hLayoutCanId);
	vLayoutCanId->addWidget(canIdView);
	vLayoutCanId->addWidget(autoTitle);
	vLayoutCanId->addWidget(paramView);
	
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
	hLayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));
	hLayout->setSpacing(0);
	itemdataPb.append(pbAddIteam);
	itemdataPb.append(pbMoveUpIteam);
	itemdataPb.append(pbMoveDownIteam);
	itemdataPb.append(pbDelIteam);

	//hLayout->addSpacerItem(new QSpacerItem(20, 80, QSizePolicy::Expanding));
	listname.clear();
	listname << tr("字段名称") << tr("起止字节") << tr("起止位") << tr("长度") << tr("精度") << tr("偏移量")<<tr("属性")<<tr("滚动显示")<<tr("数据来源")<<tr("16进制");
	
	tableView = new QTableWidget();
	tableView->setColumnCount(10);
	tableView->setHorizontalHeaderLabels(listname);
	tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	//tableView->setItemDelegateForColumn(1, new CustomTableWidgetDelegate(0, 7, tableView));
	//connect(tableView, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(on_tableView_doubleCLicked(int, int)));
	connect(tableView, SIGNAL(cellChanged(int, int)), this, SLOT(on_tableView_cellChanged(int, int)));
	tableView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	QVBoxLayout* vLayout = new QVBoxLayout();
	

	


	vLayout->addLayout(hLayout);
	vLayout->addWidget(tableView);
	//vLayout->addWidget(paramView);

	QSplitter* splitterHor = new QSplitter(Qt::Horizontal);
	QWidget* w1 = new QWidget(this);
	QWidget* w2 = new QWidget(this);
	QWidget* w3 = new QWidget(this);
	w1->setLayout(vLayoutModel);
	w2->setLayout(vLayoutCanId);
	w3->setLayout(vLayout);
	splitterHor->addWidget(w1);
	splitterHor->addWidget(w2);
	splitterHor->addWidget(w3);
	splitterHor->setStretchFactor(0, 3);
	splitterHor->setStretchFactor(1, 1);
	splitterHor->setStretchFactor(2, 4);
	QMargins M = QMargins(0, 0, 0, 0);
	splitterHor->setContentsMargins(M);
	QGridLayout* gg = new QGridLayout(this);
	gg->addWidget(splitterHor);
	//新建一个水平layout
	//QHBoxLayout* hLayoutAll = new QHBoxLayout();
	//hLayoutAll->addLayout(vLayoutModel);
	//hLayoutAll->addLayout(vLayoutCanId);
	//hLayoutAll->addLayout(vLayout);
	////设置比例，1：1：3
	//hLayoutAll->setStretch(0, 1);
	//hLayoutAll->setStretch(1, 1);
	//hLayoutAll->setStretch(2, 10);
	//hLayoutAll->setSpacing(5);
	this->setLayout(gg);
	
	
	QLOG_INFO() << "设置界面初始化完成，Good Job！";
}
void QCanSetting::setUserType(const int userTp) 
{ 
	this->userType = userTp;
	
}
void QCanSetting::reInitUI()
{
	InitpGboleData();
	for (int m = 0; m < modelPb.size(); m++)
	{
		modelPb.at(m)->setEnabled(this->userType);

	}
	for (int m = 0; m < canIDPb.size(); m++)
	{
		canIDPb.at(m)->setEnabled(this->userType);

	}
	for (int m = 0; m < itemdataPb.size(); m++)
	{
		itemdataPb.at(m)->setEnabled(this->userType);

	}
}
void QCanSetting::on_pbAddModel_clicked()
{
	
	if (!modelView)
		return;
	disconnect(modelView, SIGNAL(cellChanged(int, int)), this, SLOT(on_modelView_cellChangedV2(int, int)));
	short int row = modelView->rowCount();
	modelView->setRowCount(row + 1);
	QComboBox* proto = new QComboBox();
	proto->addItem(tr("Intel"));
	proto->addItem(tr("Motorola"));
	QComboBox* bundRa = new QComboBox();
	bundRa->addItem("200kb/s");
	bundRa->addItem("250kb/s");
	bundRa->addItem("500kb/s");
	bundRa->addItem("800kb/s");
	bundRa->addItem("2400b/s");
	bundRa->addItem("9600b/s");
	bundRa->addItem("10400b/s");
	bundRa->addItem("19200b/s");
	bundRa->setCurrentIndex(1);

	/*QComboBox* cbPlatform = new QComboBox();
	cbPlatform->addItem("无分类");
	cbPlatform->addItem("3kW");
	cbPlatform->addItem("5kW");
	cbPlatform->addItem("7kW");
	cbPlatform->addItem("10kW");
	cbPlatform->addItem("15kW");*/
	connect(proto, SIGNAL(currentIndexChanged(int)), this, SLOT(on_modelView_currentIndexChanged(int)));
	connect(bundRa, SIGNAL(currentIndexChanged(int)), this, SLOT(on_modelView_bundIndexChanged(int)));
	modelView->setItem(row, 0, new QTableWidgetItem(tr("型号")+QString::number(row)));
	modelView->setCellWidget(row, 1, proto);
	modelView->setCellWidget(row, 2, bundRa);
	modelView->setItem(row, 3, new QTableWidgetItem("1000"));

	QCheckBox* cbStandard = new QCheckBox(this);
	//扩展帧还是标准帧
	modelView->setCellWidget(row, 4, cbStandard);
	connect(cbStandard, SIGNAL(stateChanged(int)), this, SLOT(on_modelView_cbStandar(int)));
	//平台选项
	modelView->setItem(row, 5, new QTableWidgetItem("Non"));
	//cbPlatform->setCurrentIndex(0);
	//connect(cbPlatform, SIGNAL(currentIndexChanged(int)), this, SLOT(on_modelView_platFormIndexChanged(int)));
	struct protoData data;
	data.modelName = tr("型号") + QString::number(row);
	data.agreement = 0;
	data.bundRate = 1;
	data.bStandardId = 0;
	data.sPlatform = "Non";
	data.circle = 1000;
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
	if (this->userType == 0)
	{
		modelView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	}
	else
	{
		modelView->setEditTriggers(QAbstractItemView::DoubleClicked);
	}
	for (int i = 0; i < qGb->pGboleData.size(); ++i)
	{

		modelView->setRowCount(i + 1);
		QComboBox* proto = new QComboBox();
		proto->addItem(tr("Intel"));
		proto->addItem(tr("Motorola"));
		QComboBox* bundRa = new QComboBox();
		bundRa->addItem("200kb/s");
		bundRa->addItem("250kb/s");
		bundRa->addItem("500kb/s");
		bundRa->addItem("800kb/s");
		bundRa->addItem("2400b/s");
		bundRa->addItem("9600b/s");
		bundRa->addItem("10400b/s");
		bundRa->addItem("19200b/s");
		bundRa->setCurrentIndex(qGb->pGboleData.at(i).bundRate);

		

		modelView->setItem(i, 0, new QTableWidgetItem(qGb->pGboleData.at(i).modelName));
		modelView->setCellWidget(i, 1, proto);
		modelView->setCellWidget(i, 2, bundRa);
		uint circle = 1000;
		if (qGb->pGboleData.at(i).circle < 50)
		{
			circle = 50;
			qGb->pGboleData.at(i).circle = 50;
		}
		else if (qGb->pGboleData.at(i).circle > 100000)
		{
			circle = 1000;
			qGb->pGboleData.at(i).circle = 1000;
		}
		else
		{
			circle = qGb->pGboleData.at(i).circle;
		}

		modelView->setItem(i, 3, new QTableWidgetItem(QString::number(circle)));
		proto->setCurrentIndex(qGb->pGboleData.at(i).agreement);

		QCheckBox* cbStandard = new QCheckBox(this);
		modelView->setCellWidget(i, 4, cbStandard);
		if (qGb->pGboleData.at(i).bStandardId)
			cbStandard->setChecked(true);
		else
			cbStandard->setChecked(false);
		connect(cbStandard, SIGNAL(stateChanged(int)), this, SLOT(on_modelView_cbStandar(int)));
		connect(bundRa, SIGNAL(currentIndexChanged(int)), this, SLOT(on_modelView_bundIndexChanged(int)));
		connect(proto, SIGNAL(currentIndexChanged(int)), this, SLOT(on_modelView_currentIndexChanged(int)));


		//平台选项
		modelView->setItem(i, 5, new QTableWidgetItem(qGb->pGboleData.at(i).sPlatform));

		if (this->userType == 0) {
			bundRa->setEnabled(false);
			proto->setEnabled(false);
			cbStandard->setEnabled(false);
		}
		else 
		{ 
			bundRa->setEnabled(true);
			proto->setEnabled(true);
			cbStandard->setEnabled(true);
		}

	}
}

void QCanSetting::on_pbMoveUpModel_clicked()
{
	int curRow = modelView->currentRow();
	
	//最顶行或者没选中，不理
	if (curRow < 1)
	{
		return;
	}
	disconnect(modelView, SIGNAL(cellChanged(int, int)), this, SLOT(on_modelView_cellChangedV2(int, int)));
	//在它上面先插入一行
	modelView->insertRow(curRow - 1);
	//把这一行的所有列取出来
	
	QString idtex = modelView->item(curRow + 1, 0)->text();								//型号名称
	QComboBox* cb = dynamic_cast<QComboBox*>(modelView->cellWidget(curRow + 1, 1));		//格式：inter或者motorola
	QComboBox* cb2 = dynamic_cast<QComboBox*>(modelView->cellWidget(curRow + 1, 2));	//波特率
	QString cycle = modelView->item(curRow + 1, 3)->text();								//报文周期	
	QCheckBox* ch = dynamic_cast<QCheckBox*>(modelView->cellWidget(curRow + 1, 4));		//帧格式：标准或者扩展
	QString platf = modelView->item(curRow + 1, 5)->text();								//平台
	//在单元格要设置一个Item，不然直接丢元素进去
	//disconnect(modelView, SIGNAL(itemChanged(cellChanged(int, int))), this, SLOT(on_modelView_cellChangedV2(int, int)));
	modelView->setItem(curRow - 1, 0, new QTableWidgetItem(idtex));
	modelView->setCellWidget(curRow - 1, 1, cb);
	modelView->setCellWidget(curRow - 1, 2, cb2);
	modelView->setItem(curRow - 1, 3, new QTableWidgetItem(cycle));
	modelView->setCellWidget(curRow - 1, 4, ch);
	modelView->setItem(curRow - 1, 5, new QTableWidgetItem(platf));
	//删除原来行，因为在它前面新增了一行，所以序号要变
	modelView->removeRow(curRow + 1);
	modelView->setCurrentCell(curRow - 1, 0);
	qGboleData* qGb = qGboleData::getInstance();
	if (!qGb)return;
	if (curRow > qGb->pGboleData.size() - 1)
	{
		QMessageBox::warning(NULL, "warnning", tr("超出范围，不能上下移动"));
		return;
	}
	std::swap(qGb->pGboleData[curRow], qGb->pGboleData[curRow - 1]);
	
}

void QCanSetting::on_pbMoveDownModel_clicked()
{
	int curRow = modelView->currentRow();
	
	//最下面行或者没选中，不理
	if (curRow >= modelView->rowCount() - 1)
	{
		return;
	}
	
	/*modelView->insertRow(curRow + 2);
	QString idtex = modelView->item(curRow, 0)->text();
	QComboBox* cb = dynamic_cast<QComboBox*>(modelView->cellWidget(curRow, 1));
	modelView->setItem(curRow + 2, 0, new QTableWidgetItem(idtex));
	modelView->setCellWidget(curRow + 2, 1, cb);*/
	disconnect(modelView, SIGNAL(cellChanged(int, int)), this, SLOT(on_modelView_cellChangedV2(int, int)));
	modelView->insertRow(curRow + 2);
	QString idtex = modelView->item(curRow, 0)->text();							//型号名称
	QComboBox* cb = dynamic_cast<QComboBox*>(modelView->cellWidget(curRow, 1));		//格式：inter或者motorola
	QComboBox* cb2 = dynamic_cast<QComboBox*>(modelView->cellWidget(curRow, 2));	//波特率
	QString cycle = modelView->item(curRow, 3)->text();								//报文周期	
	QCheckBox* ch = dynamic_cast<QCheckBox*>(modelView->cellWidget(curRow, 4));		//帧格式：标准或者扩展
	QString platf = modelView->item(curRow, 5)->text();								//平台
	//在单元格要设置一个Item，不然直接丢元素进去
	modelView->setItem(curRow + 2, 0, new QTableWidgetItem(idtex));
	modelView->setCellWidget(curRow + 2, 1, cb);
	modelView->setCellWidget(curRow + 2, 2, cb2);
	modelView->setItem(curRow + 2, 3, new QTableWidgetItem(cycle));
	modelView->setCellWidget(curRow + 2, 4, ch);
	modelView->setItem(curRow + 2, 5, new QTableWidgetItem(platf));


	modelView->removeRow(curRow);
	modelView->setCurrentCell(curRow + 1, 0);
	qGboleData* qGb = qGboleData::getInstance();
	if (!qGb)return;
	if (curRow > qGb->pGboleData.size())
	{
		QMessageBox::warning(NULL, "warnning", tr("超出范围，不能上下移动"));
		return;
	}
	std::swap(qGb->pGboleData[curRow], qGb->pGboleData[curRow + 1]);
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
void QCanSetting::on_pbCopyModel_clicked()
{
	qGboleData* qGb = qGboleData::getInstance();
	if (!qGb)return;
	int curRow = modelView->currentRow();
	if (curRow > qGb->pGboleData.size() - 1)
		return;
	protoData bb = qGb->pGboleData.at(curRow);
	qGb->pGboleData.push_back(bb);
	SetTableData();
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
	

	canIdView->setItem(row, 0, new QTableWidgetItem(tr("123456")));
	canIdView->setCellWidget(row, 1, proto);

	QCheckBox* isSend = new QCheckBox();
	isSend->setChecked(false);
	canIdView->setCellWidget(row, 2, isSend);
	canIdView->setItem(row, 3, new QTableWidgetItem(tr("8")));
	canIdView->setItem(row, 4, new QTableWidgetItem(tr("-1")));
	canIdData cdata;
	cdata.strCanId = canIdView->item(row,0)->text();
	cdata.opt = 0;
	cdata.len = 8;
	cdata.circle = -1;
	qGb->pGboleData.at(curSelectRow).cItem.push_back(cdata);
	connect(isSend, &QCheckBox::stateChanged, this, &QCanSetting::on_canIdView_SendChanged);
	connect(proto, SIGNAL(currentIndexChanged(int)), this, SLOT(on_canIdView_currentIndexChanged(int)));
}

void QCanSetting::on_pbMoveUpCanId_clicked()
{
	int curRow = canIdView->currentRow();
	int modelCurRow = modelView->currentRow();
	//最顶行或者没选中，不理
	if (curRow < 1 )
	{
		return;
	}
	if (modelCurRow < 0)
	{
		QMessageBox::warning(NULL, "warnning", tr("未选中型号，不能上下移动"));
		return;
	}
	disconnect(canIdView, SIGNAL(cellChanged(int, int)), this, SLOT(on_canIdView_cellChanged(int, int)));
	//在它上面先插入一行
	canIdView->insertRow(curRow - 1);
	//把这一行的所有列取出来，也就两个
	QString idtex = canIdView->item(curRow + 1, 0)->text();
	QComboBox* cb = dynamic_cast<QComboBox*>(canIdView->cellWidget(curRow + 1, 1));
	QCheckBox* ch = dynamic_cast<QCheckBox*>(canIdView->cellWidget(curRow + 1, 2));
	QString len = canIdView->item(curRow + 1, 3)->text();
	QString circle = canIdView->item(curRow + 1, 4)->text();
	//在单元格要设置一个Item，不然直接丢元素进去
	canIdView->setItem(curRow - 1, 0, new QTableWidgetItem(idtex));
	canIdView->setCellWidget(curRow - 1, 1, cb);
	canIdView->setCellWidget(curRow - 1, 2, ch);
	canIdView->setItem(curRow - 1, 3, new QTableWidgetItem(len));
	canIdView->setItem(curRow - 1, 4, new QTableWidgetItem(circle));
	//删除原来行，因为在它前面新增了一行，所以序号要变
	canIdView->removeRow(curRow + 1);
	canIdView->setCurrentCell(curRow - 1, 0);
	qGboleData* qGb = qGboleData::getInstance();
	if (!qGb)return;
	if (curRow > qGb->pGboleData.at(modelCurRow).cItem.size() - 1)
	{
		QMessageBox::warning(NULL, "warnning", tr("超出范围，不能上下移动"));
		return;
	}
	std::swap(qGb->pGboleData.at(modelCurRow).cItem[curRow], qGb->pGboleData.at(modelCurRow).cItem[curRow - 1]);
}

void QCanSetting::on_pbMoveDownCanId_clicked()
{
	int curRow = canIdView->currentRow();
	int modelCurRow = modelView->currentRow();
	//最下面行或者没选中，不理
	if (curRow >= canIdView->rowCount()-1)
	{
		return;
	}
	if (modelCurRow < 0)
	{
		QMessageBox::warning(NULL, "warnning", tr("未选中型号，不能上下移动"));
		return;
	}
	disconnect(canIdView, SIGNAL(cellChanged(int, int)), this, SLOT(on_canIdView_cellChanged(int, int)));
	canIdView->insertRow(curRow + 2);
	QString idtex = canIdView->item(curRow , 0)->text();
	QComboBox* cb = dynamic_cast<QComboBox*>(canIdView->cellWidget(curRow, 1));
	QCheckBox* ch = dynamic_cast<QCheckBox*>(canIdView->cellWidget(curRow, 2));
	QString len = canIdView->item(curRow, 3)->text();
	QString circle = canIdView->item(curRow, 4)->text();
	canIdView->setItem(curRow + 2, 0, new QTableWidgetItem(idtex));
	canIdView->setCellWidget(curRow + 2, 1, cb);
	canIdView->setCellWidget(curRow + 2, 2, ch);
	canIdView->setItem(curRow + 2, 3, new QTableWidgetItem(len));
	canIdView->setItem(curRow + 2, 4, new QTableWidgetItem(circle));
	canIdView->removeRow(curRow );
	canIdView->setCurrentCell(curRow + 1, 0);
	qGboleData* qGb = qGboleData::getInstance();
	if (!qGb)return;
	if (curRow > qGb->pGboleData.at(modelCurRow).cItem.size())
	{
		QMessageBox::warning(NULL, "warnning", tr("超出范围，不能上下移动"));
		return;
	}
	std::swap(qGb->pGboleData.at(modelCurRow).cItem[curRow], qGb->pGboleData.at(modelCurRow).cItem[curRow + 1]);
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
	tableView->blockSignals(true);
	short int curSelectRow = modelView->currentRow();
	if (curSelectRow<0 || curSelectRow>qGb->pGboleData.size() - 1)
	{
		QMessageBox::warning(this, tr("warning"), tr("未选中型号，不能添加，要先选中型号，然后再选中CanID，再添加"));
		tableView->blockSignals(false);
		return;
	}
	short int curSelectCanRow = canIdView->currentRow();
	if (curSelectCanRow<0 || curSelectCanRow>qGb->pGboleData.at(curSelectRow).cItem.size() - 1)
	{
		QMessageBox::warning(this, tr("warning"), tr("未选中CanId，不能添加，要先选中型号，然后再选中CanID，再添加"));
		tableView->blockSignals(false);
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
	QCheckBox* isRoll = new QCheckBox();
	tableView->setCellWidget(row, 7, isRoll);
	connect(isRoll, &QCheckBox::stateChanged, this, &QCanSetting::on_CheckStateChanged);
	tableView->setItem(row, 8, new QTableWidgetItem("-1"));

	QCheckBox* isHex = new QCheckBox();
	tableView->setCellWidget(row, 9, isHex);
	isHex->setChecked(true);
	connect(isHex, &QCheckBox::stateChanged, this, &QCanSetting::on_isHexCheckStateChanged);
	protoItem pItem;
	pItem.bitName	= tableView->item(row, 0)->text();
	pItem.startByte = tableView->item(row, 1)->text().toInt(NULL, 10);
	pItem.startBit	= tableView->item(row, 2)->text().toInt(NULL, 10);
	pItem.bitLeng	= tableView->item(row, 3)->text().toInt(NULL, 10);
	pItem.precision = tableView->item(row, 4)->text().toFloat();
	pItem.offset	= tableView->item(row, 5)->text().toInt(NULL, 10);
	pItem.send = 0;
	pItem.isRoll = false;
	pItem.dataFrom = "-1";
	pItem.octhex = false;
	try
	{
		qGb->pGboleData.at(curSelectRow).cItem.at(curSelectCanRow).pItem.push_back(pItem);
	}
	catch (const std::exception&e)
	{
		QMessageBox::warning(this, tr("warning"), e.what());
	}
	catch(...) {
		QMessageBox::warning(this, tr("warning"),("Look this:"+QString::number(curSelectRow) + " and " + QString::number(curSelectCanRow)));
	}
	tableView->blockSignals(false);
}

void QCanSetting::on_pbMoveUpIteam_clicked()
{
	int canidCurRow = canIdView->currentRow();
	int modelCurRow = modelView->currentRow();
	int curRow = tableView->currentRow();
	//最顶行或者没选中，不理
	if (canidCurRow < 0 || modelCurRow < 0)
	{
		QMessageBox::warning(NULL, "warnning", tr("未选中型号或者CANID，不能上下移动"));
		return;
	}
	if (curRow<1)
	{
		return;
	}
	//关闭信号槽，因为下面的setItem会触发cellCHanged这个信号
	//disconnect(tableView, SIGNAL(cellChanged(int, int)), this, SLOT(on_tableView_cellChanged(int, int)));
	tableView->blockSignals(true);
	//在它上面先插入一行
	tableView->insertRow(curRow - 1);
	//把这行的所有列数据取出来
	QString str1 = tableView->item(curRow + 1, 0)->text();
	QString str2 = tableView->item(curRow + 1, 1)->text();
	QString str3 = tableView->item(curRow + 1, 2)->text();
	QString str4 = tableView->item(curRow + 1, 3)->text();
	QString str5 = tableView->item(curRow + 1, 4)->text();
	QString str6 = tableView->item(curRow + 1, 5)->text();
	QPushButton* qpb = dynamic_cast<QPushButton*>(tableView->cellWidget(curRow + 1, 6));
	QCheckBox* qcb = dynamic_cast<QCheckBox*>(tableView->cellWidget(curRow + 1, 7));
	QString str9 = tableView->item(curRow + 1, 8)->text();
	QCheckBox* qcb2 = dynamic_cast<QCheckBox*>(tableView->cellWidget(curRow + 1, 9));
	//放到新插入的那一行
	tableView->setItem(curRow - 1, 0, new QTableWidgetItem(str1));
	tableView->setItem(curRow - 1, 1, new QTableWidgetItem(str2));
	tableView->setItem(curRow - 1, 2, new QTableWidgetItem(str3));
	tableView->setItem(curRow - 1, 3, new QTableWidgetItem(str4));
	tableView->setItem(curRow - 1, 4, new QTableWidgetItem(str5));
	tableView->setItem(curRow - 1, 5, new QTableWidgetItem(str6));
	tableView->setCellWidget(curRow - 1, 6, qpb);
	tableView->setCellWidget(curRow - 1, 7, qcb);
	tableView->setItem(curRow - 1, 8, new QTableWidgetItem(str9));
	tableView->setCellWidget(curRow - 1, 9, qcb2);
	tableView->removeRow(curRow + 1);
	tableView->setCurrentCell(curRow - 1, 0);
	qGboleData* qGb = qGboleData::getInstance();
	if (!qGb)return;
	if (modelCurRow > qGb->pGboleData.size() - 1)
	{
		QMessageBox::warning(NULL, "warnning", QString("超出范围，不能上下移动"));
		tableView->blockSignals(false);
		return;
	}
	if (canidCurRow > qGb->pGboleData.at(modelCurRow).cItem.size() - 1)
	{
		QMessageBox::warning(NULL, "warnning", tr("超出范围，不能上下移动"));
		tableView->blockSignals(false);
		return;
	}
	if (curRow > qGb->pGboleData.at(modelCurRow).cItem.at(canidCurRow).pItem.size() - 1)
	{
		QMessageBox::warning(NULL, "warnning", tr("超出范围，不能上下移动"));
		tableView->blockSignals(false);
		return;
	}
	//交换两行的数据
	std::swap(qGb->pGboleData.at(modelCurRow).cItem.at(canidCurRow).pItem[curRow], qGb->pGboleData.at(modelCurRow).cItem.at(canidCurRow).pItem[curRow - 1]);
	tableView->blockSignals(false);
}

void QCanSetting::on_pbMoveDownIteam_clicked()
{
	int canidCurRow = canIdView->currentRow();
	int modelCurRow = modelView->currentRow();
	int curRow = tableView->currentRow();
	//最下面行或者没选中，不理
	if (canidCurRow < 0 || modelCurRow < 0)
	{
		QMessageBox::warning(NULL, "warnning", tr("未选中型号或者CANID，不能上下移动"));
		return;
	}
	if (curRow >= tableView->rowCount()-1)
	{
		return;
	}
	//关闭信号槽，因为下面的setItem会触发cellCHanged这个信号
	disconnect(tableView, SIGNAL(cellChanged(int, int)), this, SLOT(on_tableView_cellChanged(int, int)));
	//在它上面先插入一行
	tableView->insertRow(curRow + 2);
	//把这行的所有列数据取出来
	QString str1 = tableView->item(curRow , 0)->text();
	QString str2 = tableView->item(curRow , 1)->text();
	QString str3 = tableView->item(curRow , 2)->text();
	QString str4 = tableView->item(curRow , 3)->text();
	QString str5 = tableView->item(curRow , 4)->text();
	QString str6 = tableView->item(curRow , 5)->text();
	QPushButton* qpb = dynamic_cast<QPushButton*>(tableView->cellWidget(curRow , 6));
	QCheckBox* qcb = dynamic_cast<QCheckBox*>(tableView->cellWidget(curRow , 7));
	QString str9 = tableView->item(curRow , 8)->text();
	QCheckBox* qcb2 = dynamic_cast<QCheckBox*>(tableView->cellWidget(curRow, 9));
	//放到新插入的那一行
	tableView->setItem(curRow + 2, 0, new QTableWidgetItem(str1));
	tableView->setItem(curRow + 2, 1, new QTableWidgetItem(str2));
	tableView->setItem(curRow + 2, 2, new QTableWidgetItem(str3));
	tableView->setItem(curRow + 2, 3, new QTableWidgetItem(str4));
	tableView->setItem(curRow + 2, 4, new QTableWidgetItem(str5));
	tableView->setItem(curRow + 2, 5, new QTableWidgetItem(str6));
	tableView->setCellWidget(curRow + 2, 6, qpb);
	tableView->setCellWidget(curRow + 2, 7, qcb);
	tableView->setItem(curRow + 2, 8, new QTableWidgetItem(str9));
	tableView->setCellWidget(curRow + 2, 9, qcb2);
	tableView->removeRow(curRow);

	tableView->setCurrentCell(curRow + 1, 0);
	qGboleData* qGb = qGboleData::getInstance();
	if (!qGb)return;
	if (modelCurRow > qGb->pGboleData.size() - 1)
	{
		QMessageBox::warning(NULL, "warnning", tr("超出范围，不能上下移动"));
		return;
	}
	if (canidCurRow > qGb->pGboleData.at(modelCurRow).cItem.size() - 1)
	{
		QMessageBox::warning(NULL, "warnning", tr("超出范围，不能上下移动"));
		return;
	}
	if (curRow > qGb->pGboleData.at(modelCurRow).cItem.at(canidCurRow).pItem.size())
	{
		QMessageBox::warning(NULL, "warnning", tr("超出范围，不能上下移动"));
		return;
	}
	std::swap(qGb->pGboleData.at(modelCurRow).cItem.at(canidCurRow).pItem[curRow], qGb->pGboleData.at(modelCurRow).cItem.at(canidCurRow).pItem[curRow + 1]);
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
	try
	{
		if (curSelectCanRow<0 || curSelectCanRow>qGb->pGboleData.at(curSelectRow).cItem.size() - 1)
		{
			QMessageBox::warning(this, tr("warning"), tr("未选中CanId，不能删除，要先选中型号，然后再选中CanID，再删除"));
			return;
		}
		//超出范围
		if (index<0 || index > qGb->pGboleData.at(curSelectRow).cItem.at(curSelectCanRow).pItem.size()-1)
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
	catch (const std::exception&e)
	{
		QLOG_WARN() << "删除行时Vector超出范围：" << e.what();
		QMessageBox::warning(NULL, tr("警告"), tr("删除行时Vector超出范围\non_pbDelIteam_clicked"));
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
void QCanSetting::on_modelView_bundIndexChanged(int index)
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
	qGb->pGboleData.at(row).bundRate = cbx->currentIndex();
}



void QCanSetting::on_modelView_cellChangedV2(int row, int col)
{
	qGboleData* qGb = qGboleData::getInstance();
	if (!qGb)return;
	if (row<0 || row>qGb->pGboleData.size() - 1)
	{
		return;
	}
	if(0==col)
		qGb->pGboleData.at(row).modelName = modelView->item(row,col)->text();
	else if (col == 3)
	{
		qGb->pGboleData.at(row).circle = modelView->item(row, col)->text().toInt();
	}
	else if (col == 5)
	{
		qGb->pGboleData.at(row).sPlatform = modelView->item(row, col)->text();
	}
	//关掉这个，防止添加行setData的时候，触发这个信号
	//disconnect(modelView, SIGNAL(itemChanged(cellChanged(int, int))), this, SLOT(on_modelView_cellChangedV2(int, int)));
	disconnect(modelView, SIGNAL(cellChanged(int, int)), this, SLOT(on_modelView_cellChangedV2(int, int)));
}

void QCanSetting::on_modelView_doubleClicked(int, int)
{
	//注意QT的这个cellChanged信号，他会当setData的时候触发这个信号
	//所以曲线救国，在双击的时候才连接这个槽，
	connect(modelView, SIGNAL(cellChanged(int, int)), this, SLOT(on_modelView_cellChangedV2(int, int)));
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
	if (0 == this->userType)
	{
		canIdView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	}
	else
	{
		canIdView->setEditTriggers(QAbstractItemView::DoubleClicked);
	}
	try
	{
		for (int i = 0; i < qGb->pGboleData.at(row).cItem.size(); ++i)
		{
			canIdView->setRowCount(i + 1);
			QComboBox* proto = new QComboBox();
			proto->addItem(tr("接收"));
			proto->addItem(tr("发送"));
			proto->setCurrentIndex(qGb->pGboleData.at(row).cItem.at(i).opt);
			//QString mt = "0x" + QString("%1").arg(qGb->pGboleData.at(row).cItem.at(i).CanId, QString::number(qGb->pGboleData.at(row).cItem.at(i).CanId, 16).length(), 16).toUpper().trimmed();
			QString mt = qGb->pGboleData.at(row).cItem.at(i).strCanId;
			canIdView->setItem(i, 0, new QTableWidgetItem(mt));
			canIdView->setCellWidget(i, 1, proto);
			connect(proto, SIGNAL(currentIndexChanged(int)), this, SLOT(on_canIdView_currentIndexChanged(int)));

			QCheckBox* isSend = new QCheckBox();
			isSend->setChecked(qGb->pGboleData.at(row).cItem.at(i).isSend);
			canIdView->setCellWidget(i, 2, isSend);
			connect(isSend, &QCheckBox::stateChanged, this, &QCanSetting::on_canIdView_SendChanged);
			QString len = QString::number(qGb->pGboleData.at(row).cItem.at(i).len);
			canIdView->setItem(i, 3, new QTableWidgetItem(len));
			canIdView->setItem(i, 4, new QTableWidgetItem(QString::number(qGb->pGboleData.at(row).cItem.at(i).circle)));

			if (0 == this->userType)
			{
				proto->setEnabled(false);
				isSend->setEnabled(false);
			}
			else
			{
				proto->setEnabled(true);
				isSend->setEnabled(true);
			}
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
	int mc = paramView->rowCount();
	for (int p = 0; p < paramView->rowCount(); p++)
		paramView->removeRow(mc - p - 1);
	paramView->setRowCount(26);
	paramView->setItem(0, 0, new QTableWidgetItem(QString("使能所在行")));
	paramView->setItem(0, 1, new QTableWidgetItem(QString::number(qGb->pGboleData.at(row).ats.m_iEnableInLine)));
	paramView->setItem(1, 0, new QTableWidgetItem(QString("使能操作")));
	paramView->setItem(1, 1, new QTableWidgetItem(QString::number(qGb->pGboleData.at(row).ats.m_iEnOp)));
	paramView->setItem(2, 0, new QTableWidgetItem(QString("功率所在行")));
	paramView->setItem(2, 1, new QTableWidgetItem(QString::number(qGb->pGboleData.at(row).ats.m_iPowerInLine)));
	paramView->setItem(3, 0, new QTableWidgetItem(QString("欠压阈值V")));
	paramView->setItem(3, 1, new QTableWidgetItem(QString::number(qGb->pGboleData.at(row).ats.m_iLowVoltage)));
	paramView->setItem(4, 0, new QTableWidgetItem(QString("欠压恢复V")));
	paramView->setItem(4, 1, new QTableWidgetItem(QString::number(qGb->pGboleData.at(row).ats.m_iLowVoltageRe)));


	paramView->setItem(5, 0, new QTableWidgetItem(QString("过压阈值V")));
	paramView->setItem(5, 1, new QTableWidgetItem(QString::number(qGb->pGboleData.at(row).ats.m_iOverVoltage)));
	paramView->setItem(6, 0, new QTableWidgetItem(QString("过压恢复V")));
	paramView->setItem(6, 1, new QTableWidgetItem(QString::number(qGb->pGboleData.at(row).ats.m_iOverVoltageRe)));

	paramView->setItem(7, 0, new QTableWidgetItem(QString("过温保护°C")));
	paramView->setItem(7, 1, new QTableWidgetItem(QString::number(qGb->pGboleData.at(row).ats.m_iOverTemperature)));
	paramView->setItem(8, 0, new QTableWidgetItem(QString("过温恢复°C")));
	paramView->setItem(8, 1, new QTableWidgetItem(QString::number(qGb->pGboleData.at(row).ats.m_iOverTempRe)));
	paramView->setItem(9, 0, new QTableWidgetItem(QString("过温OK/NG误差°C")));
	paramView->setItem(9, 1, new QTableWidgetItem(QString::number(qGb->pGboleData.at(row).ats.m_iOverTempTolerance)));


	paramView->setItem(10, 0, new QTableWidgetItem(QString("额定电压V")));
	paramView->setItem(10, 1, new QTableWidgetItem(QString::number(qGb->pGboleData.at(row).ats.m_iRatedVolt)));

	paramView->setItem(11, 0, new QTableWidgetItem(QString("功率请求大小")));
	paramView->setItem(11, 1, new QTableWidgetItem(QString::number(qGb->pGboleData.at(row).ats.m_fRequirePW)));
	paramView->setItem(12, 0, new QTableWidgetItem(QString("额定功率")));
	paramView->setItem(12, 1, new QTableWidgetItem(QString::number(qGb->pGboleData.at(row).ats.m_fRatedPW)));
	paramView->setItem(13, 0, new QTableWidgetItem(QString("额定功率误差")));
	paramView->setItem(13, 1, new QTableWidgetItem(qGb->pGboleData.at(row).ats.m_strTolerance));
	

	paramView->setItem(14, 0, new QTableWidgetItem(QString("软件版本")));
	paramView->setItem(14, 1, new QTableWidgetItem(qGb->pGboleData.at(row).ats.m_strVer));
	paramView->setItem(15, 0, new QTableWidgetItem(QString("版本接收ID")));
	paramView->setItem(15, 1, new QTableWidgetItem(qGb->pGboleData.at(row).ats.m_strVerRecID));
	paramView->setItem(16, 0, new QTableWidgetItem(QString("版本号个数")));
	paramView->setItem(16, 1, new QTableWidgetItem(QString::number(qGb->pGboleData.at(row).ats.m_usIDBytelen)));
	paramView->setItem(17, 0, new QTableWidgetItem(QString("Ver起止位")));
	paramView->setItem(17, 1, new QTableWidgetItem(QString::number(qGb->pGboleData.at(row).ats.m_usIDByteStartbit)));

	paramView->setItem(18, 0, new QTableWidgetItem(QString("测额定功率温度°C")));
	paramView->setItem(18, 1, new QTableWidgetItem(QString::number(qGb->pGboleData.at(row).ats.m_usRatedPWTemp)));
	paramView->setItem(19, 0, new QTableWidgetItem(QString("测额定功率流量L")));
	paramView->setItem(19, 1, new QTableWidgetItem(QString::number(qGb->pGboleData.at(row).ats.m_usRatedPWFlow)));
	paramView->setItem(20, 0, new QTableWidgetItem(QString("加热起始温度°C")));
	paramView->setItem(20, 1, new QTableWidgetItem(QString::number(qGb->pGboleData.at(row).ats.m_usHeatTemp)));
	paramView->setItem(21, 0, new QTableWidgetItem(QString("制冷温度°C")));
	paramView->setItem(21, 1, new QTableWidgetItem(QString::number(qGb->pGboleData.at(row).ats.m_usCoolTemp)));

	paramView->setItem(22, 0, new QTableWidgetItem(QString("测过温关流量？")));
	paramView->setItem(22, 1, new QTableWidgetItem(QString::number(qGb->pGboleData.at(row).ats.m_bTurnOffFlow)));
	paramView->setItem(23, 0, new QTableWidgetItem(QString("测功率关制冷？")));
	paramView->setItem(23, 1, new QTableWidgetItem(QString::number(qGb->pGboleData.at(row).ats.m_bTurnOffCool)));

	paramView->setItem(24, 0, new QTableWidgetItem(QString("出口停止使能温度")));
	paramView->setItem(24, 1, new QTableWidgetItem(QString::number(qGb->pGboleData.at(row).ats.m_iOutTempStop)));
	paramView->setItem(25, 0, new QTableWidgetItem(QString("参考出(0)入(1)水口温度")));
	paramView->setItem(25, 1, new QTableWidgetItem(QString::number(qGb->pGboleData.at(row).ats.m_usOutOrInTemp)));

}

void QCanSetting::on_modelView_cbStandar(int bStandard)
{
	qGboleData* qGb = qGboleData::getInstance();
	if (!qGb)return;
	QCheckBox* cb = dynamic_cast<QCheckBox*>(sender());
	if (!cb)
		return;
	if (!modelView) return;
	
	
	QModelIndex pIndex = modelView->indexAt(QPoint(cb->frameGeometry().x(), cb->frameGeometry().y()));
	int mCurRow = pIndex.row();
	if (mCurRow < 0 || mCurRow>qGb->pGboleData.size() - 1) {
		QMessageBox::warning(this, tr("warning"), tr("超出界限， 不能修改"));
		return;
	}
	qGb->pGboleData.at(mCurRow).bStandardId = cb->isChecked();

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
void QCanSetting::on_canIdView_SendChanged(int check)
{
	qGboleData* qGb = qGboleData::getInstance();
	if (!qGb)return;
	QCheckBox* cb = dynamic_cast<QCheckBox*>(sender());
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

	if (row > qGb->pGboleData.at(mCurRow).cItem.size() - 1 || row < 0)
	{
		QMessageBox::warning(this, tr("warning"), tr("修改的位置超出范围"));
		return;
	}
	try
	{
		bool b = check > 0 ? true : false;
		qGb->pGboleData.at(mCurRow).cItem.at(row).isSend = b;
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(this, tr("warning"), QString(tr("Vector超出:") + e.what() + "Infunction:on_canIdView_currentIndexChanged"));
	}
}

/*
* @brief：第二层表格修改后的槽函数，用于保存修改后的值
* @param row：被修改的单元格的所在行
* @param col：被修改的单元格的所在列
* @return：无
*/
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
		//不在范围
		if (row > qGb->pGboleData.at(mCurRow).cItem.size() - 1 || row <0)
		{
			QMessageBox::warning(this, tr("warning"), tr("修改的位置超出范围"));
			return;
		}
		if(0==col)
			qGb->pGboleData.at(mCurRow).cItem.at(row).strCanId = canIdView->item(row,col)->text().trimmed();
		else if(3==col)
			qGb->pGboleData.at(mCurRow).cItem.at(row).len = canIdView->item(row, col)->text().trimmed().toInt();
		else if(4==col)
			qGb->pGboleData.at(mCurRow).cItem.at(row).circle = canIdView->item(row, col)->text().trimmed().toInt();
	}
	catch (const std::exception&e)
	{
		QMessageBox::warning(this, tr("warning"), QString(tr("Vector超出:") + e.what() + "Infunction:on_canIdView_cellChanged"));
	}
	//关掉这个，防止添加行setData的时候，触发这个信号
	disconnect(canIdView, SIGNAL(cellChanged(int, int)), this, SLOT(on_canIdView_cellChanged(int, int)));
}
/*
* @brief：第二层的双击事件响应，建立被编辑后的槽
*/
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
	if (0 == this->userType)
	{
		tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	}
	else
	{
		tableView->setEditTriggers(QAbstractItemView::DoubleClicked);
	}
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
			QCheckBox* isRoll = new QCheckBox();
			if (qGb->pGboleData.at(mRow).cItem.at(row).pItem.at(i).isRoll)
				isRoll->setCheckState(Qt::Checked);
			tableView->setCellWidget(i, 7, isRoll);
			//这个connect一定放在setCheckState(Qt::Checked);这个函数后面，不然一调用这个函数，就会触发信号
			connect(isRoll, &QCheckBox::stateChanged, this, &QCanSetting::on_CheckStateChanged);
			tableView->setItem(i, 8, new QTableWidgetItem(qGb->pGboleData.at(mRow).cItem.at(row).pItem.at(i).dataFrom));
			QCheckBox* isHex = new QCheckBox();
			tableView->setCellWidget(i, 9, isHex);
			isHex->setChecked(qGb->pGboleData.at(mRow).cItem.at(row).pItem.at(i).octhex);
			connect(isHex, &QCheckBox::stateChanged, this, &QCanSetting::on_isHexCheckStateChanged);
			if (0 == this->userType)
			{
				property->setEnabled(false);
				isRoll->setEnabled(false);
				isHex->setEnabled(false);
			}
			else
			{
				property->setEnabled(true);
				isRoll->setEnabled(true);
				isHex->setEnabled(true);
			}
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
			qGb->pGboleData.at(mCurRow).cItem.at(cCurRow).pItem.at(row).precision = tableView->item(row, col)->text().toFloat();
			break;
		case 5:
			qGb->pGboleData.at(mCurRow).cItem.at(cCurRow).pItem.at(row).offset = tableView->item(row, col)->text().toInt();
			break;
		case 8:
			qGb->pGboleData.at(mCurRow).cItem.at(cCurRow).pItem.at(row).dataFrom = tableView->item(row, col)->text().trimmed();
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
	//disconnect(tableView, SIGNAL(cellChanged(int, int)), this, SLOT(on_tableView_cellChanged(int, int)));
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
		//std::map<QString, cellProperty>&mapMM = qGb->pGboleData.at(mCurRow).cItem.at(cCurRow).pItem.at(row).itemProperty;
		//取出该项的vector
		std::vector<cellProperty>& stlMM = qGb->pGboleData.at(mCurRow).cItem.at(cCurRow).pItem.at(row).stl_itemProperty;
		//丢个指针进去（C/C++指针真TMD的好）
		pp->setIntoMap(&stlMM);
		pp->setWindowFlag(Qt::Window);
		pp->show();
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(this, tr("warning"), QString(tr("Vector超出:") + e.what() + "Infunction:on_property_clicked"));
		//return;
	}

	
}

void QCanSetting::on_CheckStateChanged(int isCheck)
{
	QCheckBox* pb = dynamic_cast<QCheckBox*>(sender());
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
		if (row > qGb->pGboleData.at(mCurRow).cItem.at(cCurRow).pItem.size() - 1 || row < 0)
		{
			QMessageBox::warning(this, tr("warning"), QString(tr("字段vector超出范围：\nrow > pGboleData.at(mCurRow).cItem.at(cCurRow).pItem.size() - 1")));
			return;
		}
		qGb->pGboleData.at(mCurRow).cItem.at(cCurRow).pItem.at(row).isRoll = isCheck==2?true:false;
		
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(this, tr("warning"), QString(tr("Vector超出:") + e.what() + "Infunction:on_CheckStateChanged(int isCheck)"));
		//return;
	}
}
void QCanSetting::on_isHexCheckStateChanged(int isCheck)
{
	QCheckBox* pb = dynamic_cast<QCheckBox*>(sender());
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
		if (row > qGb->pGboleData.at(mCurRow).cItem.at(cCurRow).pItem.size() - 1 || row < 0)
		{
			QMessageBox::warning(this, tr("warning"), QString(tr("字段vector超出范围：\nrow > pGboleData.at(mCurRow).cItem.at(cCurRow).pItem.size() - 1")));
			return;
		}
		qGb->pGboleData.at(mCurRow).cItem.at(cCurRow).pItem.at(row).octhex = isCheck == 2 ? true : false;

	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(this, tr("warning"), QString(tr("Vector超出:") + e.what() + "Infunction:on_CheckStateChanged(int isCheck)"));
		//return;
	}
}

void QCanSetting::on_importDBC_clicked()
{
	QString path = QFileDialog::getOpenFileName(this, tr("选择.DBC文件"), QApplication::applicationDirPath(), tr("DBC(*.dbc)"));
	DbcParser *dp = new ParserZ(path);
	bool res = false;
	protoData dd =  dp->parserdbc(res);
	if (res)
	{
		qGboleData* qGb = qGboleData::getInstance();
		if (!qGb)return;
		qGb->pGboleData.push_back(dd);
		SetTableData();
	}

}

void QCanSetting::on_pbSaveItemParam_clicked()
{
}

void QCanSetting::on_paramView_doubleCLicked(int, int)
{
	connect(paramView, SIGNAL(cellChanged(int, int)), this, SLOT(on_paramView_cellChanged(int, int)));
}
/*
* @brief: Slot function。responed by autotest paramter changed
* @paramter1:int row cell row
* @paramter2:int col cell column
*/
void QCanSetting::on_paramView_cellChanged(int row, int col)
{
	if ( col > 1 || row < 0 || col < 0)
	{
		disconnect(paramView, SIGNAL(cellChanged(int, int)), this, SLOT(on_paramView_cellChanged(int, int)));
		return;
	}
	if (col != 1)
	{
		disconnect(paramView, SIGNAL(cellChanged(int, int)), this, SLOT(on_paramView_cellChanged(int, int)));
		return;
	}
	int n = modelView->currentRow();
	qGboleData* qGb = qGboleData::getInstance();
	if (!qGb)
	{
		disconnect(paramView, SIGNAL(cellChanged(int, int)), this, SLOT(on_paramView_cellChanged(int, int)));
		return;
	}
	if (n < 0 || n>qGb->pGboleData.size() - 1)
	{
		disconnect(paramView, SIGNAL(cellChanged(int, int)), this, SLOT(on_paramView_cellChanged(int, int)));
		return;
	}
	switch (row)
	{
	case 0:
		qGb->pGboleData.at(n).ats.m_iEnableInLine = paramView->item(row, col)->text().toInt();
		break;
	case 1:
		qGb->pGboleData.at(n).ats.m_iEnOp = paramView->item(row, col)->text().toInt();
		break;
	case 2:
		qGb->pGboleData.at(n).ats.m_iPowerInLine = paramView->item(row, col)->text().toInt();
		break;
	case 3:
		qGb->pGboleData.at(n).ats.m_iLowVoltage = paramView->item(row, col)->text().toInt();
		break;
	case 4:
		qGb->pGboleData.at(n).ats.m_iLowVoltageRe = paramView->item(row, col)->text().toInt();
		break;
	case 5:
		qGb->pGboleData.at(n).ats.m_iOverVoltage = paramView->item(row, col)->text().toInt();
		break;
	case 6:
		qGb->pGboleData.at(n).ats.m_iOverVoltageRe = paramView->item(row, col)->text().toInt();
		break;
	case 7:
		qGb->pGboleData.at(n).ats.m_iOverTemperature = paramView->item(row, col)->text().toInt();
		break;
	case 8:
		qGb->pGboleData.at(n).ats.m_iOverTempRe = paramView->item(row, col)->text().toInt();
		break;
	case 9:
		qGb->pGboleData.at(n).ats.m_iOverTempTolerance = paramView->item(row, col)->text().toInt();
		break;
	case 10:
		qGb->pGboleData.at(n).ats.m_iRatedVolt = paramView->item(row, col)->text().toInt();
		break;
	case 11:
		qGb->pGboleData.at(n).ats.m_fRequirePW = paramView->item(row, col)->text().toInt();
		break;
	case 12:
		qGb->pGboleData.at(n).ats.m_fRatedPW = paramView->item(row, col)->text().toInt();
		break;
	case 13:
		qGb->pGboleData.at(n).ats.m_strTolerance = paramView->item(row, col)->text();
		break;
	case 14:
		qGb->pGboleData.at(n).ats.m_strVer = paramView->item(row, col)->text();
		break;
	case 15:
		qGb->pGboleData.at(n).ats.m_strVerRecID = paramView->item(row, col)->text();
		break;
	case 16:
		qGb->pGboleData.at(n).ats.m_usIDBytelen = paramView->item(row, col)->text().toUShort();
		break;
	case 17:
		qGb->pGboleData.at(n).ats.m_usIDByteStartbit = paramView->item(row, col)->text().toUShort();
		break;
	case 18:
		qGb->pGboleData.at(n).ats.m_usRatedPWTemp = paramView->item(row, col)->text().toShort();
		break;
	case 19:
		qGb->pGboleData.at(n).ats.m_usRatedPWFlow = paramView->item(row, col)->text().toUShort();
		break;
	case 20:
		qGb->pGboleData.at(n).ats.m_usHeatTemp = paramView->item(row, col)->text().toShort();
		break;
	case 21:
		qGb->pGboleData.at(n).ats.m_usCoolTemp = paramView->item(row, col)->text().toShort();
		break;
	case 22:
		qGb->pGboleData.at(n).ats.m_bTurnOffFlow = paramView->item(row, col)->text().toShort();
		break;
	case 23:
		qGb->pGboleData.at(n).ats.m_bTurnOffCool = paramView->item(row, col)->text().toShort();
		break;
	case 24:
		qGb->pGboleData.at(n).ats.m_iOutTempStop = paramView->item(row, col)->text().toInt();
		break;
	case 25:
		qGb->pGboleData.at(n).ats.m_usOutOrInTemp = paramView->item(row, col)->text().toInt();
		break;
	default:
		break;
	}
	disconnect(paramView, SIGNAL(cellChanged(int, int)), this, SLOT(on_paramView_cellChanged(int, int)));
}
