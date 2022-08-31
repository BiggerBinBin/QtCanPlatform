/*
 * QtCanPlatform.h - header file of QtCanPlatform class
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

#include <QtWidgets/QMainWindow>
#include "ui_QtCanPlatform.h"
#include <QTableView>
#include <qtextbrowser.h>
#include <iostream>
#include <memory>
#include <QStringList>
#include <QStringListModel>
#include "QCanSetting.h"
#include "PCAN.h"
#include <QTimer>
#include <QPushButton>
#include <QCloseEvent>
#pragma execution_character_set("utf-8")  
class QtCanPlatform : public QMainWindow
{
    Q_OBJECT

public:
    QtCanPlatform(QWidget *parent = nullptr);
    ~QtCanPlatform();
    virtual void closeEvent(QCloseEvent* event) override;
private:
    Ui::QtCanPlatformClass ui;
    void initUi();
    void initData();
    QStringListModel *model=nullptr;

    //UI
    QTableWidget* tableView=nullptr;
    QTableWidget* tableRecView=nullptr;
    QTextBrowser* textBrowser=nullptr;
    QCanSetting* canSetting = nullptr;
   
    int currentModel = -1;
    bool sendDataIntoTab();
    bool recDataIntoTab();
    void sendData();
    bool intelProtocol(canIdData &cdata,uchar data[], unsigned int &fream_id);
    bool motoProtocol(canIdData& cdata,uchar data[], unsigned int& fream_id);
private:
    std::vector<canIdData>recCanData;
    std::vector<canIdData>sendCanData;
    PCAN *pcan = nullptr;
    QPushButton* pbSend = nullptr;
    QComboBox* cbPcan = nullptr;
    QPushButton* pbOpen = nullptr;
    QLineEdit* cycle = nullptr;
    QComboBox* cbBitRate = nullptr;
    QPushButton* reFresh = nullptr;
    QTimer* sendTimer = nullptr;
    bool pcanIsOpen = false;
    std::map<QString, std::vector<parseData>>showTableD;
private slots:
    void qCanSettingShow();
    void on_CurrentModelChanged(int index);
    void on_pbSend_clicked(bool clicked);
    void on_pbRefreshDevice_clicked();
    void on_pbOpenPcan_clicked();
    void on_tableDoubleClicked(int,int);
    void on_tableClicked(int, int);
    void on_ReceiveData(uint frame_id, QByteArray data);
};
