/*
 * QCanSetting.h - header file of QCanSetting class
 *
 * Copyright (C) 2021-2022 lyb <liyangbinbin@foxmail.com>
 *
 * This file is part of the QtCanPlatform.
 *
 * QtCanPlatform is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * QtCanPlatform is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libQtShadowsocks; see the file LICENSE. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <QWidget>
#include "ui_QCanSetting.h"
#include <QTableView>
#include <qtablewidget.h>
#include <QStandardItemModel>
#pragma execution_character_set("utf-8")  
#include "dataStruct.h"
#include "QSetProperty.h"
#include <QCloseEvent>

class QCanSetting : public QWidget
{
	Q_OBJECT

public:
	QCanSetting(QWidget *parent = nullptr);
	~QCanSetting();
	virtual void closeEvent(QCloseEvent* event) override;
private:
	Ui::QCanSettingClass ui;
	void InitUi();
	QTableWidget* tableView = nullptr;
	//型号table
	QTableWidget* modelView = nullptr;
	//can id table
	QTableWidget* canIdView = nullptr;
	//std::vector<struct protoData>pGboleData;
	void savepGboleData();
	void InitpGboleData();
	void SetTableData();
	std::map<QString, cellProperty>ItemProperty;
	QSetProperty *pp=nullptr;
	
signals:
	void settingWidowsClose();
private slots:
	//添加型号
	void on_pbAddModel_clicked();
	//上移
	void on_pbMoveUpModel_clicked();
	//下移
	void on_pbMoveDownModel_clicked();
	//删除
	void on_pbDelModel_clicked();
	//保存
	void on_pbSaveModel_clicked();
	//复制
	void on_pbCopyModel_clicked();
	
	//添加CanId
	void on_pbAddCanId_clicked();
	void on_pbMoveUpCanId_clicked();
	void on_pbMoveDownCanId_clicked();
	void on_pbDelCanId_clicked();
	void on_pbSaveCanId_clicked();

	//添加解析数据
	void on_pbAddIteam_clicked();
	void on_pbMoveUpIteam_clicked();
	void on_pbMoveDownIteam_clicked();
	void on_pbDelIteam_clicked();
	void on_pbSaveIteam_clicked();

	//下面型号表格按钮响应槽函数
	void on_modelView_currentIndexChanged(int index);
	void on_modelView_bundIndexChanged(int index);
	//void on_modelView_platFormIndexChanged(int index);
	void on_modelView_cellChanged(int row, int col);
	void on_modelView_doubleClicked(int, int);
	void on_modelView_Clicked(int, int);
	void on_modelView_cbStandar(int bStandard);

	//下面是CanID表格响应槽函数
	void on_canIdView_currentIndexChanged(int index);
	void on_canIdView_cellChanged(int row, int col);
	void on_canIdView_doubleClicked(int, int);
	void on_canIdView_Clicked(int, int);
	void on_canIdView_SendChanged(int);

	//字段表格响应槽函数
	void on_tableView_doubleCLicked(int, int);
	void on_tableView_cellChanged(int row, int col);

	void on_property_clicked();
	void on_CheckStateChanged(int);
	void on_isHexCheckStateChanged(int);

};
