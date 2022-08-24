#pragma once
#include <QString>
#include <iostream>
#include<vector>
#include <QColor>
struct cellProperty
{
	QString toWord;
	QColor  color;
};
struct protoItem {
	int startByte;
	int startBit;
	int bitLeng;
	int precision;
	int offset;
	QString bitName;
	std::map<QString, cellProperty>itemProperty;
};
struct canIdData {
	uint8_t opt;
	qint64 CanId;
	std::vector<struct protoItem>pItem;
};
struct protoData {
	uint8_t agreement;
	QString modelName;
	std::vector<canIdData>cItem;
};
