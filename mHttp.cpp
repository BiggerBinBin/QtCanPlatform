#include "mHttp.h"
#include <qstringlist.h>
#include"QsLog.h"
#pragma execution_character_set("utf-8")  
///https://blog.csdn.net/weixin_45647036/article/details/119384749
mHttp::mHttp(QObject *parent)
	: QObject(parent)
{
	/*manager = new QNetworkAccessManager(this);
	manager2 = new QNetworkAccessManager(this);*/
	request = new QNetworkRequest();
}

mHttp::~mHttp()
{}
QString mHttp::sendHttp(QByteArray data, QString url)
{
	if (data.isEmpty())
		return "error";
	if (!manager)
	{
		manager = new QNetworkAccessManager(this);
		connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
	}
	//QNetworkRequest request;
	request->setUrl(QUrl(url));
	/*request.setRawHeader("Host", "eap2mes");*/
	request->setHeader(QNetworkRequest::ContentTypeHeader, "application/json");	//基础设置
	//request.setRawHeader("Content-Length", QString(data.size()).toUtf8());//设置发送数据大小
	manager->post(*request, data);
	return QString();
}
QString mHttp::sendHttpSys(QByteArray data, QString url)
{
	if (data.isEmpty())
		return "error";
	if (!manager2)
	{
		manager2 = new QNetworkAccessManager(this);
		//connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
	}
	//QNetworkRequest request;
	request->setUrl(QUrl(url));
	/*request.setRawHeader("Host", "eap2mes");*/
	//request->setRawHeader("Content-Type", "application/x-www-form-urlencoded");	//基础设置
	//request.setRawHeader("Content-Length", QString(data.size()).toUtf8());//设置发送数据大小
	request->setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	QNetworkReply* reply = manager2->post(*request, data);

	QEventLoop eventLoop;
	QTimer tt;
	tt.setInterval(timeOut);
	connect(&tt, SIGNAL(timeout()), &eventLoop, SLOT(quit()));
	connect(manager2, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
	eventLoop.exec();       //block until finish
	//reply->readAll();

	QVariant varbb = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
	if (!varbb.isValid())
	{
		QLOG_WARN() << "Http发送错误:" << "Status:" << varbb.toInt();
		return QString();

	}
	if (reply->error() != QNetworkReply::NoError)
	{
		QLOG_WARN() << tr(reply->errorString().toStdString().c_str());
		QLOG_INFO() << "Status:" << varbb.toInt();
		return QString();
	}
	QByteArray bytes = reply->readAll();

	QJsonParseError pError;
	QJsonObject qJson;
	qJson = QJsonDocument::fromJson(bytes, &pError).object();
	if (pError.error == QJsonParseError::NoError)
	{

		QLOG_INFO() << "msg:" << qJson.value("msg").toString().toStdWString();
		return qJson.value("msg").toString();
	}
	return QString();
		
}
void mHttp::replyFinished(QNetworkReply* reply)
{
	QVariant varbb = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
	if (!varbb.isValid())
	{
		QLOG_WARN() << "Http发送错误:" << "Status:" << varbb.toInt();
		return;

	}
	if (reply->error() != QNetworkReply::NoError)
	{
		QLOG_WARN() << tr(reply->errorString().toStdString().c_str());
		QLOG_INFO() << "Status:" << varbb.toInt();
		return;
	}
	QByteArray bytes = reply->readAll();

	QJsonParseError pError;
	QJsonObject qJson;
	qJson = QJsonDocument::fromJson(bytes, &pError).object();
	if (pError.error == QJsonParseError::NoError)
	{

		QLOG_INFO() << "msg:" << qJson.value("msg").toString().toStdWString();
	}
}
