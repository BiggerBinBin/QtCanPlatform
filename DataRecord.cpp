#include "DataRecord.h"


DataRecord::DataRecord()
{
    datalist_name = QString("序号,"
                            "北京时间,"
                            "功率,"
                            "电流,"
                            "电压,"
                            "进口水温度,"
                            "出口水温度,"
                            "工作状态,"
                            "PTC状态,"
                            "IGBT状态,"
                            "高压反接,"
                            "PTC过温,"
                            "高压过压,"
                            "高压过流,"
                            "供电欠压,"
                            "供电过压,"
                            "PTC心跳,"
                            "通信状态,"
                            "高压欠压,"
                            "PTC过温,"
                            "进水口过温,"
                            "出水口过温,"
                            "进水口温感故障,"
                            "出水口温感故障,"
                            "高压开路,"
                            "高压短路,"
                            "MCU故障");
    QString path = QCoreApplication::applicationFilePath().mid(0,QCoreApplication::applicationFilePath().lastIndexOf('.'))+"_Data";
    QDateTime dateTime(QDateTime::currentDateTime());
    QString time_str = dateTime.toString("yyyy_MM_dd_hh_mm_ss_zzz");
    filepath = path + "/数据记录_" + time_str + ".xlsx";
}


void DataRecord::SaveData(QStringList list, int rows,QString path)
{
    dataList = list;
    this->rows = rows;
    this->filepath = path;


    this->start();
}


void DataRecord::run()
{
    if(dataList.count()>0)
    {
        if(!filepath.isEmpty()){
            CoInitializeEx(nullptr, COINIT_MULTITHREADED);
            QAxObject *excel = new QAxObject(this);
            //连接Excel控件
            excel->setControl("Excel.Application");
            //不显示窗体
            excel->dynamicCall("SetVisible (bool Visible)","false");
            //不显示任何警告信息。如果为true那么在关闭是会出现类似“文件已修改，是否保存”的提示
            excel->setProperty("DisplayAlerts", false);
            //获取工作簿集合
            QAxObject *workbooks = excel->querySubObject("WorkBooks");
            //新建一个工作簿
            workbooks->dynamicCall("Add");
            //获取当前工作簿
            QAxObject *workbook = excel->querySubObject("ActiveWorkBook");
            //获取工作表集合
            QAxObject *worksheets = workbook->querySubObject("Sheets");
            //获取工作表集合的工作表1，即sheet1
            QAxObject *worksheet = worksheets->querySubObject("Item(int)",1);




            //表头
            QString str = datalist_name;
            QStringList strlist = str.split(",");


            strlist.removeAll("");
            int listCount = strlist.count();
            QString cellCol;
            for(int j=0;j<listCount;j++)
            {
                if(j>25)
                    cellCol ="A"+QString('A'+j-26)+QString::number(1);          //excel表格，第27项是AA了
                else if(j<=25)
                    cellCol = QString('A'+j)+QString::number(1);
                else
                    return;


                QAxObject *cellRow = worksheet->querySubObject("Range(QVariant, QVariant)",cellCol);
                cellRow->dynamicCall("SetValue(const QVariant&)",QVariant(strlist[j]));


                QAxObject *font = cellRow->querySubObject("Font");  //获取单元格字体
                font->setProperty("Bold", true);  //设置单元格字体加粗
            }




            //设置表格数据
            if(dataList.count()>0)
                for(int i=0;i<rows-1;i++){
                    QString str = dataList.at(i);
                    QStringList strlist = str.split(",");
                    //            QString str = model->data(model->index(i),Qt::DisplayRole).toString();
                    //            QStringList strlist = str.split(" ");


                    strlist.removeAll("");
                    for(int j=0;j<strlist.count();j++)
                    {
                        //QString cellCol = QString('A'+j)+QString::number(i+2);
                        if(j>25)
                            cellCol ="A"+QString('A'+j-26)+QString::number(i+2);    //excel表格，第27项是AA了
                        else if(j<=25)
                            cellCol = QString('A'+j)+QString::number(i+2);
                        else
                            return;
                        QAxObject *cellRow = worksheet->querySubObject("Range(QVariant, QVariant)",cellCol);
                        cellRow->dynamicCall("SetValue(const QVariant&)",QVariant(strlist[j]));
                    }
                }


            workbook->dynamicCall("SaveAs(const QString&)",QDir::toNativeSeparators(filepath));//保存至filepath，注意一定要用QDir::toNativeSeparators将路径中的"/"转换为"\"，不然一定保存不了。
            workbook->dynamicCall("Close()");//关闭工作簿
            excel->dynamicCall("Quit()");//关闭excel
            delete excel;
            excel=NULL;
        }
    }
    dataList.clear();
    rows = 0;
}