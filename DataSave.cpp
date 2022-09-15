#include "DataSave.h"
#include <windows.h>
#include <QFileDialog>
DataSave::DataSave(QObject *parent)
	: QThread(parent)
{}

DataSave::~DataSave()
{
    isRun = false;
    msleep(1000);
}

void DataSave::SaveData(QStringList list, int rows, QString path)
{
    dataList = list;
    this->rows = rows;
    this->filepath = path;
    this->start();
}

bool DataSave::setTitle(QString title)
{
    datalist_name = title;
    return true;
}

void DataSave::run()
{
    if (dataList.count() > 0)
    {
        if (!filepath.isEmpty()) {
            HRESULT re = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
            QAxObject* excel = new QAxObject(this);
            //连接Excel控件
            excel->setControl("Excel.Application");
            //不显示窗体
            excel->dynamicCall("SetVisible (bool Visible)", "false");
            //不显示任何警告信息。如果为true那么在关闭是会出现类似“文件已修改，是否保存”的提示
            excel->setProperty("DisplayAlerts", false);
            //获取工作簿集合
            QAxObject* workbooks = excel->querySubObject("WorkBooks");
            //新建一个工作簿
            workbooks->dynamicCall("Add");
            //获取当前工作簿
            QAxObject* workbook = excel->querySubObject("ActiveWorkBook");
            //获取工作表集合
            QAxObject* worksheets = workbook->querySubObject("Sheets");
            //获取工作表集合的工作表1，即sheet1
            QAxObject* worksheet = worksheets->querySubObject("Item(int)", 1);




            //表头
            QString str = datalist_name;
            QStringList strlist = str.split(",");


            strlist.removeAll("");
            int listCount = strlist.count();
            QString cellCol;
            for (int j = 0; j < listCount; j++)
            {
                if (j > 25)
                    cellCol = "A" + QString('A' + j - 26) + QString::number(1);          //excel表格，第27项是AA了
                else if (j <= 25)
                    cellCol = QString('A' + j) + QString::number(1);
                else
                    return;


                QAxObject* cellRow = worksheet->querySubObject("Range(QVariant, QVariant)", cellCol);
                cellRow->dynamicCall("SetValue(const QVariant&)", QVariant(strlist[j]));


                QAxObject* font = cellRow->querySubObject("Font");  //获取单元格字体
                font->setProperty("Bold", true);  //设置单元格字体加粗
                if (!isRun)
                    return;
            }




            //设置表格数据
            if (dataList.count() > 0)
                for (int i = 0; i < rows; i++) {
                    QString str = dataList.at(i);
                    QStringList strlist = str.split(",");
                    //            QString str = model->data(model->index(i),Qt::DisplayRole).toString();
                    //            QStringList strlist = str.split(" ");


                    strlist.removeAll("");
                    for (int j = 0; j < strlist.count(); j++)
                    {
                        //QString cellCol = QString('A'+j)+QString::number(i+2);
                        if (j > 25)
                            cellCol = "A" + QString('A' + j - 26) + QString::number(i + 2);    //excel表格，第27项是AA了
                        else if (j <= 25)
                            cellCol = QString('A' + j) + QString::number(i + 2);
                        else
                            return;
                        QAxObject* cellRow = worksheet->querySubObject("Range(QVariant, QVariant)", cellCol);
                        cellRow->dynamicCall("SetValue(const QVariant&)", QVariant(strlist[j]));
                        if (!isRun)
                        {
                            workbook->dynamicCall("SaveAs(const QString&)", QDir::toNativeSeparators(filepath));//保存至filepath，注意一定要用QDir::toNativeSeparators将路径中的"/"转换为"\"，不然一定保存不了。
                            workbook->dynamicCall("Close()");//关闭工作簿
                            excel->dynamicCall("Quit()");//关闭excel
                            delete excel;
                            excel = NULL;
                            return;
                        }
                            
                    }
                }


            workbook->dynamicCall("SaveAs(const QString&)", QDir::toNativeSeparators(filepath));//保存至filepath，注意一定要用QDir::toNativeSeparators将路径中的"/"转换为"\"，不然一定保存不了。
            workbook->dynamicCall("Close()");//关闭工作簿
            excel->dynamicCall("Quit()");//关闭excel
            delete excel;
            excel = NULL;
        }
    }
    dataList.clear();
    rows = 0;
}
