#pragma once
#include "./Abstract/LogAbstract.h"

class LogTrc :public LogMessage::LogBase
{
	Q_OBJECT
public:
	LogTrc(LogBase* parent=nullptr);
	virtual bool loadLogFile(const QString& file, const QStringList& fifter, messageMange* LogData) override;
private:
	void readTrc(messageMange* LogData);
private:
	QString m_sAscName;
	QStringList m_fifter;
	bool m_RunState = true;
	bool m_isFilter = false;
	bool m_isReadFile = false;
};

