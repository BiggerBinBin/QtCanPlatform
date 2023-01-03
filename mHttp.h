#pragma once

#include <QObject>
#include <QtNetwork>
#include <QHttpMultiPart>
#include <QNetworkReply>
class mHttp  : public QObject
{
	Q_OBJECT

public:
	mHttp(QObject *parent=nullptr);
	~mHttp();
	QString sendHttp(QByteArray data,QString url);
	QString sendHttpSys(QByteArray data,QString url);
private:
	QNetworkAccessManager* manager = nullptr;
	QNetworkAccessManager* manager2 = nullptr;
	QNetworkRequest* request = nullptr;
	int timeOut = 1500;
public slots:
	void replyFinished(QNetworkReply* reply);
};
