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
#include <qsettings.h>
#include "QsLog.h"
//静态成员变量在类外初始化
qGboleData* qGboleData::pGboleInstance = nullptr;
qGboleData::qGboleData()
{
	//这个作为保存到json文件时的json对象的序号，因为json是无序的
	//为了避免他自动排序，搞了一万个升序序号来人为制造顺序
	//这里只搞了10000个，所以当保存的项超过10000个时，就是越界。哈哈哈哈哈哈
	for (int i = 10000; i < 20000; ++i)
		fIndex[i - 10000] = i;
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
		mItem.insert("bundRate", pGboleData.at(i).bundRate);
		mItem.insert("circle", pGboleData.at(i).circle);
		mItem.insert("bStandardId", pGboleData.at(i).bStandardId);
		//mItem.insert("iPlatform", pGboleData.at(i).iPlatform);
		mItem.insert("sPlatform", pGboleData.at(i).sPlatform);
		QJsonObject cItemObj;
		for (int j = 0; j < pGboleData.at(i).cItem.size(); j++)
		{
			QJsonObject pItemObj;
			pItemObj.insert("CanId", pGboleData.at(i).cItem.at(j).strCanId);
			pItemObj.insert("opt", pGboleData.at(i).cItem.at(j).opt);
			pItemObj.insert("isSend", pGboleData.at(i).cItem.at(j).isSend);
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
				pItem.append(pGboleData.at(i).cItem.at(j).pItem.at(k).isRoll);
				pItem.append(pGboleData.at(i).cItem.at(j).pItem.at(k).dataFrom);
				
				//std::map<QString, cellProperty>& cpro = pGboleData.at(i).cItem.at(j).pItem.at(k).itemProperty;
				std::vector<cellProperty>& sspro = pGboleData.at(i).cItem.at(j).pItem.at(k).stl_itemProperty;
				//用来存储属性对象的
				QJsonObject cellPr;
				/*std::map<QString, cellProperty>::iterator ibegin = cpro.begin();
				std::map<QString, cellProperty>::iterator iend = cpro.end();*/
				//while(ibegin!=iend)
				for(int v=0;v< sspro.size();v++)
				{
					QJsonArray itemarr;
					itemarr.append(sspro.at(v).value);		//0
					itemarr.append(sspro.at(v).toWord);		//1
					uint16_t red = sspro.at(v).r;
					uint16_t green = sspro.at(v).g;
					uint16_t blue = sspro.at(v).b;
					bool isStand = sspro.at(v).isStand;
					itemarr.append(QString::number(red));	//2
					itemarr.append(QString::number(green));	//3
					itemarr.append(QString::number(blue));	//4
					itemarr.append(isStand);				//5
					//把first当key，value是一个数组
					cellPr.insert(QString::number(fIndex[v]), itemarr);
					//ibegin++;
				}
				//把对象追加到数组后面，数据也能包含对象
				pItem.append(cellPr);
				pItem.append(pGboleData.at(i).cItem.at(j).pItem.at(k).octhex);
				pDItem.insert(QString::number(fIndex[k]), pItem);
			}
			pItemObj.insert("pDItem", pDItem);
			cItemObj.insert(QString::number(fIndex[j]), pItemObj);
		}
		mItem.insert("mDItem", cItemObj);
		mItemObjRoot.insert(QString::number(fIndex[i]), mItem);
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
		//第一层表格，也就是型号层
		protoData ptem;
		//对象就是用{}括起来的
		if (rootObj[keyList.at(i)].type() != QJsonValue::Object)
			continue;
		//转为对象
		QJsonObject rootSecond = rootObj[keyList.at(i)].toObject();
		//把对象下面的几个值取出来
		ptem.agreement = rootSecond["agreement"].toInt(0);
		ptem.modelName = rootSecond["modelName"].toString();
		ptem.bundRate = rootSecond["bundRate"].toInt(0);
		ptem.circle = rootSecond["circle"].toInt(1000);
		ptem.bStandardId = rootSecond["bStandardId"].toBool(0);
		//ptem.iPlatform = rootSecond["iPlatform"].toInt(0);
		ptem.sPlatform = rootSecond["sPlatform"].toString("Non");
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
				//第二层表格，也就是ID层，一个ID下面有好多返回的字段值
				canIdData ctemp;
				//ID，也就是CAN设备的报文地址
				ctemp.strCanId = rootfour["CanId"].toString();
				//opt就是表明该ID是发送报文还是接收报文，1是接收，0是发送
				ctemp.opt = rootfour["opt"].toInt(0);
				ctemp.isSend = rootfour["isSend"].toBool(false);
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
						if (jarr.size() < 9)
							continue;
						protoItem dtemp;
						dtemp.bitName = jarr[0].toString();
						dtemp.startByte = jarr[1].toInt();
						dtemp.startBit = jarr[2].toInt();
						dtemp.bitLeng = jarr[3].toInt();
						dtemp.precision = jarr[4].toDouble();
						dtemp.offset = jarr[5].toInt();
						dtemp.isRoll = jarr[6].toBool();
						dtemp.dataFrom = jarr[7].toString();
						dtemp.octhex = jarr[9].toBool();
						dtemp.send = 0;
						//这个数据结构，主要是用来根据返回的值，显示不同的颜色
						if (jarr[8].isObject())
						{
							QJsonObject cProperty = jarr[8].toObject();
							QStringList keyListFift = cProperty.keys();
							//{返回值:["名称",r,g,b]}
							for (int h = 0; h < keyListFift.size(); ++h)
							{
								if (cProperty[keyListFift.at(h)].isArray())
								{
									cellProperty myCellP;
									QJsonArray cpArr = cProperty[keyListFift.at(h)].toArray();
									if (cpArr.size() < 5)
										continue;
									//先转换成QString，不能直接转换int
									QString sr = cpArr[2].toString();
									QString sg = cpArr[3].toString();
									QString sb = cpArr[4].toString();
									//使用QString转换int才正确的
									myCellP.r = sr.toInt();
									myCellP.g = sg.toInt();
									myCellP.b = sb.toInt();
									myCellP.toWord = cpArr[1].toString();
									myCellP.value = cpArr[0].toString();
									myCellP.isStand = cpArr[5].toBool();
									//dtemp.itemProperty.insert({ keyListFift.at(h),myCellP });
									dtemp.stl_itemProperty.push_back(myCellP);
								}
								
							}
						}
						ctemp.pItem.push_back(dtemp);
					}
				}
				//{canID vector}
				ptem.cItem.push_back(ctemp);

			}
		}
		//{型号vetor}
		pGboleData.push_back(ptem);
	}
	isInit = true;
}
void qGboleData::save_ini()
{
	std::lock_guard<std::mutex>lock(dMut);
	// 程序目录下Data文件，名字为Jugde.ini
	QString filepath = QApplication::applicationDirPath() + "/Data/Jugde.ini";
	QSettings* setf = new QSettings(filepath, QSettings::IniFormat);

	setf->setValue("lowVolt", aTdata.m_iLowVolt);
	setf->setValue("highVolt", aTdata.m_iHeightVolt);
	setf->setValue("m_iVoltStep", aTdata.m_iVoltStep);
	setf->setValue("m_iVoltError", aTdata.m_iVoltError);
	setf->setValue("m_iHeatTempture", aTdata.m_iHeatTempture);
	setf->setValue("m_iPowerTempture", aTdata.m_iPowerTempture);
	setf->setValue("m_iOverTime", aTdata.m_iOverTime);
	setf->setValue("m_sInWebAddr", aTdata.m_sInWebAddr);
	setf->setValue("m_sOutWebAddr", aTdata.m_sOutWebAddr);
	

}
void qGboleData::read_ini()
{
	std::lock_guard<std::mutex>lock(dMut);
	QString filepath = QApplication::applicationDirPath() + "/Data/Jugde.ini";
	QFile file(filepath);
	if (!file.exists())
	{
		//不存在就创建一个
		save_ini();
		QLOG_INFO() << "Jugde.ini not exits before,but now was created";
		return;
	}
	QSettings* setf = new QSettings(filepath, QSettings::IniFormat);
	if (setf->status() != QSettings::NoError)
	{
		QLOG_INFO() << "Open ini file error:" << setf->status();
		return;
	}
	aTdata.m_iLowVolt = setf->value("lowVolt").toInt();
	aTdata.m_iHeightVolt = setf->value("highVolt").toInt();
	aTdata.m_iHeatTempture = setf->value("m_iHeatTempture").toInt();
	aTdata.m_iPowerTempture = setf->value("m_iPowerTempture").toInt();
	aTdata.m_iOverTime = setf->value("m_iOverTime").toInt();
	aTdata.m_iVoltError = setf->value("m_iVoltError").toInt();
	aTdata.m_iVoltStep = setf->value("m_iVoltStep").toInt();
	aTdata.m_sInWebAddr = setf->value("m_sInWebAddr").toString();
	aTdata.m_sOutWebAddr = setf->value("m_sOutWebAddr").toString();
	aTdata.m_iRecOnNoSend = setf->value("m_iRecOnNoSend").toInt();
	aTdata.m_iShowType = setf->value("m_iShowType").toInt();
}
qGboleData::~qGboleData()
{
	if (pGboleInstance)
	{
		delete pGboleInstance;
		pGboleInstance = nullptr;
	}
}
