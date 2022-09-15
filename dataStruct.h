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
struct cellProperty
{
	uint16_t r;
	uint16_t g;
	uint16_t b;
	QString toWord;
	QString value;
	//QColor  color;
	
};
struct protoItem {
	bool isRoll;
	int send;
	int startByte;
	int startBit;
	int bitLeng;
	float precision;
	int offset;
	QString bitName;
	QString dataFrom;
	std::vector<cellProperty>stl_itemProperty;
	//std::map<QString, cellProperty>itemProperty;
};
//CAN-ID数据结构体
struct canIdData {
	bool isSend;
	uint8_t opt;
	qint64 CanId;
	QString strCanId;
	std::vector<struct protoItem>pItem;
};
//型号数据结构体
struct protoData {
	
	uint8_t agreement;
	QString modelName;
	std::vector<canIdData>cItem;
};
