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
protected:
	void run();
private:
	QString datalist_name;
	QStringList dataList;
	int rows;
	QString filepath;
	bool isRun = true;
};
