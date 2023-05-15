#include "mHttp.h"
#include <qstringlist.h>
#include"QsLog.h"
#include <windows.h>
#include <locale>//setlocale(LC_ALL,"");
#include <codecvt>
#include <sstream>

#pragma execution_character_set("utf-8")  
///https://blog.csdn.net/weixin_45647036/article/details/119384749
#define POST_OK 200
wchar_t* QStringToWchar(QString qStr)
{
	return (wchar_t*)reinterpret_cast<const wchar_t*>(qStr.utf16());
}
mHttp::mHttp(QObject *parent)
	: QObject(parent)
{
	/*manager = new QNetworkAccessManager(this);
	manager2 = new QNetworkAccessManager(this);*/
	request = new QNetworkRequest();
	/*soapHttp = new QtSoapHttpTransport(this);
	connect(soapHttp, SIGNAL(responseReady(const QtSoapMessage&)), this, SLOT(onResponse(const QtSoapMessage&)));*/
	
}

mHttp::~mHttp()
{}
QString mHttp::sendWebMesIN(QString url, QStringList data)
{
	return QString();
}
QString mHttp::sendWebMesOUT(QString url, QStringList data)
{
	return QString();
}
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
	//request->setRawHeader("Host", "10.7.1.9:8060");
	request->setRawHeader("Content-Type", "application/x-www-form-urlencoded");	//基础设置
	request->setRawHeader("Content-Length", QString(data.size()).toUtf8());//设置发送数据大小
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
	request->setUrl(QUrl("/BarCodeService.asmx/TrackInData_BarCode?DeviceID=PHU7KW-007&Barcode=20221009005-02"));
	request->setRawHeader("Host", "10.7.1.9:8060");
	QNetworkReply* reply = manager2->get(*request);

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
