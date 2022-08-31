#pragma once
#pragma execution_character_set("utf-8")  
#include <QWidget>
#include "ui_QSetProperty.h"
#include <QTableWidget>
#include "dataStruct.h"
#include <QCloseEvent>

class QSetProperty : public QWidget
{
	Q_OBJECT

public:
	QSetProperty(QWidget *parent = nullptr);
	~QSetProperty();
	//外面传进来的map，用来保存数据
	void setIntoMap(std::map<QString, cellProperty>* cp); 
	virtual void closeEvent(QCloseEvent* event) override;
private:
	Ui::QSetPropertyClass ui;
	void InitUI();
	void ItitData();
	QTableWidget* table=nullptr;
	std::map<QString, cellProperty>*mcp;
private slots:
	void getColor();
	void on_pbAddItem_clicked();
	void on_pbDelItem_clicked();
	void on_pbSaveItem_clicked();
	void on_itemDoubleClicked();
	void on_table_cellChanged(int,int);
};
