#include <QMessageBox>
#include <QtDBus/QDBusMessage>
#include <qheaderview.h>
#include <QtConcurrent>
#include <qapplication.h>
#include "ReceiveTableManage.h"
#include "../../CANMasterPlus/ModelDataManage/Unities/MsgParser.h"
#include "../../CANMasterPlus/ModelDataManage/Unities/AlgorithmSet.h"
#include "../DeviceIO/3rd/QsLog/include/QsLog.h"
#include "DataSave.h"
ReceiveTableManage::ReceiveTableManage(QObject *parent)
	: QObject(parent), tableWidget(new QTableWidget)
{
    qRegisterMetaType<std::vector<showTableData>>();
    //Q_DECLARE_METATYPE(showTableData);
    tableWidget->horizontalHeader()->setHidden(true);
    tableWidget->verticalHeader()->setHidden(true);
    iniPath = QApplication::applicationDirPath() + "/Data/syssetting.ini";
    saveData = new DataSave();
    currentModelIndex = 0;
    //connect(this, &ReceiveTableManage::sigNewMsg, this, &ReceiveTableManage::onNewMsgIntoTable);
    m_TimerShow = new QTimer(this);
    connect(m_TimerShow, &QTimer::timeout, this, &ReceiveTableManage::onNewMsgIntoTable);
    //m_TimerShow->start()
    m_TimerLoss = new QTimer(this);
    connect(m_TimerLoss, &QTimer::timeout, this, &ReceiveTableManage::onStopShow);
    recDataIntoTab();
    readIni(iniPath);
   
    
}

ReceiveTableManage::~ReceiveTableManage()
{
    if (this->m_TimerShow)
    {
        this->m_TimerShow->stop();
    }
}
void ReceiveTableManage::transferMsg(int ch, unsigned int fream_id, QByteArray data)
{
    for (int i = 0; i < m_CurrentModel.cItem.size(); i++)
    {
        //if(m_CurrentModel.cItem.at(0).opt)
    }
    auto it = m_msg_gap.find(fream_id);
    static int grp_t;
    grp_t = 100;
    if (it != m_msg_gap.end())
    {
        grp_t = QDateTime::currentMSecsSinceEpoch() - it.value();
        if(grp_t>=50)
            m_msg_gap[fream_id] = QDateTime::currentMSecsSinceEpoch();
    }
    else
    {
        m_msg_gap[fream_id] = QDateTime::currentMSecsSinceEpoch();
    }
    if (grp_t >= 50)
    {
        QtConcurrent::run(this, &ReceiveTableManage::msgParser, ch, fream_id, data);
        if (this->m_TimerShow)
        {
            if (!this->m_TimerShow->isActive())
                this->m_TimerShow->start(this->m_CurrentModel.circle);
        }
        this->m_TimerLoss->start(this->m_CurrentModel.circle * 2);
    }
}



void ReceiveTableManage::clearData()
{
    m_dataSaveList.clear();
    recDataIntoTab();
}
bool ReceiveTableManage::readIni(QString path)
{
    QFile f(path);
    if (!f.exists())
    {
        //technologyPW = "1234";
        //adminPW = "kus@1234";
        return false;
    }
    std::shared_ptr<QSettings>s = std::make_shared<QSettings>(path, QSettings::IniFormat);//   new QSettings();
    this->m_iAutoSaveNum = s->value("m_iAutoSaveNum").toInt();
    this->m_bIsAutoSave = s->value("m_bIsAutoSave").toInt();
    return true;
}
bool ReceiveTableManage::recDataIntoTab()
{
    //=========================2024-06-10 The Dragon Boat Festival Reconstruction==========================
    m_excelTitle.clear();
    m_cur_count_item = 0;
    recCanData.clear();
    showTableVec.clear();
    m_excelTitle = QStringLiteral("序号,北京时间,");
    tableWidget->setColumnCount(this->tableColumn);
    if (!tableWidget)
        return false;
    int rcount = tableWidget->rowCount();
    for (int m = 0; m < rcount; m++)
        tableWidget->removeRow(rcount - m - 1);

    qGboleData* qGb = qGboleData::getInstance();
    if (!qGb->getIsInit())
        qGb->read();
    if (!qGb)return false;
    if (currentModelIndex > qGb->pGboleData.size() - 1 || currentModelIndex < 0)
    {
        //QMessageBox::warning(this, tr("warning"), tr("数据出错，当前型号不存在"));
        return false;
    }
    const protoData pTemp = qGb->pGboleData.at(currentModelIndex);
    m_CurrentModel = qGb->pGboleData.at(currentModelIndex);
    //canIdData cTemp
    for (int i = 0; i < pTemp.cItem.size(); i++)
    {
        //取出操作为接收的信号
        if (0 == pTemp.cItem.at(i).opt)
        {
            recCanData.push_back(pTemp.cItem.at(i));

            //建立一个vector，因为这是有序的，保证了界面上ID的消息顺序按照设置时的顺序来
            showTableData dd;
            for (int mk = 0; mk < pTemp.cItem.at(i).pItem.size(); mk++)
            {
                parseData p;
                p.name = pTemp.cItem.at(i).pItem.at(mk).bitName;
                p.toWord = "0";
                p.value = 0;
                p.color.b = 255;
                p.color.g = 255;
                p.color.r = 255;
                dd.Pdata.push_back(p);
            }
            dd.IdName = QString::number(pTemp.cItem.at(i).strCanId.toLong(nullptr, 16));
            showTableVec.push_back(dd);
        }
    }
    if (recCanData.size() <= 0)
    {
        //QMessageBox::warning(this, tr("warning"), tr("该型号没有接收信号，请添加再操作"));
        return false;
    }
    for (int i = 0; i < recCanData.size(); i++)
    {
        int num = recCanData.at(i).pItem.size();
        int mod_num = num % this->tableColumn;
        int sigma = mod_num == 0 ? (num / this->tableColumn) : (num / this->tableColumn + 1);
        int total_count = sigma * this->tableColumn;
        int cr = tableWidget->rowCount();
        //每加一行就要设置到表格去
        tableWidget->setRowCount(cr + 1);
        //当前列表格的下标
        int idex = 0;
        for (int j = 0; j < total_count; j++, idex++)
        {
            if (idex >= this->tableColumn)
            {
                idex = 0;
                cr = tableWidget->rowCount();
                tableWidget->setRowCount(cr + 2);
                cr += 1;
            }
            QTableWidgetItem* item;
            if(j< num)
                item = new QTableWidgetItem(recCanData.at(i).pItem.at(j).bitName);
            else
                item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            item->setForeground(QBrush(recFontColor));
            item->setBackgroundColor(recBackgroudColor);
            QFont ff;
            ff.setBold(true);
            item->setFont(ff);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            tableWidget->setItem(cr, idex, item);
            {
                //保存Excel的表头
                if (j < num)
                    m_excelTitle += recCanData.at(i).pItem.at(j).bitName + ",";

            }
            //tableWidget->setItem(cr+1, idex, new QTableWidgetItem(QString("/")));
        }
        cr = tableWidget->rowCount();
        tableWidget->setRowCount(cr + 1);
    }
    for(int mc=0;mc<tableWidget->columnCount();mc++)
        tableWidget->setColumnWidth(mc, 120);
    //m_TimerShow->start(this->m_CurrentModel.circle);
    return true;
    //if (tp700 && tp700->getIsUpdate())
        //{
        //    int cr = tableWidget->rowCount();
        //    //每加一行就要设置到表格去
        //    tableWidget->setRowCount(cr + 2);
        //    QStringList alias = tp700->getAliasName().split(",");
        //    for (int i = 0; i < alias.size() && i < 8; i++)
        //    {
        //        QTableWidgetItem* item = new QTableWidgetItem(alias.at(i));
        //        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        //        item->font().setBold(true);
        //        item->setBackgroundColor(recBackgroudColor);
        //        item->setForeground(QBrush(recFontColor));
        //        QFont ff;
        //        ff.setBold(true);
        //        item->setFont(ff);
        //        tableWidget->setItem(cr, i, item);
        //    }
        //}

    return true;
}
bool ReceiveTableManage::onNewMsgIntoTable()
{
    if (this->showTableVec.size() <= 0)
        return false;
    m_readwrite_lock.lockForRead();
    const std::vector<showTableData> m_showTableVec(this->showTableVec);

    m_readwrite_lock.unlock();
    QString dTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz");
    //QString dTemp = QString::number(m_cur_count_item) + "," + dTime ;
    QString dTemp = dTime;
    //m_readwrite_lock.lockForRead();
    std::vector<showTableData>::const_iterator iBeginV = m_showTableVec.begin();
    std::vector<showTableData>::const_iterator iEndV = m_showTableVec.end();
    int cr = 0;
    while (iBeginV != iEndV)
    {
        //每加一行就要设置到表格去
        int num = iBeginV->Pdata.size();
        int mod_num = num % tableColumn;
        int sigma = mod_num == 0 ? (num / tableColumn) : (num / tableColumn + 1);
        int total_count = sigma * tableColumn;
        int idex = 0;
        for (int j = 0; j < total_count; j++, idex++)
        {
            if (idex >= tableColumn)   //一行最多放10个数据
            {
                //满10个，从头开始
                idex = 0;
                //下一行的下一行，也就是隔一行，要加2；
                cr += 2;
            }
            if(j < num)
            {
                QString tnamp = iBeginV->Pdata.at(j).name;
                QString toword = iBeginV->Pdata.at(j).toWord;
                QTableWidgetItem* item = tableWidget->item(cr, idex);
                if (nullptr == item)continue;
                item->setText(tnamp);
                QTableWidgetItem* item2 = tableWidget->item(cr + 1, idex);
                if (nullptr == item2)
                {
                    item2 = new QTableWidgetItem(toword);
                    item2->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
                    tableWidget->setItem(cr + 1, idex, item2);
                }
                item2->setText(toword);
                QColor toWorlColor(iBeginV->Pdata.at(j).color.r, iBeginV->Pdata.at(j).color.g, iBeginV->Pdata.at(j).color.b);
                item2->setBackgroundColor(toWorlColor);
                dTemp += "," + toword;
            }
        }
        iBeginV++;
        cr += 2;
    }
   
    //m_readwrite_lock.unlock();

    /*if(QDateTime::currentMSecsSinceEpoch()- lastTime.toMSecsSinceEpoch() >= m_CurrentModel.circle)
    {
        m_cur_count_item++;
        dTemp = QString::number(m_cur_count_item) + "," + dTemp;
        m_dataSaveList.append(dTemp);
        lastTime = QDateTime::currentDateTime();
    }*/
    m_cur_count_item++;
    dTemp = QString::number(m_cur_count_item) + "," + dTemp;
    m_dataSaveList.append(dTemp);

    if (this->m_dataSaveList.size() >= this->m_iAutoSaveNum)
    {
        saveCanData();
        emit on_sigClearUIData();
    }

    return true;
}
/*
* @brief:保存CAN数据
* @param:无
* @return:无
*/
void ReceiveTableManage::saveCanData()
{
    QString appPath = QApplication::applicationDirPath() + "/PCAN-DATA/";
    QDir dd(appPath);
    if (!dd.exists())
    {
        dd.mkpath(appPath);
    }
    appPath += QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss-zzz") + ".xlsx";
    if (m_dataSaveList.size() < 1)
        return;
    //tableRollData->clear();

    saveData->setTitle(m_excelTitle);
    saveData->SaveData(m_dataSaveList, m_dataSaveList.size(), appPath);
    m_dataSaveList.clear();
    clearData();
    //on_pbClearCanData_clicked();
    //strSaveList.clear();
}
void ReceiveTableManage::msgParser(int ch, unsigned int fream_id, QByteArray data)
{
    
    QStringList binaryStr;
    binaryStr.clear();
    QString hex;
    for (int k = 0; k < data.size(); ++k)
    {
        QString str = QString("%1").arg((uint8_t)data[k], 8, 2, QLatin1Char('0'));
        binaryStr.append(str);

        hex += QString("%1").arg(str.toInt(NULL, 2), 2, 16, QLatin1Char('0')).toUpper() + " ";
    }

    bool isRoll = false;
    for (int i = 0; i < m_CurrentModel.cItem.size(); i++)
    {
        uint currID = m_CurrentModel.cItem.at(i).strCanId.toUInt(NULL, 16);
        if (currID != fream_id)
            continue;
        std::vector<parseData>parseArr;
        for (int m = 0; m < m_CurrentModel.cItem.at(i).pItem.size(); ++m)
        {
            float temp = 0;
            parseData pd;
            QString datafrom = m_CurrentModel.cItem.at(i).pItem.at(m).dataFrom;
            //由其它组成
            if (datafrom != "-1")
            {
                QStringList splt;
                QString symbol;
                if (datafrom.contains("XOR"))
                {
                    uchar cData[8];
                    for (int d = 0; d < 8; d++)
                    {
                        cData[d] = (uchar)data[d];
                    }
                    uchar res = checksumXOR(cData/*, data.size()*/);
                    uchar csum = data[m];
                    if (res == csum)
                    {
                        temp = 0;   //校验通过
                    }
                    else
                    {
                        temp = 1;
                    }
                }
                else if (datafrom.toUpper().contains("CH"))
                {
                    if (!m_GetTempPointer)
                        temp = -99.99;
                    else
                        temp = m_GetTempPointer->getTemperatur(datafrom);
                }
                else
                {
                    if (datafrom.contains("*"))
                    {
                        symbol = "*";
                    }
                    else if (datafrom.contains("/"))
                    {
                        symbol = "/";
                    }
                    else if (datafrom.contains("+"))
                    {
                        symbol = "+";
                    }
                    else if (datafrom.contains("-"))
                    {
                        symbol = "-";
                    }
                    splt = datafrom.split(symbol);
                    float v1 = 0;
                    float v2 = 0;
                    if (splt.size() > 1)
                    {
                        uint16_t f1 = splt.at(0).toUInt() - 1;    //行数从
                        uint16_t f2 = splt.at(1).toUInt() - 1;
                        if ((f1 < 0 || f1 >= m_CurrentModel.cItem.at(i).pItem.size()) || (f2 < 0 || f2 >= m_CurrentModel.cItem.at(i).pItem.size()))
                        {
                            temp = 0;
                            QLOG_WARN() << "paraser error: id = " << m_CurrentModel.cItem.at(i).strCanId << ",BitName=" << m_CurrentModel.cItem.at(i).pItem.at(m).bitName;
                        }
                        else
                        {
                            switch (m_CurrentModel.agreement)
                            {
                            case 0:
                                v1 = MsgParser::intel_Parser(m_CurrentModel.cItem.at(i).pItem.at(f1), data, false);
                                v2 = MsgParser::intel_Parser(m_CurrentModel.cItem.at(i).pItem.at(f2), data, false);
                                break;
                            case 1:
                                v1 = MsgParser::moto_Msb_Parser(m_CurrentModel.cItem.at(i).pItem.at(f1), data, false);
                                v2 = MsgParser::moto_Msb_Parser(m_CurrentModel.cItem.at(i).pItem.at(f2), data, false);
                                break;
                            case 2:
                                v1 = MsgParser::moto_Lsb_Parser(m_CurrentModel.cItem.at(i).pItem.at(f1), data, false);
                                v2 = MsgParser::moto_Lsb_Parser(m_CurrentModel.cItem.at(i).pItem.at(f2), data, false);
                                break;
                            default:
                                break;
                            }



                        }

                    }
                    if ("*" == symbol)
                    {
                        temp = v1 * v2;
                    }
                    else if ("/" == symbol)
                    {
                        if (v2 != 0)
                            temp = v1 / v2;
                    }
                    else if ("+" == symbol)
                    {

                        temp = v1 + v2;
                    }
                    else if ("-" == symbol)
                    {

                        temp = v1 - v2;
                    }
                }


            }
            else
            {
                switch (m_CurrentModel.agreement)
                {
                case 0:
                    temp = MsgParser::intel_Parser(m_CurrentModel.cItem.at(i).pItem.at(m), data, false);
                    break;
                case 1:
                    temp = MsgParser::moto_Msb_Parser(m_CurrentModel.cItem.at(i).pItem.at(m), data, false);
                    break;
                case 2:
                    temp = MsgParser::moto_Lsb_Parser(m_CurrentModel.cItem.at(i).pItem.at(m), data, false);
                    break;
                default:
                    break;
                }
            }

            pd.name = m_CurrentModel.cItem.at(i).pItem.at(m).bitName;
            pd.value = temp;
            pd.toWord = QString::number(temp);
            pd.color.r = 255;
            pd.color.g = 255;
            pd.color.b = 255;
            std::vector<cellProperty>& ss = m_CurrentModel.cItem.at(i).pItem.at(m).stl_itemProperty;
            //标准值
            //int stdddd = 0;
            for (int i = 0; i < ss.size(); i++)
            {
                pd.color.r = 0xFF;
                pd.color.g = 0xB9;
                pd.color.b = 0x0F;
                /*if (ss.at(i).isStand)
                {
                    stdddd = ss.at(i).value.toInt();
                }*/
                if (ss.at(i).value.toInt() == temp)
                {
                    pd.color.r = ss.at(i).r;
                    pd.color.g = ss.at(i).g;
                    pd.color.b = ss.at(i).b;
                    pd.toWord = ss.at(i).toWord;
                    break;
                }

            }
            parseArr.push_back(pd);
        }
        m_readwrite_lock.lockForWrite();
        int i_index = YB::idNameInVector(showTableVec, QString::number(fream_id));
        if (i_index >= 0)
        {
            showTableVec.at(i_index).Pdata = parseArr;
        }
        else
        {
            showTableData temp;
            temp.IdName = QString::number(fream_id);
            temp.Pdata = parseArr;
            showTableVec.push_back(temp);
        }
        m_readwrite_lock.unlock();
    }
    
    //emit sigNewMsg(showTableVec);
   // emit sigNewMsg();
    //setintoTable(showTableVec);
}
unsigned char ReceiveTableManage::checksumXOR(const uchar data[])
{
    return (data[1] ^ data[2] ^ data[3] ^ data[4] ^ data[5] ^ data[6] ^ data[7]);
}
void ReceiveTableManage::on_currentModelIndex_Changed(int index)
{
    this->currentModelIndex = index;
    this->recDataIntoTab();
}
void ReceiveTableManage::onStopShow()
{
    if (this->m_TimerShow)
    {
        this->m_TimerShow->stop();
    }
}