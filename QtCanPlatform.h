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
#include <qset.h>
#include <QProgressDialog.h>
#include "canthread.h"
#include "mHttp.h"
#include "HardWarePlin.h"
#include "HardWareTcan.h"
#include "AutoDeviceManage.h"
#include "QLogPlot.h"
#include <qpointer.h>
#include <qreadwritelock.h>
#include <QMetaType>
#include "Login.h"
#include "HardWareTlin.h"
#include "SerialportManage.h"
#pragma execution_character_set("utf-8")  

struct UpMesData {
    QString m_strRatedTemp;             //测额定功率冷却液的温度
    QString m_strRatedVoltage;          //测额定功率高压电源的电压
    QString m_strRatedFlow;             //测额定功率冷却液的流量
    QString m_strBundrate;              //CAN通信的波特率
    QString m_strLowVoltage;            //低压的电压
    QString m_strVer;                   //软件版本
    QString m_strLowProtected;          //欠压保护电压
    QString m_strLowProtectedRe;        //欠压故障恢复电压
    QString m_strHighProtected;         //过压保护电压
    QString m_strHighProtectedRe;       //过压故障恢复电压
    QString m_strRatedPW;               //测出来的额定功率
    QString m_strOverTempProtected;     //过温保护
    QString m_strOverTempProtectedRe;   //过温保护恢复
    QString m_strOtherFault;            //其它错误
    QString m_strTestResult;            //测试结果
};

class CanTestPlatform : public QMainWindow
{
    Q_OBJECT
   
public:
    
    CanTestPlatform(QWidget *parent = nullptr);
    ~CanTestPlatform();
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
    ushort m_usRoll;
private:
    void initUi();
    void initData();
    void initAutoResTableWidget();
    bool sendDataIntoTab();
    bool sendDataIntoTab_up();
    bool recDataIntoTab();
    void sendData();
    //数据解析
    bool intelProtocol(canIdData &cdata,uchar data[], unsigned int &fream_id);
    bool motoProtocol(canIdData& cdata,uchar data[], unsigned int& fream_id);

    //接收到的数据解析(单通道)
    void recAnalyseIntel(unsigned int fream_id, QByteArray data);
    void recAnalyseMoto(unsigned int fream_id, QByteArray data);
    void recAnalyseMotoLSB(unsigned int fream_id, QByteArray data);
    //kvaser多通道
    void recAnalyseIntel(int ch,unsigned int fream_id, QByteArray data);
    void recAnalyseMoto(int ch,unsigned int fream_id, QByteArray data);
    void recAnalyseMotoLSB(int ch,unsigned int fream_id, QByteArray data);

    void getByteInfo(const std::vector<parseData>& parse,int ch);
    void setPowerSupply(const AutoTestStruct& at, int op);
    void setHeatint(const AutoTestStruct& at, float value);
    void setHeatint(const AutoTestStruct& at, float value,int tempture);
    void setCancelHeatint(const AutoTestStruct& at, float value);
    void getAveragePW(const AutoTestStruct& at);
  
    //未使用
    void getModelTitle();
    //数据保存
    void saveCanData();
    void saveCanDataMult();
    //CRC计算
    unsigned char crc_high_first(const uchar data[], unsigned char len);
    unsigned char checksumXOR(const uchar data[]);

    //读取配置文件 
    void readSetFile();
    void saveSetFile();
    //配置
    void configSetFile();
    //自动测试工作流程函数
    //void workRun();
    void workAutoTest();

    //读取发送的数据
    void getSendDataFromTable();

    //void saveAutoTestRes(const QString &fileName,const QStringList& data);
    void showAutoTestStep(int n, QString data, QString remake);
    void getVerAuto(const AutoTestStruct& at);
    QStringList ucharToBinay(const uchar data[8]);

    void on_pbMESConnect(bool);
    void on_TCPStateChanged(QAbstractSocket::SocketState state);
    void on_TCPDataReadyRec();
    void on_TCPDataSend_MES(QByteArray data);
    void runPostMes(QString str);
    bool getMesResponed(QString str);
    float getPowerResponed(QString str);
    QString parserMESStr(QString str);
    bool upMesOutData();
    void runOutMonitor();

private:
    std::vector<canIdData>recCanData;
    
    std::vector<canIdData>sendCanData;
    std::vector<int>HashArr;
    //pcan设备指针
    PCAN *pcan = nullptr;
    //多通道pcan设备指针
    PCAN *pcanArr[4] = { nullptr };
  
    
    //canayst设备指针
    CANThread* canayst= nullptr ;
    //kvaser设备指针
    kvaser* kcan = nullptr;
    //保存kcan的打开状态
    const int* ckHandle = nullptr;

    //PLIN硬件接口，未来会扩充到其它硬件，以实现多态
    HardWareBase* pHardWare = nullptr;
    HardWareTcan* pHardTcan = nullptr;
    HardWareTlin* pHardTlin = nullptr;
    QLogPlot* qlogp = nullptr;

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

    QPushButton* LogPb;
    //CAN打开状态变量
    bool pcanIsOpen = false;
    //日志条数
    int maxTextBrowser = 0;
    //是否打印报文
    bool isTrace = false;

    bool bStandard = false;
    //PTC错误，用于自动化测试
    QSet<QString>multErr;
    //std::map<QString, std::vector<parseData>>showTableD;
    //保存当前接到的数据
    std::vector<showTableData>showTableVec;
    QStringList rollTitle;
    QStringListModel* titleModel = nullptr;
    //滚动显示的数据
    std::vector<RollStruct>RollShowData;
    DataSave *saveData=nullptr;
    //多路保存
    DataSave* saveDataArr[4] = { nullptr };
    //单个通道时的保存变量
    QStringList strSaveList;
    //多通道时的数据结构
    std::map<unsigned int, QStringList>multReceData;
    //保存excel的表头
    QString excelTitle;
    
    uint16_t lostTimeOut = 8000;
    QColor recBackgroudColor = QColor(0, 120, 215);
    QColor recFontColor = QColor(255, 250, 255);
    QDateTime lastTime;

    //自动判断的条件与判断值
    //int lowVolt = 330;      //V,这个电压下开始测低压保护
    //int highVolt = 740;     //V,这个电压下开始测高压保护
    //int avgPower = 7000;    //平均功率,未用到
    //int m_iHeatTempture = -15;                 //°C，进水口这个温度下开始测加热
    //int m_iPowerTempture = 0;              //°C，这个温度下的额定功率
    //int m_iOverTime = 300;                //秒，超过这个时间，这个测试部分就要跳过了
    //int m_iVoltError = 8;                //电压误差
    //int m_iVoltStep = 4;                 //电压步进
    //int rmFirstFream = 3;               //测功率时，到达指定条件后延迟帧数
    int agvPowerFream = 10;             //测功率时的平均帧数;
    //int m_iPowerVolt = 600;
    int bundRate = 250;
    QString m_sInWebAddr = "";
    QString m_sOutWebAddr = "";
    float realPower[4] = { 0 };
    float realVolt[4] = { 0 };
    float realWTemp[4] = { 0 };
    float realINTemp[4] = { 0 };
    QString realHVErr[4] = { ""};
    QString realHvLv[4] = { ""};
    QString realOTPro[4] = { ""};
    QString realOVTemp[4] = { ""};
    QString strErrorName = "高压异常";
    QString strOkName = "高压正常";
    QString strBundRate[8] = { "200","250","500","800","2400","9600","10400","19200"};
    std::vector<float>PowerArr[4];
    QSet<QString>Error[4];
    int runStep = -1;
    bool isRecordError = false;
    int _period_ = 1000;
    QStringList testItemList;
    QCheckBox* m_cbOutTempMonitor;
    QPushButton* pbStartAutoTest;
    QPushButton* pbDevicesManage;
    QPushButton* pbGeneralParameter;
    QLineEdit* lineEditCodeIn;
    //提交入站申请
    QPushButton* pbSummitCode;
    //自动测试显示表格
    QTableWidget* tableAutoResults;
    //设备管理类
    AutoDeviceManage* autoDevMan;

    //QLine
    QLineEdit* mBlowAirLineedit;
    QPushButton* mBlowAirPb;
    QTimer *mBlowAirTimer=nullptr;

    int m_bCommunication = 0;
    //当前测试型号的参数
    struct protoData currentTestModel;
    float m_fFlowCool=0;
    //获取版本号开关
    bool m_bGetVer;
    bool m_bParseVer;
    uchar m_arrVerCanMessage[8];
    QStringList m_strVerCanMessage;
    QReadWriteLock rw_Lock;
    QReadWriteLock lock;
    bool m_bResetfault;             //插入错误前清理已有的错误
    bool CodeOk = false;
    bool m_bMesOK = false;
    //bool m_bPowerOK = false;
    QLineEdit* LineEdit_IPAddr_Mes = nullptr;
    QLineEdit* LineEdit_Port_Mes = nullptr;
    QPushButton* pbConnectPLC_Mes = nullptr;

    QTcpSocket* postMes = nullptr;
    QMutex m_Mutex_Mes;
    QMutex m_Mutex_Mes_Pop;
    QMutex m_Mutex_Pow;
    QMutex m_Mutex_Pow_Pop;
    QQueue<QString>m_MESData;
    QQueue<QString>m_PowerData;
    std::atomic_bool _bIsRec_Mes = false;
    std::atomic_bool _bIsRec_Pow = false;
    QString m_strPHUCode;
    UpMesData up_mes_var;
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

    //右上角操作区显示类型
    //0：显示风暧操作
    //1：显示自动化测试操作
    int m_iShowType = 0;
    //打开设备接收数据
    //0：打开设备就接收
    //1：点击发送才接收
    int m_iRecOnNoSend = 0;
    //每次保存的数据条数
    //自动保存的数据条数，即达到这个数量就会自动保存
    int saveListNum = 600;
    int m_iSaveDataGrap = 1;
    QTimer m_tSaveDataGrapTime;

    //风暧显示时间
    QLineEdit* m_iTimeStopLineEdit = nullptr;
    QPushButton* m_pbStartRad = nullptr;
    QLabel* m_labShowTime = nullptr;
    QTimer* m_tCaptureTimer = nullptr;
    int runTimed = 0;
    //*************记录使能的时间******************
    QLabel* curruntime;
    QLabel* totaltime;
    QPushButton* clearCountTime;
    QCheckBox* isEnableCountTime;
    uint m_uiElspseTotalTime = 0;
    uint m_uiElspseTime = 0;
    QTimer* m_tRecord;
    //运行时间
    int countRunTime = 0;
    uint timeStmp = 0;
    uint timeStmp_send = 0;
    //登录界面
    Login* canSeting;
    bool isLogin;
    int loginType = 0;
    //多个发送周期标识，>1即为多个发送周期,适配如果科技发送两个ID请求时
    //不同的ID的发送周期不同
    int multiCircle;
    QCheckBox* m_cbSavePeriod;
    QLineEdit* m_lePeroid;
    uint m_iSavePeroidNum;
    uint countPeroid;
    QGroupBox* gp;
    //是否显示自动测试界面
    bool m_bShowAutoTest;
    //使能时间
    //uint m_iEnableCount;

    SerialportManage* serialport = nullptr;
    QByteArray OpenLowPwSerial;     //低压电压继电器开启，即断开（常开）
    QByteArray CloseLowPwSerial;    //低压电源继电器关闭，即导通
    QByteArray OpenPureSerial;      //纯水机继电器开（常闭）
    QByteArray ClosePureSerial;     //纯水机继电器关
    int m_iBlowAirTime = 120000;    //吹气时间(ms)
    int m_iWashTime = 120000;       //纯水清洗时间(ms)
    int m_iBackWaterTime = 120000;       //纯水清洗时间(ms)
    std::set<QString>NoVersionSet{"联合卡车"};
    bool isCANTestTempetrue = false;

private slots:
      void on_CapturePower();
      void on_CaptureElapseTime();
      void on_canSettingShowFromLogin(int n);
private slots:
    //CAN协议设置
    void qCanSettingShow();
    //选择当前型号
    void on_CurrentModelChanged(int index);
    //选择当前平台
    void on_CurrentPlatformChanged(int index);
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
    //CAN/LIN 数据接收
    void on_ReceiveData(uint frame_id, QByteArray data);
    void on_ReceiveDataLIN(uint frame_id, QByteArray data,int reserve);
    void on_ReceiveDataMulti(uint frame_id, QByteArray data);
    void on_ReceiveData(int ch,uint frame_id, QByteArray data);
    //void on_ReceiveData2(ushort ch,uint frame_id, uchar data[8]);
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
    //void on_autoWork(bool isRun);
    //自动化工作线程发出的信号的接收函数
    //void on_recSigEndRunWork(int n,int channel);
    //debug用的step
    void on_lineEdit_editingFinished();
    //获取版本号（博世专用）
    void on_pbGetVer_clicked(bool);

    void on_pbClearLogShow_clicked();
    //CAN盒子的类型
    void on_cbCanType_currentIndexChanged(int index);
    //void on_commitData(const QByteArray& byte, int id);

    void on_action_About_triggered();
    void on_action_History_triggered();

    void on_pbLogReplay_clicked();
    
    //***************新版测试*****************//
    //开启自动测试
    void on_pbStartAutoTest_clicked(bool b);
    //设备管理
    void on_pbDevicesManage_clicked();
    //通用参数
    void on_pbGeneralParameter_clicked();
    //处理测试线程的一些界面需求
    void on_processAutoTestSignal(int n, QString str);
    void on_lineEditCodeIn_editingFinished();
    void on_pbSummitCode_clicked();
    void on_SendMesState(int n, QString str);
    void on_RecFlowCool(float flow);

    void on_sigFromPowerNewData(QString data);
    //Mes回报的槽函数
    void on_sigFroMesNewData(QString data);
    //电源返回槽函数
    void on_sigFromThisPowerSet(QString data);
    //数据保存周期开关
    void on_savePeriodCheck_Changed(int n);
    //出口温度监控monitor
    void on_outTempMonitor_Changed(int n);
    void on_plotWindowCLose();
    void on_pbHidenAutoWidget_clicked(bool b);

    void on_pbBlowAir_clicked(bool);
    void on_BlowAir_Stop();
    void on_actionChinese_triggered(bool b);
    void on_actionEnglish_triggered(bool b);
    void on_actionAboutQT_triggered(bool b);

Q_SIGNALS:
    void sigNewRoll();
    void sigSendHttp(QByteArray byte, int id);
    void sigNewRollMult(int ch);
    void sigEndRunWork(int n,int channel);
    void sigNewMessageToGraph(unsigned int fream_id, QByteArray byte,int dx);
    void sigAutoTestSend(int n, QString str);
    void sigSendPutMesData(QByteArray data);
    void sigSendPutPowData(QString data);
    void sigSendMesState(int n, QString str);
    void actionChinese_triggered(bool check);
    void actionEnglish_triggered(bool check);
public:
    void initLogger();
    void destroyLogger();

public slots:
    void logSlot(const QString& message, int level);
    void changeEvent(QEvent* e) override;
    
};
