#pragma once

#include "./Abstract/LogAbstract.h"

class LogAsc  : public LogMessage::LogBase
{
	Q_OBJECT

public:
	LogAsc(LogBase* parent = nullptr);
	virtual bool loadLogFile(const QString& file, const QStringList& fifter, messageMange* LogData) override;
	~LogAsc();
private:
	void readAsc(messageMange* LogData);
private:
	QString m_sAscName;
	QStringList m_fifter;
	bool m_RunState = true;
	bool m_isFilter = false;
	bool m_isReadFile = false;
};
