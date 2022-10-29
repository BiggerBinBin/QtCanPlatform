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
#include <QtConcurrent>
#include <set>
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
    //显示发送UI的表格
    QTableWidget* tableView=nullptr;
    //显示接收数据UI的表格
    QTableWidget* tableRecView=nullptr;
    //四通道，需要四个显示tablewidget
    QTableWidget* tableArray[4];
    QTableWidget* tableRollTitle=nullptr;
    QTableWidget* tableRollData=nullptr;
    QTableWidget* tableRollTitleArray[4];
    QTableWidget* tableRollDataArray[4];
    QTabWidget* tabRollBox = nullptr;
    QTextBrowser* textBrowser=nullptr;
    QCanSetting* canSetting = nullptr;
    //显示接收到的数据的tab界面
    QTabWidget* tabRecBox = nullptr;
    //容器，放4个QTableWidget
    QWidget* w[4];
    QWidget* w_rolling[4];
    //control ui
    QDeviceCtrl* dCtrl = nullptr;

    std::atomic_bool _bWork = false;
    std::atomic_int _iSetp = 0;

    int currentModel = -1;
    bool sendDataIntoTab();
    bool recDataIntoTab();
    void sendData();
    //数据解析
    bool intelProtocol(canIdData &cdata,uchar data[], unsigned int &fream_id);
    bool motoProtocol(canIdData& cdata,uchar data[], unsigned int& fream_id);

    //接收到的数据解析(单通道)
    void recAnalyseIntel(unsigned int fream_id, QByteArray data);
    void recAnalyseMoto(unsigned int fream_id, QByteArray data);
    //kvaser多通道
    void recAnalyseIntel(int ch,unsigned int fream_id, QByteArray data);
    void recAnalyseMoto(int ch,unsigned int fream_id, QByteArray data);

    //未使用
    void getModelTitle();
    //数据保存
    void saveCanData();
    void saveCanDataMult();
    //CRC计算
    unsigned char crc_high_first(uchar data[], unsigned char len);

    //读取配置文件 
    void readSetFile();
    //配置
    void configSetFile();
    //自动测试工作流程函数
    void workRun();

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
    DataSave* saveDataArr[4] = { nullptr };
    //单个通道时的保存变量
    QStringList strSaveList;
    //多通道时的数据结构
    std::map<unsigned int, QStringList>multReceData;
    //保存excel的表头
    QString excelTitle;
    //自动保存的数据条数，即达到这个数量就会自动保存
    int saveListNum = 600;
    uint16_t lostTimeOut = 3000;
    QColor recBackgroudColor = QColor(0, 120, 215);
    QColor recFontColor = QColor(255, 250, 255);
    QDateTime lastTime;

    //自动判断的条件与判断值
    int lowVolt = 350;      //低压
    int highVolt = 750;     //高压
    int avgPower = 7000;    //平均功率
    int shortIndex = 7;     //短路在报文数据的下标
    QString voltId = "0x0C004400";      //电压是在哪个报文
    QString powerId = "0x0C004400";     //功率是在哪个报文
    QString shortId = "0x0C004400";     //短路是在哪个报文
    int tempture = -15;                 //进水口温度
    int rmFirstFream = 3;               //测功率时，到达指定条件后延迟帧数
    int agvPowerFream = 60;             //测功率时的平均帧数;
    float realPower[4] = { 0 };
    float realVolt[4] = { 0 };
    float realWTemp[4] = { 0 };
    QString realHvLv[4] = { ""};
    std::vector<float>PowerArr;
    std::set<QString>Error[4];

    bool isRecordError = false;

   
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
    void on_setInToRollDataMult(int ch);
    void on_checkTraceChanged(int);
    void on_pbSaveCanData_clicked();
    void on_pbClearCanData_clicked();
    void on_checkSendChanged(int);
    void on_recTimeout();
    void on_autoWork(bool isRun);
    void on_recSigEndRunWork(int n);
signals:
    void sigNewRoll();
    void sigNewRollMult(int ch);
    void sigEndRunWork(int n);
public:
    void initLogger();
    void destroyLogger();

public slots:
    void logSlot(const QString& message, int level);
    
};
