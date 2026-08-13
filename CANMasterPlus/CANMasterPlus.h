#pragma once

#include <QtWidgets/QMainWindow>
#include <qscopedpointer.h>
#include <QLabel>
#include "ui_CANMasterPlus.h"
#include "../ModelDataManage/Unities/dataStruct.h"
#include "MsgQueue.h"
#include "TP700TemperatureModel.h"
#include <QMutex>
//#include "mdFiveCalculate.h"
typedef struct mes
{
    bool isFresh;
    uint32_t timeStrmp;
    QByteArray msg;
}MessageStr;

//calss declaration
class Communicate;
class QPushButton;
//class QLabel;
class QTableWidget;
class QComboBox;
class MessageView;
class QTextBrowser;
class ModelDataManage;
class RequestTableManage;
class ReceiveTableManage;
class RollShowManage;
class HilGui;
class SysSetting;
class Login;
class CANMasterPlus : public QMainWindow
{
    Q_OBJECT
    //Q_PROPERTY(int UserType READ getUserType WRITE setUserType)
public:
    CANMasterPlus(QWidget *parent = nullptr);
    ~CANMasterPlus();
    
private:
    Ui::CANMasterPlusClass ui;

    //CAN/LIN devices ptr
    QScopedPointer<Communicate> m_CommuniPtr;

    //top bar widdets
    QComboBox* comboxDevType = nullptr;
    QComboBox* comboxDevChannel = nullptr;
    QPushButton* pbRefreshDev = nullptr;
    //select model button
    QComboBox* comboxModel = nullptr;
    //select baudtate button
    QComboBox* comboxBaudRate = nullptr;
    //open device button
    QPushButton* pbOpenDevice = nullptr;
    QPushButton* pbSendMsg = nullptr;
    //platform select button
    QComboBox* comboxPlatform = nullptr;
    QComboBox* comboxPTCType = nullptr;
    QLineEdit* devLineEdit = nullptr;
    QPushButton* pbSearch = nullptr;
    QPushButton* pbPicture = nullptr;

    //open model data manage ui
    QPushButton* pbModelManage = nullptr;
    //open testcase manage ui
    QPushButton* pbTestCaseConfigManage = nullptr;
    //open plug-in devices ui
    QPushButton* pbPluginDevices = nullptr;

    //show receive/transmit can message
    MessageView* messageview;
    //log show browser
    QTextBrowser* logBrower = nullptr;
    QTableWidget* testStatusTable = nullptr;
    QTabWidget* logStatusTab = nullptr;
    //global model data manager class
    ModelDataManage* modelManage = nullptr;

    //Request operator table manager
    RequestTableManage* reqTableManage = nullptr;
    ReceiveTableManage* recTableManage = nullptr;
    RollShowManage* rollTabManage = nullptr;
    HilGui* testCaseConfigGui = nullptr;
    TP700TemperatureModel* tp700 = nullptr;
    //SysSetting* sysSetGui = nullptr;
    std::shared_ptr<SysSetting>sysSetGui = nullptr;
    std::shared_ptr<Login>loginGui = nullptr;
    //mdFiveCalculate* thisMdFiveCalculate = nullptr;


    static QStringList platform;
    static QStringList baudRate;
    static QStringList ptctype;
    //current model's data
    protoData m_currModelData;	
    //multiple id and cycle
    int m_multiCycle = 0;
    QTimer* m_timerSingelCycle = nullptr;
    int m_dev_channel = 0;
   
    //communicate status
    QLabel m_connect_status;
    QTimer* m_connect_status_timer = nullptr;
    uint communicate_loss_time = 5000;
    //LIN schedule 
    uint m_iLINDelayTime = 10;

    QMap<uint32_t, MessageStr>m_msgMap;
    QMutex m_mutex;

    std::vector<QString>HashArr;//开局初始化UI后，将所有型号进行存储，然后根据KW选择时找到正确的索引
    
private Q_SLOTS:
    void on_comboxDevType_IndexChanged(int index);
    void on_comboxDevChannel_IndexChanged(int index);
    void on_comboxModel_IndexChanged(int index);
    void on_pbRefreshDev_Clicked(bool b);
    void on_comboxBaudRate_IndexChanged(int index);
    void on_pbOpenDevice_Clicked(bool b);
    void on_pbSendMsg_Clicked(bool b);
    void on_comboxPlatform_IndexChanged(int index);
    void on_pbModelManage_clicked(bool b);
    void on_pbTestCaseConfigManage_clicked(bool b);
    void transferMsg(int ch, unsigned int fream_id, QByteArray data);
    void on_timeout_SendMsg();
    void on_pbSaveData_clicked();
    void on_pbClearData_clicked();
    //log slot function
    void logSlot(QString str, int level);
    void tastItemInit();
    void tastcastStatus(int idex,int status);

    void on_timeout_communicate();

    //toolbar action slot
    //void on_actionAdmin_triggered();
    //void on_actionTechnician_triggered();
    void on_actionDataSave_triggered();
    void on_actionLogin_triggered();
    void on_actionLogout_triggered();
    void on_actionUserTypeChanged(int usertype);
    void on_actionProtocolEdit_triggered();
    void on_actionTestCaseEdit_triggered();
    void on_actionPluginDevices_triggered();
    void on_actionHistory_triggered();
    void on_actionCalculate_triggered();
    void on_pbSearch_clicked(); 
    void on_pbPicture_clicked();
private:
    void InitGUI();
    void ConnectWidget();
    void initLogger();
    void destroyLogger();
    void reGetPTCType(QStringList);
Q_SIGNALS:
    void sigCurrModelChanged(int index);
};
