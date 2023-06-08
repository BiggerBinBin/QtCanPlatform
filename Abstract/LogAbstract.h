#pragma once
#include <qobject.h>
#include <qstring.h>
#include "../dataStruct.h"

namespace LogMessage
{
	class LogBase:public QObject
	{
		Q_OBJECT
	public:
		LogBase(QObject* parent=nullptr);
		virtual ~LogBase();
		virtual bool loadLogFile(const QString& file, const QStringList& fifter, messageMange* LogData) = 0;
	Q_SIGNALS:
		void sigLoadState(int n);
	private:

	};

	
}