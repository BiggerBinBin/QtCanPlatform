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
	void setIntoMap(std::vector<cellProperty>* cp);
	//窗口关闭事件，用来清空表格的内容，以免下次打开窗口时还是之前的内容
	virtual void closeEvent(QCloseEvent* event) override;
private:
	Ui::QSetPropertyClass ui;
	void InitUI();
	void ItitData();
	QTableWidget* table=nullptr;
	//std::map<QString, cellProperty>*mcp;
	//指针传递，用来修改父窗口的属性，所以外部调用本类时，必须先调用setIntoMap把指针传进来
	std::vector<cellProperty>* stl_mcp;
private slots:
	//关闭QT颜色面板之后的方法。用来获取用户选取的颜色
	void getColor();
	//添加属性槽函数
	void on_pbAddItem_clicked();
	//删除属性槽函数
	void on_pbDelItem_clicked();
	void on_pbSaveItem_clicked();
	//双击单元格的槽函数
	void on_itemDoubleClicked(int,int);
	//单元格修改后的槽函数
	void on_table_cellChanged(int,int);
};
