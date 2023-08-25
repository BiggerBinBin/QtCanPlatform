#include "LogAsc.h"
#include <Qtconcurrent>
#include <QFile>
#include <QTextStream>
#include <QsLog.h>
#include <qstringlist.h>
LogAsc::LogAsc(LogBase* parent) :LogBase(parent)
{

}
LogAsc::~LogAsc()
{

}

void LogAsc::readAsc(messageMange* LogData)
{
	QFile fp(m_sAscName);
	if (!fp.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QLOG_WARN() << "Open Asc file fail!";
		return;
	}
	QTextStream in(&fp);
	uint lastTime = 0;
	do
	{
		QString line = in.readLine();
		if (line.isEmpty())
			continue;
		QStringList strspilte = line.split(QRegularExpression("[,\\s]+"), QString::SkipEmptyParts);

		if (strspilte.size() < 6)
			continue;
		if(strspilte.at(3)!="Rx" && strspilte.at(3) != "Tx")
			continue;
		if(strspilte.at(4) != "d")
			continue;
		ushort len = strspilte.at(5).toUShort();
		if (strspilte.size() < (6 + len))
			continue;
		QString strID = strspilte.at(2);
		strID.replace("x", "");
		
		uint id = strID.toUInt(nullptr, 16);
		//本型号的ID才要，其它的不要
		bool bGet = false;
		for (auto sid : m_fifter)
		{
			if (sid.toUInt(nullptr, 16) != id)
				continue;
			bGet = true;
		}
		if (!bGet)
			continue;
		messageData msg;
		memset(msg.Data, 0, 8);
		for (int m = 0; m < len; m++)
		{
			msg.Data[m] = strspilte.at(6 + m).toUShort(nullptr,16);
		}
		msg.Id = id;
		msg.TimeStemp = fabs(strspilte.at(0).toDouble() * 10000 - lastTime);
		lastTime = strspilte.at(0).toDouble() * 10000;
		LogData->iLen++;
		LogData->vData.push_back(msg);
	} while (!in.atEnd());
	emit sigLoadState(0);
}

bool LogAsc::loadLogFile(const QString& file, const QStringList& fifter, messageMange* LogData)
{
	m_sAscName = file;
	m_fifter = fifter;
	m_RunState = true;
	QtConcurrent::run(this, &LogAsc::readAsc, LogData);
	m_isReadFile = true;
	return true;
	
}
