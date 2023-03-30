#pragma once

#include <QObject>
#include "qGboleData.h"
#include "dataStruct.h"
#include <QTimer>
class QBLFManage  : public QObject
{
	Q_OBJECT

public:
	QBLFManage(QObject *parent);
	~QBLFManage();
	void getBLFLog(const QString &file,const QStringList &fifter, messageMange* LogData);
	void startGetMes(const int circle, const QStringList idList);
	void stopGetMes();
private:
	void readBLF(messageMange* LogData);
private slots:
	void sendMessage();
private:
	QString m_sBlfName;
	QString m_sAscName;
	bool RunState = true;
	bool isFilter = false;
	bool isReadFile = false;
	QStringList fifter;
	//messageMange LogData;
	QTimer* m_tSendMes=nullptr;
signals:
	void sigStatus(int);
	void sigInit(int);
	void sigNewMessage(uint id, QByteArray data);
};
