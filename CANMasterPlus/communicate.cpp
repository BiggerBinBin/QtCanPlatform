#include <QPushButton>
#include <QJsonObject>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QDebug>
#include <QJsonArray>
#include <QApplication>
#include "communicate.h"
#include "../DeviceIO/RegisterDevClass.h"
#include "../DeviceIO/3rd/pcan/include/PCANBasic.h"

Communicate::Communicate(QObject *parent) : QObject(parent)
{
    //Using this static function register reflect class first
    RegisterDevReflect::RegisterReflect();
    QString path = QApplication::applicationDirPath()+"/Config/className.json";
    readDevName(path);
    m_sharedPtrDev.reset(static_cast<DeviceIO*>(CReflectClass<DeviceIO>::createObject(QByteArray(m_className.at(0).toUtf8()), nullptr)));
}

QStringList Communicate::setDevType(short int devtype)
{
    if (devtype >= m_className.size())
    {
        
        return QStringList();
    }
    m_sharedPtrDev.reset(static_cast<DeviceIO*>(CReflectClass<DeviceIO>::createObject(QByteArray(m_className.at(devtype).toUtf8()), nullptr)));
    return m_sharedPtrDev.data()->enumDevice();
}

bool Communicate::openDevice(const DeviceParameter &param)
{

    if(param.devType>= m_className.size())
    {
        return false;
    }
    QJsonObject obj;
    obj.insert("ch",param.devChannel);
    obj.insert("bundRate",param.baudRate);
    obj.insert("devModel",param.devType);
    if (param.devType == 8)
    {
        int i = 0;
        for (auto it=param.v.begin();it!=param.v.end();it++)
        {
            QString str = QString::number(i);
            //QString hexStr = QString::number(*it, 16);
            obj.insert("id"+str,*it);
            i++;
        }
    }

    m_sharedPtrDev.reset(static_cast<DeviceIO*>(CReflectClass<DeviceIO>::createObject(QByteArray(m_className.at(param.devType).toUtf8()),nullptr)));
    connect(m_sharedPtrDev.data(), &DeviceIO::sigNewMessage, this, &Communicate::on_recFromDevMsg);
    m_sharedPtrDev.data()->enumDevice();
    return m_sharedPtrDev.data()->openDevice(obj);

}

QStringList Communicate::devList()
{
    if (!m_sharedPtrDev.isNull())
        return m_sharedPtrDev.data()->enumDevice();
    else
        return QStringList();
}

void Communicate::closeDevice()
{
    disconnect(m_sharedPtrDev.data(), &DeviceIO::sigNewMessage, this, &Communicate::on_recFromDevMsg);
    if(!m_sharedPtrDev.isNull())
        m_sharedPtrDev->disConnectDev();
}

void Communicate::sendData(const ushort ch, const uint fream_id, uchar data[8], bool isStandard, int len, int dir)
{
    if (0 == dir)
        m_sharedPtrDev->sendData(ch, fream_id, data, isStandard, len, m_sharedPtrDev->Master);
    else
        m_sharedPtrDev->sendData(ch, fream_id, data, isStandard, len, m_sharedPtrDev->Slave);
}

bool Communicate::readDevName(QString path)
{
    QFile file(path);
    if(!file.exists())
    {
        return false;
    }
    if (!file.open(QIODevice::ReadOnly))
    {
        //QMessageBox::warning(NULL, QObject::tr("warning"), QObject::tr("数据不存在，初始化数据失败"));
        return false;
    }
    //Using text stream to read data
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    QString str = stream.readAll();
    QJsonParseError err;
    //string convert to QJsonDocument
    QJsonDocument jdoc = QJsonDocument::fromJson(str.toUtf8(),&err);
    //has something error
    if(err.error != QJsonParseError::NoError)
    {
        qDebug()<<"QJsonParseError: "<<err.errorString();
        return false;
    }

    QJsonObject jobj = jdoc.object();
    if(!jobj["className"].isArray())
        return false;

     QJsonArray jarr = jobj["className"].toArray();
     m_className.clear();
     for(int i=0;i<jarr.size();i++)
     {
         m_className.append(jarr.at(i).toString());
     }
     return true;
}
void Communicate::on_recFromDevMsg(int ch, unsigned int fream_id, QByteArray data)
{
    emit on_sendNewMsg(ch, fream_id, data);
}