#pragma once

#include <QWidget>
#include "ui_QSignalManage.h"
#include "dataStruct.h"
class QSignalManage : public QWidget
{
	Q_OBJECT

public:
	QSignalManage(protoData* pModel,QWidget *parent = nullptr);
	~QSignalManage();
	void setCurrentModel(int index);
	void virtual closeEvent(QCloseEvent* event) override;
	virtual void showEvent(QShowEvent* event);
private:
	Ui::QSignalManageClass ui;
	void InitUI();
private:
	//std::vector<canIdData>recCanData;
	protoData *pModelCanMes = nullptr;
	enum TREE_TYPE
	{
		ROOT=0,
		CHILD=1
	};
	int curModelIndex;
signals:
	void QSiganlClose();
private slots:
	void on_itemChange_slot(QTreeWidgetItem* item, int column);

};
