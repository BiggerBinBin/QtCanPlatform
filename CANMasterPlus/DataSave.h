/*
 * QDeviceCtrl.h - header file of QDeviceCtrl class
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

#include <QThread>
#include <QAxObject>
#include <QStringList>
class DataSave  : public QThread
{
	Q_OBJECT

public:
	DataSave(QObject *parent=nullptr);
	~DataSave();
	void SaveData(QStringList list, int rows, QString path);
	bool setTitle(QString title);
	void writeExcelFast(QString fileName, QList<QList<QVariant>>& x_y);
	void castListListVariant2Variant(QList<QList<QVariant>>& cells, QVariant& res);
	void convert2ColName(int data, QString& res);
	QString to26AlphabetString(int data);
	QList<QList<QVariant>> paraString(QStringList list);
protected:
	void run();
private:
	QString datalist_name;
	QStringList dataList;
	int rows;
	QString filepath;
	bool isRun = true;
};
