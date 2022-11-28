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
#include <QProgressDialog.h>
#include "canthread.h"
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
    QProgressDialog* progress = nullptr;
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
private:
    void initUi();
    void initData();
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

    //读取发送的数据
    void getSendDataFromTable();

    void saveAutoTestRes(const QString &fileName,const QStringList& data);

private:
    std::vector<canIdData>recCanData;
    std::vector<canIdData>sendCanData;
    //pcan设备指针
    PCAN *pcan = nullptr;
    PCAN *pcanArr[4] = { nullptr };
    CANThread* canayst= nullptr ;
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
    //多个pcan组合？
    QComboBox* cbIsMutil = nullptr;
    //设备打开
    QPushButton* pbOpen = nullptr;
    //报文周期
    QLineEdit* cycle = nullptr;
    //波特率
    QComboBox* cbBitRate = nullptr;
    //刷新设备
    QPushButton* reFresh = nullptr;
    //型号选择
    QComboBox* cbSelectModel = nullptr;
    //通信状态
    QLabel* communicaLabel = nullptr;
    //定时发送请求
    QTimer* sendTimer = nullptr;
    //通信丢失定时器
    QTimer* lostQTimer = nullptr;
    //多路的通信丢失定时器
    QTimer* lostQTimerArr[4] = { nullptr };
    //CAN打开状态变量
    bool pcanIsOpen = false;
    //日志条数
    int maxTextBrowser = 0;
    //是否打印报文
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
    int lowVolt = 330;      //V,这个电压下开始测低压保护
    int highVolt = 740;     //V,这个电压下开始测高压保护
    int avgPower = 7000;    //平均功率,未用到
    int m_iHeatTempture = -15;                 //°C，进水口这个温度下开始测加热
    int m_iPowerTempture = 0;              //°C，这个温度下的额定功率
    int m_iOverTime = 300;                //秒，超过这个时间，这个测试部分就要跳过了
    int m_iVoltError = 8;                  //电压误差
    int m_iVoltStep = 4;                   //电压步进
    int rmFirstFream = 3;               //测功率时，到达指定条件后延迟帧数
    int agvPowerFream = 60;             //测功率时的平均帧数;
    float realPower[4] = { 0 };
    float realVolt[4] = { 0 };
    float realWTemp[4] = { 0 };
    QString realHVErr[4] = { ""};
    QString realHvLv[4] = { ""};
    QString realOTPro[4] = { ""};
    QString strErrorName = "高压异常";
    QString strOkName = "高压正常";
    std::vector<float>PowerArr[4];
    std::set<QString>Error[4];
    int runStep = -1;
    bool isRecordError = false;

    QTimer* t_GetVer = nullptr;         //
    QPushButton* pbGetVer = nullptr;
    QLabel labelVer1;
    QLabel labelVer2;
    QLabel labelVer3;
    QLabel labelVer4;
    //博世获取版本号命令
    uchar data_ver[8] = { 0x06,0x20,0x22,0x10,0x24,0x09,0x24,0x06 };

    //存储自动化测试结果
    QString phuRes_1 = "";
    QString phuRes_2 = "";
    QString phuRes_3 = "";
    QStringList m1_listTestRes;
    QStringList m2_listTestRes;
    QStringList m3_listTestRes;
    QString m1_strPhuCode;
    QString m2_strPhuCode;
    QString m3_strPhuCode;
   
private slots:
    //CAN协议设置
    void qCanSettingShow();
    //选择当前型号
    void on_CurrentModelChanged(int index);
    //发送
    void on_pbSend_clicked(bool clicked);
    //刷新设备
    void on_pbRefreshDevice_clicked();
    //打开设备
    void on_pbOpenPcan_clicked();
    //双击编辑“发送表格”的内容
    void on_tableDoubleClicked(int,int);
    //表格中的单元格内容发生变化了
    void on_tableClicked(int, int);
    //CAN 数据接收
    void on_ReceiveData(uint frame_id, QByteArray data);
    void on_ReceiveDataMulti(uint frame_id, QByteArray data);
    void on_ReceiveData(int ch,uint frame_id, QByteArray data);
    //CAN协议设置的窗口关闭响应，用来刷新设置的数据
    void on_SettingWidowsClose();
    //“发送表格”中的下拉框操作
    void on_cbSelectSendItemChanged(int);
    //滚动显示
    void on_setInToRollData();
    void on_setInToRollDataMult(int ch);
    //数据监控
    void on_checkTraceChanged(int);
    //保存数据
    void on_pbSaveCanData_clicked();
    //清除数据
    void on_pbClearCanData_clicked();

    void on_checkSendChanged(int);
    //接收超时函数
    void on_recTimeout();
    void on_recTimeoutMutil();
    //自动化工作
    void on_autoWork(bool isRun);
    //自动化工作线程发出的信号的接收函数
    void on_recSigEndRunWork(int n,int channel);
    //debug用的step
    void on_lineEdit_editingFinished();
    //获取版本号（博世专用）
    void on_pbGetVer_clicked(bool);

    void on_pbClearLogShow_clicked();

    void on_cbCanType_currentIndexChanged(int index);
signals:
    void sigNewRoll();
    void sigNewRollMult(int ch);
    void sigEndRunWork(int n,int channel);
public:
    void initLogger();
    void destroyLogger();

public slots:
    void logSlot(const QString& message, int level);
    
};
