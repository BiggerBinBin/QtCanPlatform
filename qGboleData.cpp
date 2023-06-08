#include "qGboleData.h"
#include <qapplication.h>
#include <qmessagebox.h>
#include <QDir>
#include <QJsonDocument>	// ����Json
#include <QJsonArray>		// [ ]
#include <QJsonObject>		// { }
#include <QJsonValue>		// int float double null { } [ ]
#include <QJsonParseError>
#include <QTextStream>
#include <QString>
#include <qobject.h>
#include <qsettings.h>
#include "QsLog.h"
//��̬��Ա�����������ʼ��
qGboleData* qGboleData::pGboleInstance = nullptr;
qGboleData::qGboleData()
{
	//�����Ϊ���浽json�ļ�ʱ��json�������ţ���Ϊjson�������
	//Ϊ�˱������Զ����򣬸���һ��������������Ϊ����˳��
	//����ֻ����10000�������Ե���������10000��ʱ������Խ�硣������������
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
			QMessageBox::warning(NULL, QObject::tr("warning"), QObject::tr("��������Ŀ¼ʧ��"));
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
		mItem.insert("m_iEnableInLine", pGboleData.at(i).ats.m_iEnableInLine);
		mItem.insert("m_iEnOp", pGboleData.at(i).ats.m_iEnOp);
		mItem.insert("m_iPowerInLine", pGboleData.at(i).ats.m_iPowerInLine);
		mItem.insert("m_iLowVoltage", pGboleData.at(i).ats.m_iLowVoltage);
		mItem.insert("m_iLowVoltageRe", pGboleData.at(i).ats.m_iLowVoltageRe);
		mItem.insert("m_iOverVoltage", pGboleData.at(i).ats.m_iOverVoltage);
		mItem.insert("m_iOverVoltageRe", pGboleData.at(i).ats.m_iOverVoltageRe);
		mItem.insert("m_iOverTemperature", pGboleData.at(i).ats.m_iOverTemperature);
		mItem.insert("m_iOverTempRe", pGboleData.at(i).ats.m_iOverTempRe);
		mItem.insert("m_iOverTempTolerance", pGboleData.at(i).ats.m_iOverTempTolerance);
		mItem.insert("m_fRequirePW", pGboleData.at(i).ats.m_fRequirePW);
		mItem.insert("m_fRatedPW", pGboleData.at(i).ats.m_fRatedPW);
		mItem.insert("m_iRatedVolt", pGboleData.at(i).ats.m_iRatedVolt);
		mItem.insert("m_strTolerance", pGboleData.at(i).ats.m_strTolerance);
		mItem.insert("m_strVer", pGboleData.at(i).ats.m_strVer);
		mItem.insert("m_strVerRecID", pGboleData.at(i).ats.m_strVerRecID);
		mItem.insert("m_usIDBytelen", pGboleData.at(i).ats.m_usIDBytelen);
		mItem.insert("m_usIDByteStartbit", pGboleData.at(i).ats.m_usIDByteStartbit);
		mItem.insert("m_usRatedPWTemp", pGboleData.at(i).ats.m_usRatedPWTemp);
		mItem.insert("m_usRatedPWFlow", pGboleData.at(i).ats.m_usRatedPWFlow);
		mItem.insert("m_usHeatTemp", pGboleData.at(i).ats.m_usHeatTemp);
		mItem.insert("m_usCoolTemp", pGboleData.at(i).ats.m_usCoolTemp);
		mItem.insert("m_bTurnOffFlow", pGboleData.at(i).ats.m_bTurnOffFlow);
		mItem.insert("m_bTurnOffCool", pGboleData.at(i).ats.m_bTurnOffCool);
		mItem.insert("m_iOutTempStop", pGboleData.at(i).ats.m_iOutTempStop);



		QJsonObject cItemObj;
		for (int j = 0; j < pGboleData.at(i).cItem.size(); j++)
		{
			QJsonObject pItemObj;
			pItemObj.insert("CanId", pGboleData.at(i).cItem.at(j).strCanId);
			pItemObj.insert("opt", pGboleData.at(i).cItem.at(j).opt);
			pItemObj.insert("isSend", pGboleData.at(i).cItem.at(j).isSend);
			pItemObj.insert("len", pGboleData.at(i).cItem.at(j).len);
			pItemObj.insert("circle", pGboleData.at(i).cItem.at(j).circle);
			QJsonObject pDItem;
			//pItem��һ������������
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
				//�����洢���Զ����
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
					//��first��key��value��һ������
					cellPr.insert(QString::number(fIndex[v]), itemarr);
					//ibegin++;
				}
				//�Ѷ���׷�ӵ�������棬����Ҳ�ܰ�������
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
	// ��json�����������ת��Ϊ�ַ���
	QJsonDocument doc;
	// ��object����λ���ĵ�����Ҫ����
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
	//QFile ���Ի�ȡ�ļ��Ǵ���
	QFile file(appPath);
	if (!file.exists(appPath))
		return;
	//���ļ���ֻ��ģʽ�����ʧ�ܣ���return
	if (!file.open(QFile::ReadOnly | QFile::Truncate))
	{
		QMessageBox::warning(NULL, QObject::tr("warning"), QObject::tr("���ݲ����ڣ���ʼ������ʧ��"));
		return;
	}
	//��text�������ڴ���
	QTextStream readStrem(&file);
	//���ø�ʽΪutf-8
	readStrem.setCodec("UTF-8");
	//��ȡ����
	QString str = readStrem.readAll();
	//����֮��ǵ�Ҫclose����ֹ�����쳣�˳���������
	file.close();
	QJsonParseError jsonError;
	//����Ϊjson�ĵ�
	QJsonDocument doc = QJsonDocument::fromJson(str.toUtf8(), &jsonError);
	if (jsonError.error != QJsonParseError::NoError)
	{
		QMessageBox::warning(NULL, QObject::tr("warning"), QString(QObject::tr("����jsonʧ�ܣ�") + jsonError.errorString()));
		return;
	}
	//ת����json object
	QJsonObject rootObj = doc.object();
	//��ȡ����key
	QStringList keyList = rootObj.keys();
	//����key���б���
	for (int i = 0; i < keyList.size(); i++)
	{
		//��һ����Ҳ�����ͺŲ�
		protoData ptem;
		//���������{}��������
		if (rootObj[keyList.at(i)].type() != QJsonValue::Object)
			continue;
		//תΪ����
		QJsonObject rootSecond = rootObj[keyList.at(i)].toObject();
		//�Ѷ�������ļ���ֵȡ����
		ptem.agreement = rootSecond["agreement"].toInt(0);
		ptem.modelName = rootSecond["modelName"].toString();
		ptem.bundRate = rootSecond["bundRate"].toInt(0);
		ptem.circle = rootSecond["circle"].toInt(1000);
		ptem.bStandardId = rootSecond["bStandardId"].toBool(0);
		//ptem.iPlatform = rootSecond["iPlatform"].toInt(0);
		ptem.sPlatform = rootSecond["sPlatform"].toString("Non");

		ptem.ats.m_iEnableInLine = rootSecond["m_iEnableInLine"].toInt(0);
		ptem.ats.m_iPowerInLine = rootSecond["m_iPowerInLine"].toInt(0);
		ptem.ats.m_iLowVoltage = rootSecond["m_iLowVoltage"].toInt(0);
		ptem.ats.m_iLowVoltageRe = rootSecond["m_iLowVoltageRe"].toInt(0);
		ptem.ats.m_iOverVoltage = rootSecond["m_iOverVoltage"].toInt(0);
		ptem.ats.m_iOverVoltageRe = rootSecond["m_iOverVoltageRe"].toInt(0);
		ptem.ats.m_iOverTemperature = rootSecond["m_iOverTemperature"].toInt(0);
		ptem.ats.m_iOverTempRe = rootSecond["m_iOverTempRe"].toInt(0);
		ptem.ats.m_iOverTempTolerance = rootSecond["m_iOverTempTolerance"].toInt(0);
		ptem.ats.m_iEnOp = rootSecond["m_iEnOp"].toInt(1);
		ptem.ats.m_fRatedPW = rootSecond["m_fRatedPW"].toInt(7000);
		ptem.ats.m_fRequirePW = rootSecond["m_fRequirePW"].toInt(100);
		ptem.ats.m_iRatedVolt = rootSecond["m_iRatedVolt"].toInt(600);
		ptem.ats.m_strTolerance = rootSecond["m_strTolerance"].toString("+10;-5");
		ptem.ats.m_strVer = rootSecond["m_strVer"].toString("Non");
		ptem.ats.m_strVerRecID = rootSecond["m_strVerRecID"].toString("18AD8DE4");
		ptem.ats.m_usIDBytelen = rootSecond["m_usIDBytelen"].toInt(4);
		ptem.ats.m_usIDByteStartbit = rootSecond["m_usIDByteStartbit"].toInt(8);
		ptem.ats.m_usRatedPWTemp = rootSecond["m_usRatedPWTemp"].toInt(0);
		ptem.ats.m_usRatedPWFlow = rootSecond["m_usRatedPWFlow"].toInt(10);
		ptem.ats.m_usHeatTemp = rootSecond["m_usHeatTemp"].toInt(-15);
		ptem.ats.m_usCoolTemp = rootSecond["m_usCoolTemp"].toInt(-20);
		ptem.ats.m_bTurnOffFlow = rootSecond["m_bTurnOffFlow"].toInt(1);
		ptem.ats.m_bTurnOffCool = rootSecond["m_bTurnOffCool"].toInt(0);
		ptem.ats.m_iOutTempStop = rootSecond["m_iOutTempStop"].toInt(50);


		//�������Ƕ�׶���
		if (rootSecond["mDItem"].type() == QJsonValue::Object)
		{
			//��mDItem��Value��Ϊ���󣬼�����������{}��������
			QJsonObject rootthird = rootSecond["mDItem"].toObject();
			QStringList keyListThird = rootthird.keys();
			for (int j = 0; j < keyListThird.size(); j++)
			{
				if (rootthird[keyListThird.at(j)].type() != QJsonValue::Object)
					continue;
				//mDitem����Ҳ�Ƕ��󣬶�Ӧ�����ݽṹ��struct canIdData,std::vector<canIdData>cItem;
				QJsonObject rootfour = rootthird[keyListThird.at(j)].toObject();
				//�ڶ�����Ҳ����ID�㣬һ��ID�����кö෵�ص��ֶ�ֵ
				canIdData ctemp;
				//ID��Ҳ����CAN�豸�ı��ĵ�ַ
				ctemp.strCanId = rootfour["CanId"].toString();
				//opt���Ǳ�����ID�Ƿ��ͱ��Ļ��ǽ��ձ��ģ�1�ǽ��գ�0�Ƿ���
				ctemp.opt = rootfour["opt"].toInt(0);
				ctemp.isSend = rootfour["isSend"].toBool(false);
				ctemp.len = rootfour["len"].toInt(8);
				ctemp.circle = rootfour["circle"].toInt(-1);
				if (rootfour["pDItem"].isObject())
				{
					//pDitem��Ӧ����std::vector<struct protoItem>pItem;
					QJsonObject rootfift = rootfour["pDItem"].toObject();
					QStringList keyListFour = rootfift.keys();
					for (int k = 0; k < keyListFour.size(); k++)
					{
						if (!rootfift[keyListFour.at(k)].isArray())
							continue;
						//��Ӧ struct protoItem
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
						//������ݽṹ����Ҫ���������ݷ��ص�ֵ����ʾ��ͬ����ɫ
						if (jarr[8].isObject())
						{
							QJsonObject cProperty = jarr[8].toObject();
							QStringList keyListFift = cProperty.keys();
							//{����ֵ:["����",r,g,b]}
							for (int h = 0; h < keyListFift.size(); ++h)
							{
								if (cProperty[keyListFift.at(h)].isArray())
								{
									cellProperty myCellP;
									QJsonArray cpArr = cProperty[keyListFift.at(h)].toArray();
									if (cpArr.size() < 5)
										continue;
									//��ת����QString������ֱ��ת��int
									QString sr = cpArr[2].toString();
									QString sg = cpArr[3].toString();
									QString sb = cpArr[4].toString();
									//ʹ��QStringת��int����ȷ��
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
		//{�ͺ�vetor}
		pGboleData.push_back(ptem);
	}
	isInit = true;
}
void qGboleData::save_ini()
{
	std::lock_guard<std::mutex>lock(dMut);
	// ����Ŀ¼��Data�ļ�������ΪJugde.ini
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
		//�����ھʹ���һ��
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
	aTdata.m_iSaveListNum = setf->value("m_iSaveListNum").toInt();
}
qGboleData::~qGboleData()
{
	if (pGboleInstance)
	{
		delete pGboleInstance;
		pGboleInstance = nullptr;
	}
}
