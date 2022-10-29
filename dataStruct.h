#pragma once
#include <QString>
#include <iostream>
#include<vector>
#include <QColor>
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
//
struct cellProperty
{
	uint16_t r;
	uint16_t g;
	uint16_t b;
	QString toWord;
	QString value;
	bool isStand;
	//QColor  color;
	
};
//字段属性
struct protoItem {
	bool octhex;								//16进制否
	bool isRoll;								//是否把数据显示在另外一个表格
	int send;									//发送的数据
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
	qint64 CanId;							//CAN-ID，弃用
	QString strCanId;						//CAN-ID
	std::vector<struct protoItem>pItem;		//数据
};
//型号数据结构体
struct protoData {
	
	uint8_t agreement;				//协议
	int bundRate;					//波特率
	int circle;						//报文周期
	QString modelName;				//名称
	std::vector<canIdData>cItem;	//数据
};
