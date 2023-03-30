#pragma once

#include <QObject>
#include "dataStruct.h"
#include <QByteArray>
class QParseMessages  : public QObject
{
	Q_OBJECT

public:
	QParseMessages(QObject *parent);
	~QParseMessages();
	static bool parseMessage_intel(const QByteArray& data, struct showTableData* tableData);
	static bool parseMessage_MotoMSB(const QByteArray& data, struct showTableData* tableData);
	static bool parseMessage_MotoLSB(const QByteArray& data, struct showTableData* tableData);
private:

};
