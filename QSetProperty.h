#pragma once
#pragma execution_character_set("utf-8")  
#include <QWidget>
#include "ui_QSetProperty.h"
#include <QTableWidget>
#include "dataStruct.h"

class QSetProperty : public QWidget
{
	Q_OBJECT

public:
	QSetProperty(QWidget *parent = nullptr);
	~QSetProperty();
	void setIntoMap(std::map<QString, cellProperty>*cp) { mcp = cp; }
private:
	Ui::QSetPropertyClass ui;
	void InitUI();
	QTableWidget* table=nullptr;
	std::map<QString, cellProperty>*mcp;
private slots:
	void getColor();
	void on_pbAddItem_clicked();
	void on_pbDelItem_clicked();
	void on_pbSaveItem_clicked();
};
