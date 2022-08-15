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
QCanSetting::QCanSetting(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
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
	connect(pbAddModel, &QPushButton::clicked, this, &QCanSetting::on_pbAddModel_clicked);
	QPushButton* pbMoveUpModel = new QPushButton("上移");
	connect(pbMoveUpModel, &QPushButton::clicked, this, &QCanSetting::on_pbMoveUpModel_clicked);
	QPushButton* pbMoveDownModel = new QPushButton("下移");
	connect(pbMoveDownModel, &QPushButton::clicked, this, &QCanSetting::on_pbMoveDownModel_clicked);
	QPushButton* pbDelModel = new QPushButton("删除");
	connect(pbDelModel, &QPushButton::clicked, this, &QCanSetting::on_pbDelModel_clicked);
	QPushButton* pbSaveModel = new QPushButton("保存");
	connect(pbSaveModel, &QPushButton::clicked, this, &QCanSetting::on_pbSaveModel_clicked);
	QHBoxLayout* hLayoutMdeol = new QHBoxLayout();
	hLayoutMdeol->addWidget(pbAddModel);
	hLayoutMdeol->addWidget(pbMoveUpModel);
	hLayoutMdeol->addWidget(pbMoveDownModel);
	hLayoutMdeol->addWidget(pbDelModel);
	hLayoutMdeol->addWidget(pbSaveModel);
	hLayoutMdeol->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));

	modelView = new QTableWidget();
	modelView->setColumnCount(2);
	modelView->setHorizontalHeaderLabels(listname);
	modelView->setSelectionBehavior(QAbstractItemView::SelectRows);
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
	QHBoxLayout* hLayoutCanId = new QHBoxLayout();
	hLayoutCanId->addWidget(pbAddCanId);
	hLayoutCanId->addWidget(pbMoveUpCanId);
	hLayoutCanId->addWidget(pbMoveDownCanId);
	hLayoutCanId->addWidget(pbDelCanId);
	hLayoutCanId->addWidget(pbSaveCanId);
	hLayoutCanId->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));

	canIdView = new QTableWidget();
	canIdView->setColumnCount(2);
	canIdView->setHorizontalHeaderLabels(listname);
	canIdView->setSelectionBehavior(QAbstractItemView::SelectRows);
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
	//hLayout->addSpacerItem(new QSpacerItem(20, 80, QSizePolicy::Expanding));
	listname.clear();
	listname << "字段名称" << "起止字节" << "起止位" << "长度" << "精度" << "偏移量";
	
	tableView = new QTableWidget();
	tableView->setColumnCount(6);
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
	pGboleData.push_back(data);
	
}

void QCanSetting::savepGboleData()
{
	QString appPath = QApplication::applicationDirPath() + "/Data";
	QDir dir(appPath);
	if (!dir.exists())
	{
		if (!dir.mkpath(appPath))
		{
			QMessageBox::warning(this, tr("warning"), tr("创建保存目录失败"));
			return;
		}
	}
	
	QJsonObject mItemObjRoot;
	for (int i = 0; i < pGboleData.size(); i++)
	{

		QJsonObject mItem;
		mItem.insert("modelName", pGboleData.at(i).modelName);
		mItem.insert("agreement", pGboleData.at(i).agreement);
		QJsonObject cItemObj;
		for (int j = 0; j < pGboleData.at(i).cItem.size(); j++)
		{
			QJsonObject pItemObj;
			pItemObj.insert("CanId", pGboleData.at(i).cItem.at(j).CanId);
			pItemObj.insert("opt", pGboleData.at(i).cItem.at(j).opt);
			QJsonObject pDItem;
			//pItem用一个数组来储存
			for (int k = 0; k < pGboleData.at(i).cItem.at(j).pItem.size(); k++)
			{
				QJsonArray pItem;
				pItem.append(pGboleData.at(i).cItem.at(j).pItem.at(k).bitName);
				pItem.append(pGboleData.at(i).cItem.at(j).pItem.at(k).startByte);
				pItem.append(pGboleData.at(i).cItem.at(j).pItem.at(k).startBit);
				pItem.append(pGboleData.at(i).cItem.at(j).pItem.at(k).bitLeng);
				pItem.append(pGboleData.at(i).cItem.at(j).pItem.at(k).precision);
				pItem.append(pGboleData.at(i).cItem.at(j).pItem.at(k).offset);
				pDItem.insert(QString::number(k), pItem);
			}
			pItemObj.insert("pDItem", pDItem);
			cItemObj.insert(QString::number(j), pItemObj);
		}
		mItem.insert("mDItem", cItemObj);
		mItemObjRoot.insert(QString::number(i), mItem);
	}
	// 将json对象里的数据转换为字符串
	QJsonDocument doc;
	// 将object设置位本文档的主要对象
	doc.setObject(mItemObjRoot);
	appPath += "/appData.json";
	QFile file(appPath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		return;
	}
	QTextStream stream(&file);
	stream.setCodec("UTF-8");
	stream << doc.toJson();
	file.close();
}

void QCanSetting::InitpGboleData()
{
	QString appPath = QApplication::applicationDirPath() + "/Data/appData.json";
	//QFile 可以获取文件是存在
	QFile file(appPath);
	if (!file.exists(appPath))
		return;
	//打开文件，只读模式，如果失败，就return
	if (!file.open(QFile::ReadOnly | QFile::Truncate))
	{
		QMessageBox::warning(this, tr("warning"), tr("数据不存在，初始化数据失败"));
		return;
	}
	//用text流读到内存中
	QTextStream readStrem(&file);
	//设置格式为utf-8
	readStrem.setCodec("UTF-8");
	//读取所有
	QString str = readStrem.readAll();
	//读完之后记得要close，防止程序异常退出，损坏数据
	file.close();
	QJsonParseError jsonError;
	//解析为json文档
	QJsonDocument doc = QJsonDocument::fromJson(str.toUtf8(), &jsonError);
	if (jsonError.error != QJsonParseError::NoError)
	{
		QMessageBox::warning(this, tr("warning"), QString(tr("解析json失败：") + jsonError.errorString()));
		return;
	}
	//转换成json object
	QJsonObject rootObj = doc.object();
	//获取所有key
	QStringList keyList = rootObj.keys();
	//根据key进行遍历
	for (int i = 0; i < keyList.size(); i++)
	{
		protoData ptem;
		//对象就是用{}括起来的
		if (rootObj[keyList.at(i)].type() != QJsonValue::Object)
			continue;
		//转为对象
		QJsonObject rootSecond = rootObj[keyList.at(i)].toObject();
		//把对象下面的几个值取出来
		ptem.agreement = rootSecond["agreement"].toInt(0);
		ptem.modelName = rootSecond["modelName"].toString();
		//如果还有嵌套对象
		if (rootSecond["mDItem"].type() == QJsonValue::Object)
		{
			//把mDItem的Value变为对象，即它是用括号{}括起来的
			QJsonObject rootthird = rootSecond["mDItem"].toObject();
			QStringList keyListThird = rootthird.keys();
			for (int j = 0; j < keyListThird.size(); j++)
			{
				if (rootthird[keyListThird.at(j)].type() != QJsonValue::Object)
					continue;
				//mDitem本身也是对象，对应的数据结构是struct canIdData,std::vector<canIdData>cItem;
				QJsonObject rootfour = rootthird[keyListThird.at(j)].toObject();
				canIdData ctemp;
				ctemp.CanId = rootfour["CanId"].toInt(0);
				ctemp.opt = rootfour["opt"].toInt(0);
				if (rootfour["pDItem"].isObject())
				{
					//pDitem对应的是std::vector<struct protoItem>pItem;
					QJsonObject rootfift = rootfour["pDItem"].toObject();
					QStringList keyListFour = rootfift.keys();
					for (int k = 0; k < keyListFour.size(); k++)
					{
						if (!rootfift[keyListFour.at(k)].isArray())
							continue;
						//对应 struct protoItem
						QJsonArray jarr = rootfift[keyListFour.at(k)].toArray();
						if (jarr.size() < 5)
							continue;
						protoItem dtemp;
						dtemp.bitName	= jarr[0].toString();
						dtemp.startByte = jarr[1].toInt();
						dtemp.startBit	= jarr[2].toInt();
						dtemp.bitLeng	= jarr[3].toInt();
						dtemp.precision = jarr[4].toInt();
						dtemp.offset	= jarr[5].toInt();
						ctemp.pItem.push_back(dtemp);
					}
				}

				ptem.cItem.push_back(ctemp);
				
			}
		}
		pGboleData.push_back(ptem);
	}
	//初始化表格
	SetTableData();
}

void QCanSetting::SetTableData()
{
	if (pGboleData.size() < 0)
		return;
	for (int i = 0; i < pGboleData.size(); ++i)
	{
		
		modelView->setRowCount(i + 1);
		QComboBox* proto = new QComboBox();
		proto->addItem(tr("Intel"));
		proto->addItem(tr("Motorola"));
		connect(proto, SIGNAL(currentIndexChanged(int)), this, SLOT(on_modelView_currentIndexChanged(int)));
		modelView->setItem(i, 0, new QTableWidgetItem(pGboleData.at(i).modelName));
		modelView->setCellWidget(i, 1, proto);
		proto->setCurrentIndex(pGboleData.at(i).agreement);
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
}

void QCanSetting::on_pbSaveModel_clicked()
{
	savepGboleData();
}

void QCanSetting::on_pbAddCanId_clicked()
{
	if (!canIdView)
		return;
	if (!modelView)
		return;
	if (modelView->rowCount() < 1)
		return;
	short int curSelectRow = modelView->currentRow();
	if (curSelectRow<0 || curSelectRow>pGboleData.size() - 1)
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

	canIdView->setItem(row, 0, new QTableWidgetItem(tr("0x123456")));
	canIdView->setCellWidget(row, 1, proto);
	canIdData cdata;
	cdata.CanId = canIdView->item(row,0)->text().toInt(NULL,16);
	cdata.opt = 0;
	pGboleData.at(curSelectRow).cItem.push_back(cdata);
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
}

void QCanSetting::on_pbSaveCanId_clicked()
{
	savepGboleData();
}

void QCanSetting::on_pbAddIteam_clicked()
{
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
	if (curSelectRow<0 || curSelectRow>pGboleData.size() - 1)
	{
		QMessageBox::warning(this, tr("warning"), tr("未选中型号，不能添加，要先选中型号，然后再选中CanID，再添加"));
		return;
	}
	short int curSelectCanRow = canIdView->currentRow();
	if (curSelectCanRow<0 || curSelectCanRow>pGboleData.at(curSelectRow).cItem.size() - 1)
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
	protoItem pItem;
	pItem.bitName	= tableView->item(row, 0)->text();
	pItem.startByte = tableView->item(row, 1)->text().toInt(NULL, 10);
	pItem.startBit	= tableView->item(row, 2)->text().toInt(NULL, 10);
	pItem.bitLeng	= tableView->item(row, 3)->text().toInt(NULL, 10);
	pItem.precision = tableView->item(row, 4)->text().toInt(NULL, 10);
	pItem.offset	= tableView->item(row, 5)->text().toInt(NULL, 10);
	try
	{
		pGboleData.at(curSelectRow).cItem.at(curSelectCanRow).pItem.push_back(pItem);
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

void QCanSetting::on_pbDelIteam_clicked()
{
	if (!tableView)
		return;
	short int index = tableView->currentIndex().row();
	if (index < 0)return;
	tableView->removeRow(index);
}

void QCanSetting::on_pbSaveIteam_clicked()
{
	savepGboleData();
}

void QCanSetting::on_modelView_currentIndexChanged(int index)
{
	//取出发出者的指针
	QComboBox* cbx = qobject_cast<QComboBox*>(sender());
	if (cbx == nullptr)
		return;
	//取按钮所在行列
	QModelIndex idx = modelView->indexAt(QPoint(cbx->frameGeometry().x(), cbx->frameGeometry().y()));
	int row = idx.row();
	if (row<0 || row>pGboleData.size() - 1)
	{
		QMessageBox::warning(this, tr("警告"), tr("数据超出范围"));
		return;
	}
	pGboleData.at(row).agreement = cbx->currentIndex();
}

void QCanSetting::on_modelView_cellChanged(int row, int col)
{
	
	if (row<0 || row>pGboleData.size() - 1||col!=0)
	{
		return;
	}
	pGboleData.at(row).modelName = modelView->item(row,col)->text();
	//关掉这个，防止添加行setData的时候，触发这个信号
	disconnect(modelView, SIGNAL(itemChanged(cellChanged(int, int))), this, SLOT(on_modelView_cellChanged(int, int)));
}

void QCanSetting::on_modelView_doubleClicked(int, int)
{
	//注意QT的这个cellChanged信号，他会当setData的时候触发这个信号
	//所以曲线救国，在双击的时候才连接这个槽，
	connect(modelView, SIGNAL(cellChanged(int, int)), this, SLOT(on_modelView_cellChanged(int, int)));
}

void QCanSetting::on_modelView_Clicked(int row, int col)
{
	if (!canIdView)
		return;
	if (!modelView)
		return;
	if (row < 0)
		return;
	int rowCount = canIdView->rowCount();
	for (int m = 0; m < rowCount; m++)
		canIdView->removeRow(rowCount - m - 1);

	if (row > pGboleData.size() - 1)
		return;
	try
	{
		for (int i = 0; i < pGboleData.at(row).cItem.size(); ++i)
		{
			canIdView->setRowCount(i + 1);
			QComboBox* proto = new QComboBox();
			proto->addItem(tr("接收"));
			proto->addItem(tr("发送"));
			proto->setCurrentIndex(pGboleData.at(row).cItem.at(i).opt);
			canIdView->setItem(i, 0, new QTableWidgetItem(QString("%1").arg(pGboleData.at(row).cItem.at(i).CanId,8,16)));
			canIdView->setCellWidget(i, 1, proto);
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
	QComboBox* cb = dynamic_cast<QComboBox*>(sender());
	if (!cb)
		return;
	if (!modelView) return;
	int mCurRow = modelView->currentRow();
	if (mCurRow < 0 || mCurRow>pGboleData.size() - 1) {
		QMessageBox::warning(this, tr("warning"), tr("未选中型号， 不能修改CanID"));
		return;
	}
	if (!canIdView) return;
	QModelIndex pIndex = canIdView->indexAt(QPoint(cb->frameGeometry().x(), cb->frameGeometry().y()));
	int row = pIndex.row();

	if (row > pGboleData.at(mCurRow).cItem.size() - 1 || row < 0 )
	{
		QMessageBox::warning(this, tr("warning"), tr("修改的位置超出范围"));
		return;
	}
	
	pGboleData.at(mCurRow).cItem.at(row).opt = cb->currentIndex();

}

void QCanSetting::on_canIdView_cellChanged(int row, int col)
{
	if (!modelView) return;
	int mCurRow = modelView->currentRow();
	if (mCurRow < 0 ||mCurRow>pGboleData.size()-1) {
		QMessageBox::warning(this, tr("warning"), tr("未选中型号， 不能修改CanID"));
		return;
	}
	if (!canIdView) return;

	if (row > pGboleData.at(mCurRow).cItem.size() - 1 || row <0||col!=0)
	{
		QMessageBox::warning(this, tr("warning"), tr("修改的位置超出范围"));
		return;
	}
	pGboleData.at(mCurRow).cItem.at(row).CanId = canIdView->item(row,col)->text().toInt(nullptr, 16);

	//关掉这个，防止添加行setData的时候，触发这个信号
	disconnect(canIdView, SIGNAL(itemChanged(cellChanged(int, int))), this, SLOT(on_canIdView_cellChanged(int, int)));
}

void QCanSetting::on_canIdView_doubleClicked(int, int)
{
	//注意QT的这个cellChanged信号，他会当setData的时候触发这个信号
	//所以曲线救国，在双击的时候才连接这个槽，
	connect(canIdView, SIGNAL(cellChanged(int, int)), this, SLOT(on_canIdView_cellChanged(int, int)));
}

void QCanSetting::on_canIdView_Clicked(int row, int col)
{
	if (!tableView)
		return;
	if (!canIdView)
		return;
	if (!modelView)
		return;
	if (row < 0)
		return;

	short mRow = modelView->currentRow();
	if (mRow < 0 || mRow > pGboleData.size() - 1)
		return;
	//清空内容，但不清空头，若要清空头，用clear()
	int rowCount = tableView->rowCount();
	for (int m = 0; m < rowCount; m++)
		tableView->removeRow(rowCount - m - 1);
	int count = pGboleData.at(mRow).cItem.size()-1;
	if (row > count)
		return;
	try
	{
		for (int i = 0; i < pGboleData.at(mRow).cItem.at(row).pItem.size(); i++)
		{
			//short int crow = tableView->rowCount();
			tableView->setRowCount(i + 1);
			tableView->setItem(i, 0, new QTableWidgetItem(pGboleData.at(mRow).cItem.at(row).pItem.at(i).bitName));
			tableView->setItem(i, 1, new QTableWidgetItem(QString::number(pGboleData.at(mRow).cItem.at(row).pItem.at(i).startByte)));
			tableView->setItem(i, 2, new QTableWidgetItem(QString::number(pGboleData.at(mRow).cItem.at(row).pItem.at(i).startBit)));
			tableView->setItem(i, 3, new QTableWidgetItem(QString::number(pGboleData.at(mRow).cItem.at(row).pItem.at(i).bitLeng)));
			tableView->setItem(i, 4, new QTableWidgetItem(QString::number(pGboleData.at(mRow).cItem.at(row).pItem.at(i).precision)));
			tableView->setItem(i, 5, new QTableWidgetItem(QString::number(pGboleData.at(mRow).cItem.at(row).pItem.at(i).offset)));
		}
	}
	catch (const std::exception&e)
	{
		QMessageBox::warning(this, tr("warning"), QString(tr("Vector超出:")+e.what()));
	}
}

void QCanSetting::on_tableView_doubleCLicked(int, int)
{
	connect(tableView, SIGNAL(cellChanged(int, int)), this, SLOT(on_tableView_cellChanged(int, int)));
}

void QCanSetting::on_tableView_cellChanged(int row, int col)
{

	//后面要把这个关掉
	disconnect(tableView, SIGNAL(cellChanged(int, int)), this, SLOT(on_tableView_cellChanged(int, int)));
}

