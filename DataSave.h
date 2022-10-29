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
