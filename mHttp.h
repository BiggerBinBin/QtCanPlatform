/*
 * mHttp.h - header file of mHttp class
 *
 * Copyright (C) 2021-2022 lyb <liyangbinbin@foxmail.com>
 *
 * This file is part of the QtCanPlatform.
 *
 * QtCanPlatform is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * QtCanPlatform is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libQtShadowsocks; see the file LICENSE. If not, see
 * <http://www.gnu.org/licenses/>.
 */
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
	QString sendWebMesIN(QString url, QStringList);
	QString sendWebMesOUT(QString url, QStringList);
	QString sendHttp(QByteArray data,QString url);
	QString sendHttpSys(QByteArray data,QString url);
private:
	QNetworkAccessManager* manager = nullptr;
	QNetworkAccessManager* manager2 = nullptr;
	QNetworkRequest* request = nullptr;
	
	int timeOut = 3000;
	//BarCodeServiceSoapProxy* BarSer = nullptr;
public slots:
	void replyFinished(QNetworkReply* reply);
	
};
