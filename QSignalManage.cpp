#include "QSignalManage.h"
#include "qGboleData.h"
#include <QTreeWidgetItem>
#include "QsLog.h"
QSignalManage::QSignalManage(protoData* pModel, QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
    pModelCanMes = pModel;
	curModelIndex = -1;
	connect(ui.treeWidget, &QTreeWidget::itemChanged, this, &QSignalManage::on_itemChange_slot);
}

QSignalManage::~QSignalManage()
{
	
}

void QSignalManage::setCurrentModel(int index)
{
	if (curModelIndex == index)
		return;
	curModelIndex = index;
	qGboleData* qGb = qGboleData::getInstance();

	if (index<0 || index>qGb->pGboleData.size() - 1)
	{
		return;
	}	
	(*pModelCanMes) = qGb->pGboleData.at(index);
	for (int i = 0; i < (*pModelCanMes).cItem.size(); i++)
	{
		for (int j = 0; j < (*pModelCanMes).cItem.at(i).pItem.size(); j++)
		{
			(*pModelCanMes).cItem.at(i).pItem.at(j).isRoll = false;
		}
	}
	InitUI();
	
}

void QSignalManage::closeEvent(QCloseEvent* event)
{
	emit QSiganlClose();
}

void QSignalManage::showEvent(QShowEvent* event)
{
}

void QSignalManage::InitUI()
{
	//关掉槽，因为下面的添加Item会触发itemChanged信号
	disconnect(ui.treeWidget, &QTreeWidget::itemChanged, this, &QSignalManage::on_itemChange_slot);
	ui.treeWidget->setHeaderHidden(true);
	//设置展开
	ui.treeWidget->expandAll();
	ui.treeWidget->clear();
	for (int i = 0; i < (*pModelCanMes).cItem.size(); i++)
	{
		QTreeWidgetItem* item = new QTreeWidgetItem(ui.treeWidget);
		item->setText(0, (*pModelCanMes).cItem.at(i).strCanId);
		ui.treeWidget->addTopLevelItem(item);
		item->setCheckState(0, Qt::Unchecked);
		for (int m = 0; m < (*pModelCanMes).cItem.at(i).pItem.size(); m++)
		{
			QTreeWidgetItem* item2 = new QTreeWidgetItem(item);
			item2->setText(0, (*pModelCanMes).cItem.at(i).pItem.at(m).bitName);
			//Add Checkbox
			item2->setCheckState(0, Qt::Unchecked);
		}
	}
	//可以连接槽了
	connect(ui.treeWidget, &QTreeWidget::itemChanged, this, &QSignalManage::on_itemChange_slot);
}
void QSignalManage::on_itemChange_slot(QTreeWidgetItem* item, int column)
{
	if (!item->parent())
	{//it's parent
		//get the index of item
		int itemIndex =  ui.treeWidget->indexOfTopLevelItem(item);
		//disconnect the slot before operating the item
		disconnect(ui.treeWidget, &QTreeWidget::itemChanged, this, &QSignalManage::on_itemChange_slot);
		ushort itemCount = item->childCount();
		for (int m = 0; m < itemCount; m++)
		{
			//set the state of children same the parent;
			item->child(m)->setCheckState(0, item->checkState(0));
		}

		//now you can connecting the slot
		connect(ui.treeWidget, &QTreeWidget::itemChanged, this, &QSignalManage::on_itemChange_slot);

		if (itemIndex >= (*pModelCanMes).cItem.size())
		{
			QLOG_WARN() << "itemIndex overrange";
			return;
		}
		for (int m = 0; m < (*pModelCanMes).cItem.at(itemIndex).pItem.size(); m++)
		{
			//if item state is Unchecked,we set the isRoll flag to false,otherwise to true
			(*pModelCanMes).cItem.at(itemIndex).pItem.at(m).isRoll = (item->checkState(0)==Qt::Unchecked) ? 0 : 1;
		}
		
		
	}
	else
	{//it's son
		
		int itemIndex = ui.treeWidget->indexOfTopLevelItem(item->parent());
		int sonItemIndex =  item->parent()->indexOfChild(item);
		if (itemIndex >= (*pModelCanMes).cItem.size() || sonItemIndex>= (*pModelCanMes).cItem.at(itemIndex).pItem.size())
		{
			QLOG_WARN() << "itemIndex overrange";
			return;
		}
		(*pModelCanMes).cItem.at(itemIndex).pItem.at(sonItemIndex).isRoll = (item->checkState(0) == Qt::Unchecked) ? 0 : 1;

		
	}
}