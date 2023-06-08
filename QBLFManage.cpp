#include "QBLFManage.h"
#define STRICT                         /* WIN32 */
#include <windows.h>
#include "binlog.h"                    /* BL    */
#include <tchar.h>                     /* RTL   */
#include <QFile>
#include <QTextStream>
#include <QtConcurrent>
# pragma execution_character_set("utf-8")
QStringList Mon = { "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sept","Oct","Nov","Dec" };
QStringList Week = { "Mon","Tue","Wed","Thur","Fri","Sat","Sun" };
QBLFManage::QBLFManage(LogBase* parent)
	: LogBase(parent)
{

}

QBLFManage::~QBLFManage()
{

}

bool QBLFManage::loadLogFile(const QString& file, const QStringList& fifter2, messageMange* LogData)
{
    m_sBlfName = file;
    fifter = fifter2;
    RunState = true;
    QtConcurrent::run(this, &QBLFManage::readBLF, LogData);
    isReadFile = true;
    return true;
}
//void QBLFManage::stopGetMes()
//{
//    if (m_tSendMes)
//    {
//        m_tSendMes->stop();
//    }
//    RunState = false;
//}
//void QBLFManage::startGetMes(const int circle, const QStringList idList)
//{
//    if (idList.size() <= 0)
//        return;
//    fifter = idList;
//    if (!m_tSendMes)
//    {
//        m_tSendMes = new QTimer();
//        connect(m_tSendMes, &QTimer::timeout, this, &QBLFManage::sendMessage);
//    }
//    m_tSendMes->start(circle);
//}

void QBLFManage::readBLF(messageMange* LogData)
{
    LogData->iIndex = 0;
    LogData->iLen = 0;
    LogData->vData.clear();
    QByteArray dd = m_sBlfName.toLocal8Bit();
    LPCSTR pFileName = dd.data();
    DWORD dwWritten;
    LONG64 sta = 0;
    double dTimeDiff = 0.0;

    {
        HANDLE hFile;
        VBLObjectHeaderBase base;
        VBLCANMessage message;
        VBLCANMessage2_t message2;
        VBLEnvironmentVariable variable;
        VBLEthernetFrame ethframe;
        VBLAppText appText;
        VBLFileStatisticsEx statistics = { sizeof(statistics) };
        BOOL bSuccess;
        /* open file */
        hFile = BLCreateFile(pFileName, GENERIC_READ);

        if (INVALID_HANDLE_VALUE == hFile)
        {
            emit sigLoadState(-1);
            return;
        }

        BLGetFileStatisticsEx(hFile, &statistics);
        bSuccess = TRUE;

        /* read base object header from file */
        while (RunState && bSuccess && BLPeekObject(hFile, &base))
        {
            message2.mHeader.mBase = base;
            bSuccess = BLReadObjectSecure(hFile, &message2.mHeader.mBase, sizeof(message2));
            
            if (8 != message2.mDLC)
                continue;
            if (bSuccess) {
                UINT64 id;
                //CANoe��������չ֡ID�ϼ���0x80000000�ģ�����Ҫ����һ��
                if (message2.mID > 0x80000000)
                    id = message2.mID & 0x7FFFFFFF;
                else
                    id = message2.mID;

                if (fifter.size() > 0)
                {
                    for (int i = 0; i < fifter.size(); i++)
                    {
                        UINT64 fid = fifter.at(i).toInt(NULL, 16);
                        if (id == fid)
                        {
                            messageData dd;
                            dd.Id = id;
                            //ֻ�������ʱ��,ms
                            double time = (message2.mHeader.mObjectTimeStamp - dTimeDiff)/ 1000000.0;
                            dd.TimeStemp = time;
                            //��һ�μ���һ�ε�
                            dTimeDiff = message2.mHeader.mObjectTimeStamp;
                            memcpy(dd.Data, message2.mData, 8);
                            LogData->vData.push_back(dd);
                            //����+1
                            LogData->iLen++;
                            break;
                        }
                    }
                }
                /* free memory for the CAN message */
                BLFreeObject(hFile, &message2.mHeader.mBase);
            }
        }

        if (!BLCloseHandle(hFile))
        {
            isReadFile = false;
            emit sigLoadState(0);
            return;
        }
        isReadFile = false;
        emit sigLoadState(0);
        
    }
}

void QBLFManage::sendMessage()
{
    /*if (LogData.vData.size() <= LogData.iIndex)
    {
        if(m_tSendMes)
            m_tSendMes->stop();
        return;
    }
    QByteArray data;
    
    for (int i = 0; i < 8; i++)
    {
        data.append(LogData.vData[LogData.iIndex].Data[i]);
    }
    emit sigNewMessage(LogData.vData[LogData.iIndex++].Id, data);*/
}
