#pragma once

#include <QObject>
#include "../dataStruct.h"
class MsgParser  : public QObject
{
	Q_OBJECT

public:
	MsgParser(QObject *parent);
	~MsgParser();
	static float intel_Parser(const protoItem& pItem, const QByteArray& data, bool isSend);
	static float moto_Msb_Parser(const protoItem& pItem, const QByteArray& data, bool isSend);
	static float moto_Lsb_Parser(const protoItem& pItem, const QByteArray& data, bool isSend);
	static QStringList QByteToBinary(const QByteArray& data);
};
