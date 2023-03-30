#include "QParseMessages.h"

QParseMessages::QParseMessages(QObject *parent)
	: QObject(parent)
{

}

QParseMessages::~QParseMessages()
{

}

bool QParseMessages::parseMessage_intel(const QByteArray& data, showTableData* tableData)
{
	return false;
}

bool QParseMessages::parseMessage_MotoMSB(const QByteArray& data, showTableData* tableData)
{
	return false;
}

bool QParseMessages::parseMessage_MotoLSB(const QByteArray& data, showTableData* tableData)
{
	return false;
}
