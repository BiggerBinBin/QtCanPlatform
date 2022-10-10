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
#include "DataSave.h"
#include "QDeviceCtrl.h"
#include "kvaser.h"
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
    QTableWidget* tableRollTitle=nullptr;
    QTableWidget* tableRollData=nullptr;
    QTextBrowser* textBrowser=nullptr;
    QCanSetting* canSetting = nullptr;
   
    //control ui
    QDeviceCtrl* dCtrl = nullptr;

    int currentModel = -1;
    bool sendDataIntoTab();
    bool recDataIntoTab();
    void sendData();
    bool intelProtocol(canIdData &cdata,uchar data[], unsigned int &fream_id);
    bool motoProtocol(canIdData& cdata,uchar data[], unsigned int& fream_id);
    void recAnalyseIntel(unsigned int fream_id, QByteArray data);
    void recAnalyseMoto(unsigned int fream_id, QByteArray data);
    void getModelTitle();
    void saveCanData();
    //CRC计算
    unsigned char crc_high_first(uchar data[], unsigned char len);
private:
    std::vector<canIdData>recCanData;
    std::vector<canIdData>sendCanData;
    //pcan设备指针
    PCAN *pcan = nullptr;
    //kvaser设备指针
    kvaser* kcan = nullptr;
    //保存kcan的打开状态
    const int* ckHandle = nullptr;
    //发送按钮
    QPushButton* pbSend = nullptr;
    //can通道选择
    QComboBox* cbPcan = nullptr;
    //can类型选择
    QComboBox* cbCanType = nullptr;
    QPushButton* pbOpen = nullptr;
    QLineEdit* cycle = nullptr;
    QComboBox* cbBitRate = nullptr;
    QPushButton* reFresh = nullptr;
    QComboBox* cbSelectModel = nullptr;
    QLabel* communicaLabel = nullptr;
    QTimer* sendTimer = nullptr;
    QTimer* lostQTimer = nullptr;
    bool pcanIsOpen = false;
    int maxTextBrowser = 0;
    bool isTrace = false;
    std::map<QString, std::vector<parseData>>showTableD;
    QStringList rollTitle;
    QStringListModel* titleModel = nullptr;
    std::vector<RollStruct>RollShowData;
    DataSave *saveData=nullptr;
    QStringList strSaveList;
    QString excelTitle;
    int saveListNum = 600;
    uint16_t lostTimeOut = 3000;
    QColor recBackgroudColor = QColor(0, 120, 215);
    QColor recFontColor = QColor(255, 250, 255);
   
private slots:
    void qCanSettingShow();
    void on_CurrentModelChanged(int index);
    void on_pbSend_clicked(bool clicked);
    void on_pbRefreshDevice_clicked();
    void on_pbOpenPcan_clicked();
    void on_tableDoubleClicked(int,int);
    void on_tableClicked(int, int);
    void on_ReceiveData(uint frame_id, QByteArray data);
    void on_ReceiveData(int ch,uint frame_id, QByteArray data);
    void on_SettingWidowsClose();
    void on_cbSelectSendItemChanged(int);
    void on_setInToRollData();
    void on_checkTraceChanged(int);
    void on_pbSaveCanData_clicked();
    void on_pbClearCanData_clicked();
    void on_checkSendChanged(int);
    void on_recTimeout();
signals:
    void sigNewRoll();
public:
    void initLogger();
    void destroyLogger();

public slots:
    void logSlot(const QString& message, int level);
};
