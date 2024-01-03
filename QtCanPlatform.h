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
    QString m_strRatedTemp;             //��������ȴҺ���¶�
    QString m_strRatedVoltage;          //�����ʸ�ѹ��Դ�ĵ�ѹ
    QString m_strRatedFlow;             //��������ȴҺ������
    QString m_strBundrate;              //CANͨ�ŵĲ�����
    QString m_strLowVoltage;            //��ѹ�ĵ�ѹ
    QString m_strVer;                   //����汾
    QString m_strLowProtected;          //Ƿѹ������ѹ
    QString m_strLowProtectedRe;        //Ƿѹ���ϻָ���ѹ
    QString m_strHighProtected;         //��ѹ������ѹ
    QString m_strHighProtectedRe;       //��ѹ���ϻָ���ѹ
    QString m_strRatedPW;               //������Ķ����
    QString m_strOverTempProtected;     //���±���
    QString m_strOverTempProtectedRe;   //���±����ָ�
    QString m_strOtherFault;            //��������
    QString m_strTestResult;            //���Խ��
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
    //��ʾ����UI�ı��
    QTableWidget* tableView=nullptr;
    //��ʾ��������UI�ı��
    QTableWidget* tableRecView=nullptr;
    //��ͨ������Ҫ�ĸ���ʾtablewidget
    QTableWidget* tableArray[4];
    QTableWidget* tableRollTitle=nullptr;
    QTableWidget* tableRollData=nullptr;
    QTableWidget* tableRollTitleArray[4];
    QTableWidget* tableRollDataArray[4];
    QTabWidget* tabRollBox = nullptr;
    QTextBrowser* textBrowser=nullptr;
    QCanSetting* canSetting = nullptr;
    QProgressDialog* progress = nullptr;
    //��ʾ���յ������ݵ�tab����
    QTabWidget* tabRecBox = nullptr;
    //��������4��QTableWidget
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
    //���ݽ���
    bool intelProtocol(canIdData &cdata,uchar data[], unsigned int &fream_id);
    bool motoProtocol(canIdData& cdata,uchar data[], unsigned int& fream_id);

    //���յ������ݽ���(��ͨ��)
    void recAnalyseIntel(unsigned int fream_id, QByteArray data);
    void recAnalyseMoto(unsigned int fream_id, QByteArray data);
    void recAnalyseMotoLSB(unsigned int fream_id, QByteArray data);
    //kvaser��ͨ��
    void recAnalyseIntel(int ch,unsigned int fream_id, QByteArray data);
    void recAnalyseMoto(int ch,unsigned int fream_id, QByteArray data);
    void recAnalyseMotoLSB(int ch,unsigned int fream_id, QByteArray data);

    void getByteInfo(const std::vector<parseData>& parse,int ch);
    void setPowerSupply(const AutoTestStruct& at, int op);
    void setHeatint(const AutoTestStruct& at, float value);
    void setHeatint(const AutoTestStruct& at, float value,int tempture);
    void setCancelHeatint(const AutoTestStruct& at, float value);
    void getAveragePW(const AutoTestStruct& at);
  
    //δʹ��
    void getModelTitle();
    //���ݱ���
    void saveCanData();
    void saveCanDataMult();
    //CRC����
    unsigned char crc_high_first(const uchar data[], unsigned char len);
    unsigned char checksumXOR(const uchar data[]);

    //��ȡ�����ļ� 
    void readSetFile();
    void saveSetFile();
    //����
    void configSetFile();
    //�Զ����Թ������̺���
    //void workRun();
    void workAutoTest();

    //��ȡ���͵�����
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
    //pcan�豸ָ��
    PCAN *pcan = nullptr;
    //��ͨ��pcan�豸ָ��
    PCAN *pcanArr[4] = { nullptr };
  
    
    //canayst�豸ָ��
    CANThread* canayst= nullptr ;
    //kvaser�豸ָ��
    kvaser* kcan = nullptr;
    //����kcan�Ĵ�״̬
    const int* ckHandle = nullptr;

    //PLINӲ���ӿڣ�δ�������䵽����Ӳ������ʵ�ֶ�̬
    HardWareBase* pHardWare = nullptr;
    HardWareTcan* pHardTcan = nullptr;
    HardWareTlin* pHardTlin = nullptr;
    QLogPlot* qlogp = nullptr;

    //���Ͱ�ť
    QPushButton* pbSend = nullptr;
    //canͨ��ѡ��
    QComboBox* cbPcan = nullptr;
    //can����ѡ��
    QComboBox* cbCanType = nullptr;
    //���pcan��ϣ�
    QComboBox* cbIsMutil = nullptr;
    //�豸��
    QPushButton* pbOpen = nullptr;
    //��������
    QLineEdit* cycle = nullptr;
    //������
    QComboBox* cbBitRate = nullptr;
    //ˢ���豸
    QPushButton* reFresh = nullptr;
    //�ͺ�ѡ��
    QComboBox* cbSelectModel = nullptr;
    //ͨ��״̬
    QLabel* communicaLabel = nullptr;
    //��ʱ��������
    QTimer* sendTimer = nullptr;
    //ͨ�Ŷ�ʧ��ʱ��
    QTimer* lostQTimer = nullptr;

    //��·��ͨ�Ŷ�ʧ��ʱ��
    QTimer* lostQTimerArr[4] = { nullptr };

    QPushButton* LogPb;
    //CAN��״̬����
    bool pcanIsOpen = false;
    //��־����
    int maxTextBrowser = 0;
    //�Ƿ��ӡ����
    bool isTrace = false;

    bool bStandard = false;
    //PTC���������Զ�������
    QSet<QString>multErr;
    //std::map<QString, std::vector<parseData>>showTableD;
    //���浱ǰ�ӵ�������
    std::vector<showTableData>showTableVec;
    QStringList rollTitle;
    QStringListModel* titleModel = nullptr;
    //������ʾ������
    std::vector<RollStruct>RollShowData;
    DataSave *saveData=nullptr;
    //��·����
    DataSave* saveDataArr[4] = { nullptr };
    //����ͨ��ʱ�ı������
    QStringList strSaveList;
    //��ͨ��ʱ�����ݽṹ
    std::map<unsigned int, QStringList>multReceData;
    //����excel�ı�ͷ
    QString excelTitle;
    
    uint16_t lostTimeOut = 8000;
    QColor recBackgroudColor = QColor(0, 120, 215);
    QColor recFontColor = QColor(255, 250, 255);
    QDateTime lastTime;

    //�Զ��жϵ��������ж�ֵ
    //int lowVolt = 330;      //V,�����ѹ�¿�ʼ���ѹ����
    //int highVolt = 740;     //V,�����ѹ�¿�ʼ���ѹ����
    //int avgPower = 7000;    //ƽ������,δ�õ�
    //int m_iHeatTempture = -15;                 //��C����ˮ������¶��¿�ʼ�����
    //int m_iPowerTempture = 0;              //��C������¶��µĶ����
    //int m_iOverTime = 300;                //�룬�������ʱ�䣬������Բ��־�Ҫ������
    //int m_iVoltError = 8;                //��ѹ���
    //int m_iVoltStep = 4;                 //��ѹ����
    //int rmFirstFream = 3;               //�⹦��ʱ������ָ���������ӳ�֡��
    int agvPowerFream = 10;             //�⹦��ʱ��ƽ��֡��;
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
    QString strErrorName = "��ѹ�쳣";
    QString strOkName = "��ѹ����";
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
    //�ύ��վ����
    QPushButton* pbSummitCode;
    //�Զ�������ʾ���
    QTableWidget* tableAutoResults;
    //�豸������
    AutoDeviceManage* autoDevMan;

    //QLine
    QLineEdit* mBlowAirLineedit;
    QPushButton* mBlowAirPb;
    QTimer *mBlowAirTimer=nullptr;

    int m_bCommunication = 0;
    //��ǰ�����ͺŵĲ���
    struct protoData currentTestModel;
    float m_fFlowCool=0;
    //��ȡ�汾�ſ���
    bool m_bGetVer;
    bool m_bParseVer;
    uchar m_arrVerCanMessage[8];
    QStringList m_strVerCanMessage;
    QReadWriteLock rw_Lock;
    QReadWriteLock lock;
    bool m_bResetfault;             //�������ǰ�������еĴ���
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
    //������ȡ�汾������
    uchar data_ver[8] = { 0x06,0x20,0x22,0x10,0x24,0x09,0x24,0x06 };

    //�洢�Զ������Խ��
    QString phuRes_1 = "";
    QString phuRes_2 = "";
    QString phuRes_3 = "";
    QStringList m1_listTestRes;
    QStringList m2_listTestRes;
    QStringList m3_listTestRes;
    QString m1_strPhuCode;
    QString m2_strPhuCode;
    QString m3_strPhuCode;

    //���Ͻǲ�������ʾ����
    //0����ʾ���Ӳ���
    //1����ʾ�Զ������Բ���
    int m_iShowType = 0;
    //���豸��������
    //0�����豸�ͽ���
    //1��������ͲŽ���
    int m_iRecOnNoSend = 0;
    //ÿ�α������������
    //�Զ�������������������ﵽ��������ͻ��Զ�����
    int saveListNum = 600;
    int m_iSaveDataGrap = 1;
    QTimer m_tSaveDataGrapTime;

    //������ʾʱ��
    QLineEdit* m_iTimeStopLineEdit = nullptr;
    QPushButton* m_pbStartRad = nullptr;
    QLabel* m_labShowTime = nullptr;
    QTimer* m_tCaptureTimer = nullptr;
    int runTimed = 0;
    //*************��¼ʹ�ܵ�ʱ��******************
    QLabel* curruntime;
    QLabel* totaltime;
    QPushButton* clearCountTime;
    QCheckBox* isEnableCountTime;
    uint m_uiElspseTotalTime = 0;
    uint m_uiElspseTime = 0;
    QTimer* m_tRecord;
    //����ʱ��
    int countRunTime = 0;
    uint timeStmp = 0;
    uint timeStmp_send = 0;
    //��¼����
    Login* canSeting;
    bool isLogin;
    int loginType = 0;
    //����������ڱ�ʶ��>1��Ϊ�����������,��������Ƽ���������ID����ʱ
    //��ͬ��ID�ķ������ڲ�ͬ
    int multiCircle;
    QCheckBox* m_cbSavePeriod;
    QLineEdit* m_lePeroid;
    uint m_iSavePeroidNum;
    uint countPeroid;
    QGroupBox* gp;
    //�Ƿ���ʾ�Զ����Խ���
    bool m_bShowAutoTest;
    //ʹ��ʱ��
    //uint m_iEnableCount;

    SerialportManage* serialport = nullptr;
    QByteArray OpenLowPwSerial;     //��ѹ��ѹ�̵������������Ͽ���������
    QByteArray CloseLowPwSerial;    //��ѹ��Դ�̵����رգ�����ͨ
    QByteArray OpenPureSerial;      //��ˮ���̵����������գ�
    QByteArray ClosePureSerial;     //��ˮ���̵�����
    int m_iBlowAirTime = 120000;    //����ʱ��(ms)
    int m_iWashTime = 120000;       //��ˮ��ϴʱ��(ms)
    int m_iBackWaterTime = 120000;       //��ˮ��ϴʱ��(ms)
    std::set<QString>NoVersionSet{"���Ͽ���"};
    bool isCANTestTempetrue = false;

private slots:
      void on_CapturePower();
      void on_CaptureElapseTime();
      void on_canSettingShowFromLogin(int n);
private slots:
    //CANЭ������
    void qCanSettingShow();
    //ѡ��ǰ�ͺ�
    void on_CurrentModelChanged(int index);
    //ѡ��ǰƽ̨
    void on_CurrentPlatformChanged(int index);
    //����
    void on_pbSend_clicked(bool clicked);
    //ˢ���豸
    void on_pbRefreshDevice_clicked();
    //���豸
    void on_pbOpenPcan_clicked();
    //˫���༭�����ͱ�񡱵�����
    void on_tableDoubleClicked(int,int);
    //����еĵ�Ԫ�����ݷ����仯��
    void on_tableClicked(int, int);
    //CAN/LIN ���ݽ���
    void on_ReceiveData(uint frame_id, QByteArray data);
    void on_ReceiveDataLIN(uint frame_id, QByteArray data,int reserve);
    void on_ReceiveDataMulti(uint frame_id, QByteArray data);
    void on_ReceiveData(int ch,uint frame_id, QByteArray data);
    //void on_ReceiveData2(ushort ch,uint frame_id, uchar data[8]);
    //CANЭ�����õĴ��ڹر���Ӧ������ˢ�����õ�����
    void on_SettingWidowsClose();
    //�����ͱ���е����������
    void on_cbSelectSendItemChanged(int);
    //������ʾ
    void on_setInToRollData();
    void on_setInToRollDataMult(int ch);
    //���ݼ��
    void on_checkTraceChanged(int);
    //��������
    void on_pbSaveCanData_clicked();
    //�������
    void on_pbClearCanData_clicked();

    void on_checkSendChanged(int);
    //���ճ�ʱ����
    void on_recTimeout();
    void on_recTimeoutMutil();
    //�Զ�������
    //void on_autoWork(bool isRun);
    //�Զ��������̷߳������źŵĽ��պ���
    //void on_recSigEndRunWork(int n,int channel);
    //debug�õ�step
    void on_lineEdit_editingFinished();
    //��ȡ�汾�ţ�����ר�ã�
    void on_pbGetVer_clicked(bool);

    void on_pbClearLogShow_clicked();
    //CAN���ӵ�����
    void on_cbCanType_currentIndexChanged(int index);
    //void on_commitData(const QByteArray& byte, int id);

    void on_action_About_triggered();
    void on_action_History_triggered();

    void on_pbLogReplay_clicked();
    
    //***************�°����*****************//
    //�����Զ�����
    void on_pbStartAutoTest_clicked(bool b);
    //�豸����
    void on_pbDevicesManage_clicked();
    //ͨ�ò���
    void on_pbGeneralParameter_clicked();
    //��������̵߳�һЩ��������
    void on_processAutoTestSignal(int n, QString str);
    void on_lineEditCodeIn_editingFinished();
    void on_pbSummitCode_clicked();
    void on_SendMesState(int n, QString str);
    void on_RecFlowCool(float flow);

    void on_sigFromPowerNewData(QString data);
    //Mes�ر��Ĳۺ���
    void on_sigFroMesNewData(QString data);
    //��Դ���زۺ���
    void on_sigFromThisPowerSet(QString data);
    //���ݱ������ڿ���
    void on_savePeriodCheck_Changed(int n);
    //�����¶ȼ��monitor
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
