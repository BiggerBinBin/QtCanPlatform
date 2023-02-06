#include "DataSave.h"
#include <windows.h>
#include <QFileDialog>
#include "QsLog.h"
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

    QList<QList<QVariant>> bb = paraString(dataList);
    writeExcelFast(this->filepath, bb);
    return;
    //===========以下是老的代码,=========//
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
QList<QList<QVariant>> DataSave::paraString(QStringList strlists)
{
    QList<QList<QVariant>> bb;

    QList<QVariant> kk;
    
    //设置表格数据
    QStringList str = datalist_name.split(",");
    for (int m = 0; m < str.size(); m++)
        kk.append(str.at(m));
    bb.append(kk);
    for (int i = 0; i < dataList.count(); i++) {
        QString str2 = dataList.at(i);
        QStringList strlist = str2.split(",");
        QList<QVariant> aa;
        for (int k = 0; k < strlist.count(); k++)
        {
            aa.append(strlist.at(k));
        }
        int n = aa.size();
        int b = str.size();
        //为了防止有些数据项是空的，后面要补上
        if (aa.size() < str.size())
        {
            for (int i = 0; i < b - n; i++)
                aa.append("--");
        }
        bb.append(aa);
    }
    return bb;
        
}
void DataSave::writeExcelFast(QString fileName, QList<QList<QVariant>>& x_y)
{
    //初始化COM组件，其实单次调用即可，多次调用也没关系，只不守返回值可能为false
    HRESULT re = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    QAxObject* excel = new QAxObject(this);
    if (!excel)
    {
        QLOG_WARN() << "Init QAxObject Failure";
        return;
    }
    //连接Excel控件
    excel->setControl("Excel.Application");
    //excel->setControl("ket.Application");
    //不显示窗体
    excel->dynamicCall("SetVisible (bool Visible)", "false");
    //不显示任何警告信息。如果为true那么在关闭是会出现类似“文件已修改，是否保存”的提示
    excel->setProperty("DisplayAlerts", false);
    //获取工作簿集合
    QAxObject* workbooks = excel->querySubObject("WorkBooks");
    if (!workbooks)
    {
        QLOG_WARN() << "获取Excel控件失败，系统是否已经安装了Office？";
        return;
    }
    workbooks->dynamicCall("Add");//新建一个工作表。 新工作表将成为活动工作表

    QAxObject*  workbook = excel->querySubObject("ActiveWorkBook");// 获取活动工作簿 
    if (!workbook)
    {
        QLOG_WARN() << "获取Excel工作薄失败";
        return;
    }
    QAxObject*  worksheet = workbook->querySubObject("Sheets(int)", 1); //获取第一个工作表，最后参数填1
    if (!worksheet)
    {
        QLOG_WARN() << "获取工作表失败";
        return;
    }

    int row = x_y.size();//行数
    int col = x_y.at(x_y.size()-1).size();//列数
    /*将列数转换成EXCEL中列的字母形式*/
    QString rangStr;
    convert2ColName(col, rangStr);
    rangStr += QString::number(row);
    rangStr = "A1:" + rangStr;
    //QLOG_INFO() << "rangStr:" << rangStr;
    QAxObject*  usedrange_Write = worksheet->querySubObject("Range(const QString&)", rangStr);

    QVariant var;
    castListListVariant2Variant(x_y, var);
    usedrange_Write->setProperty("Value", var);
    //worksheet->dynamicCall("Protect(Password)", "123");
    workbook->dynamicCall("SaveCopyAs(QString)", QDir::toNativeSeparators(fileName));
    workbook->dynamicCall("Close(bool)", false);  //关闭文件
    excel->dynamicCall("Quit()");//关闭excel
    delete excel;
    excel = NULL;
   
    
}

void DataSave::castListListVariant2Variant(QList<QList<QVariant>>& cells, QVariant& res)
{
    QVariantList vars;
    const int rowCount = cells.size();
    for (int i = 0; i < rowCount; ++i)
    {
        vars.append(QVariant(cells[i]));
    }
    res = QVariant(vars);
}

// brief 把列数转换为excel的字母列号
// param data 大于0的数
// return 字母列号，如1->A 26->Z 27 AA
void DataSave::convert2ColName(int data, QString& res)
{
    Q_ASSERT(data > 0 && data < 65535);
    int tempData = data / 26;
    if (tempData > 0)
    {
        int mode = data % 26;
        convert2ColName(mode, res);
        convert2ColName(tempData, res);
    }
    else
    {
        res = (to26AlphabetString(data) + res);
    }
}

// brief 数字转换为26字母
// 1->A 26->Z
QString DataSave::to26AlphabetString(int data)
{
    QChar ch = data + 0x40;//A对应0x41
    return QString(ch);
}
