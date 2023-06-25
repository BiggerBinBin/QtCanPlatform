#pragma once
#include <QString>
#include <iostream>
#include<vector>
#include <QColor>
#include <QTimer>
struct AutoTestStruct
{
	int m_iEnableInLine;		//0 使能按钮所在行
	int m_iEnOp;				//1 使能时下拉框选哪个，亿华通是2，其它是1，唉
	int m_iPowerInLine;			//2 功率输入框所在行
	int m_iLowVoltage;			//3 高压欠压阈值
	int m_iLowVoltageRe;		//4 欠压恢复
	int m_iOverVoltage;			//5 高压过压阈值
	int m_iOverVoltageRe;		//6 过压恢复
	int m_iOverTemperature;		//7 过温保护
	int m_iOverTempRe;			//8 过温恢复
	int m_iOverTempTolerance;	//9 过温恢复/保护误差
	int m_iRatedVolt;			//10 额定电压
	float m_fRequirePW;			//11 测试时请求的功率，有的是占空比，有的是功率，所以需要这个参数
	float m_fRatedPW;			//12 额定功率
	QString m_strTolerance;		//13 功率误差
	QString m_strVer;			//14 软件版本号
	QString m_strVerRecID;		//15 版本号解析的ID
	ushort m_usIDBytelen;		//16 版本由几个字段组成
	ushort m_usIDByteStartbit;	//17 版本解析的起止位
	int m_usRatedPWTemp;		//18 额定功率的温度
	int m_usRatedPWFlow;		//19 额定功率的流量
	int m_usHeatTemp;			//20 加热起始温度
	int m_usCoolTemp;			//21 制冷温度
	int m_bTurnOffFlow;			//22 测试过温保护时是否关闭流量
	int m_bTurnOffCool;			//22 测试过温保护时是否关闭制冷
	int m_iOutTempStop;			//23 出水口过温温度，到达就停止使能
	ushort m_usOutOrInTemp;		//24 使用出水口温度还是入水口温度（0出，1入）
	//bool isIndepandSend;

};
struct RollStruct
{
	float value;
	QString name;
};
struct YBRGB
{
	uchar r;
	uchar g;
	uchar b;
};
struct parseData
{
	QString name;
	float value;
	QString toWord;
	struct YBRGB color;
};
struct showTableData
{
	QString IdName;
	std::vector<parseData> Pdata;
};
//值对含义
struct cellProperty
{
	uint16_t r;									//红色分量
	uint16_t g;									//绿色分量
	uint16_t b;									//蓝色分量
	QString toWord;								//值的含义
	QString value;								//值
	bool isStand;								//是否正常值（即这个值为不是异常的值）
	//QColor  color;
	
};
//字段属性
struct protoItem {
	bool octhex;								//16进制否
	bool isRoll;								//是否把数据显示在另外一个表格
	//int send;									//发送的数据
	float send;									//发送的数据
	int startByte;								//起止字节
	int startBit;								//起止位
	int bitLeng;								//长度
	float precision;							//精度
	int offset;									//偏移
	QString bitName;							//字段名称
	QString dataFrom;							//来源，如报文只有电流电压，要显示功率，就用电流电压相乘
	std::vector<cellProperty>stl_itemProperty;	//显示的属性
	//std::map<QString, cellProperty>itemProperty;
};
//CAN-ID数据结构体
struct canIdData {
	bool isSend;							//是否启用这个ID
	uint8_t opt;							//接收报文还是发送报文
	uint8_t len;							//数据长度，LIN通信用
	int circle;						//当为发送ID时，此ID的发送周期
	QTimer* timeAdd;							//定时器地址
	QString strCanId;						//CAN-ID
	std::vector<struct protoItem>pItem;		//数据
};
//型号数据结构体
struct protoData {
	
	uint8_t agreement;				//协议,0:intel,1:motolora
	int bundRate;					//波特率
	int circle;						//报文周期
	QString modelName;				//名称
	std::vector<canIdData>cItem;	//数据
	bool bStandardId;				//是否为标准帧
	QString sPlatform;				//平台:3kW,5kW,7kW,10kW,15kW
	struct AutoTestStruct ats;		//自动测试参数
};
struct autoTestData {
	int m_iLowVolt;				//欠压起步电压
	int m_iHeightVolt;			//过压起步电压
	int m_iVoltStep;			//升压步进
	int m_iVoltError;			//电压误差
	int m_iHeatTempture;		//开始加热的温度
	int m_iPowerTempture;		//额定功率的温度
	int m_iOverTime;			//超时
	QString m_sInWebAddr;		//接口地址
	QString m_sOutWebAddr;		//接口地址
	int m_iShowType;			//右上角显示的方式。0：显示风暧操作，1：显示自动化测试操作
	int m_iRecOnNoSend;			//是否打开设备就接收数据？
	int m_iSaveListNum;			//多少条数据保存一次？
	bool m_bShowAutoTest;
};
struct messageData
{
	uint Id;
	double TimeStemp;
	uchar Data[8];
};
struct messageMange
{
	std::vector<messageData>vData;
	uint iLen;
	uint iIndex;
};
