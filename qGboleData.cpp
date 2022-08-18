#include "qGboleData.h"
#include <qapplication.h>
#include <qmessagebox.h>
#include <QDir>
#include <QJsonDocument>	// 解析Json
#include <QJsonArray>		// [ ]
#include <QJsonObject>		// { }
#include <QJsonValue>		// int float double null { } [ ]
#include <QJsonParseError>
#include <QTextStream>
#include <QString>
#include <qobject.h>
qGboleData* qGboleData::pGboleInstance = nullptr;
qGboleData::qGboleData()
{

}

void qGboleData::save()
{
	std::lock_guard<std::mutex>lock(dMut);
	QString appPath = QApplication::applicationDirPath() + "/Data";
	QDir dir(appPath);
	if (!dir.exists())
	{
		if (!dir.mkpath(appPath))
		{
			QMessageBox::warning(NULL, QObject::tr("warning"), QObject::tr("创建保存目录失败"));
			return;
		}
	}

	QJsonObject mItemObjRoot;
	for (int i = 0; i < pGboleData.size(); i++)
	{

		QJsonObject mItem;
		mItem.insert("modelName", pGboleData.at(i).modelName);
		mItem.insert("agreement", pGboleData.at(i).agreement);
		QJsonObject cItemObj;
		for (int j = 0; j < pGboleData.at(i).cItem.size(); j++)
		{
			QJsonObject pItemObj;
			pItemObj.insert("CanId", pGboleData.at(i).cItem.at(j).CanId);
			pItemObj.insert("opt", pGboleData.at(i).cItem.at(j).opt);
			QJsonObject pDItem;
			//pItem用一个数组来储存
			for (int k = 0; k < pGboleData.at(i).cItem.at(j).pItem.size(); k++)
			{
				QJsonArray pItem;
				pItem.append(pGboleData.at(i).cItem.at(j).pItem.at(k).bitName);
				pItem.append(pGboleData.at(i).cItem.at(j).pItem.at(k).startByte);
				pItem.append(pGboleData.at(i).cItem.at(j).pItem.at(k).startBit);
				pItem.append(pGboleData.at(i).cItem.at(j).pItem.at(k).bitLeng);
				pItem.append(pGboleData.at(i).cItem.at(j).pItem.at(k).precision);
				pItem.append(pGboleData.at(i).cItem.at(j).pItem.at(k).offset);
				pDItem.insert(QString::number(k), pItem);
			}
			pItemObj.insert("pDItem", pDItem);
			cItemObj.insert(QString::number(j), pItemObj);
		}
		mItem.insert("mDItem", cItemObj);
		mItemObjRoot.insert(QString::number(i), mItem);
	}
	// 将json对象里的数据转换为字符串
	QJsonDocument doc;
	// 将object设置位本文档的主要对象
	doc.setObject(mItemObjRoot);
	appPath += "/appData.json";
	QFile file(appPath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		return;
	}
	QTextStream stream(&file);
	stream.setCodec("UTF-8");
	stream << doc.toJson();
	file.close();
}

void qGboleData::read()
{
	std::lock_guard<std::mutex>lock(dMut);
	QString appPath = QApplication::applicationDirPath() + "/Data/appData.json";
	//QFile 可以获取文件是存在
	QFile file(appPath);
	if (!file.exists(appPath))
		return;
	//打开文件，只读模式，如果失败，就return
	if (!file.open(QFile::ReadOnly | QFile::Truncate))
	{
		QMessageBox::warning(NULL, QObject::tr("warning"), QObject::tr("数据不存在，初始化数据失败"));
		return;
	}
	//用text流读到内存中
	QTextStream readStrem(&file);
	//设置格式为utf-8
	readStrem.setCodec("UTF-8");
	//读取所有
	QString str = readStrem.readAll();
	//读完之后记得要close，防止程序异常退出，损坏数据
	file.close();
	QJsonParseError jsonError;
	//解析为json文档
	QJsonDocument doc = QJsonDocument::fromJson(str.toUtf8(), &jsonError);
	if (jsonError.error != QJsonParseError::NoError)
	{
		QMessageBox::warning(NULL, QObject::tr("warning"), QString(QObject::tr("解析json失败：") + jsonError.errorString()));
		return;
	}
	//转换成json object
	QJsonObject rootObj = doc.object();
	//获取所有key
	QStringList keyList = rootObj.keys();
	//根据key进行遍历
	for (int i = 0; i < keyList.size(); i++)
	{
		protoData ptem;
		//对象就是用{}括起来的
		if (rootObj[keyList.at(i)].type() != QJsonValue::Object)
			continue;
		//转为对象
		QJsonObject rootSecond = rootObj[keyList.at(i)].toObject();
		//把对象下面的几个值取出来
		ptem.agreement = rootSecond["agreement"].toInt(0);
		ptem.modelName = rootSecond["modelName"].toString();
		//如果还有嵌套对象
		if (rootSecond["mDItem"].type() == QJsonValue::Object)
		{
			//把mDItem的Value变为对象，即它是用括号{}括起来的
			QJsonObject rootthird = rootSecond["mDItem"].toObject();
			QStringList keyListThird = rootthird.keys();
			for (int j = 0; j < keyListThird.size(); j++)
			{
				if (rootthird[keyListThird.at(j)].type() != QJsonValue::Object)
					continue;
				//mDitem本身也是对象，对应的数据结构是struct canIdData,std::vector<canIdData>cItem;
				QJsonObject rootfour = rootthird[keyListThird.at(j)].toObject();
				canIdData ctemp;
				ctemp.CanId = rootfour["CanId"].toInt(0);
				ctemp.opt = rootfour["opt"].toInt(0);
				if (rootfour["pDItem"].isObject())
				{
					//pDitem对应的是std::vector<struct protoItem>pItem;
					QJsonObject rootfift = rootfour["pDItem"].toObject();
					QStringList keyListFour = rootfift.keys();
					for (int k = 0; k < keyListFour.size(); k++)
					{
						if (!rootfift[keyListFour.at(k)].isArray())
							continue;
						//对应 struct protoItem
						QJsonArray jarr = rootfift[keyListFour.at(k)].toArray();
						if (jarr.size() < 5)
							continue;
						protoItem dtemp;
						dtemp.bitName = jarr[0].toString();
						dtemp.startByte = jarr[1].toInt();
						dtemp.startBit = jarr[2].toInt();
						dtemp.bitLeng = jarr[3].toInt();
						dtemp.precision = jarr[4].toInt();
						dtemp.offset = jarr[5].toInt();
						ctemp.pItem.push_back(dtemp);
					}
				}

				ptem.cItem.push_back(ctemp);

			}
		}
		pGboleData.push_back(ptem);
	}
	isInit = true;
}

qGboleData::~qGboleData()
{
	if (pGboleInstance)
	{
		delete pGboleInstance;
		pGboleInstance = nullptr;
	}
}
