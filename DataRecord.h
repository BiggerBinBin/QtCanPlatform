#pragma once
#include <QObject>
#include <QThread>
#include <QDebug>
//excel
#include <QAxObject>
#include <QStandardPaths>
#include <QFileDialog>
#include <windows.h>
#include <QCoreApplication>
#include <QTime>
class DataRecord:public QThread
{
    Q_OBJECT
public:
    DataRecord();


    void SaveData(QStringList list, int rows,QString path);
    void save();
    void run();
private:


    QString datalist_name;
    QStringList dataList;
    int rows;
    QString filepath;
};
