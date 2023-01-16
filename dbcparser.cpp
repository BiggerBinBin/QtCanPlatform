#include "dbcparser.h"

#pragma execution_character_set("utf-8")  
DbcParser::DbcParser()
{
}

DbcParser::~DbcParser()
{
}
ParserZ::ParserZ(QString dbcFileName)
{
	m_sPath = dbcFileName;
	m_hDBC = ZDBC_Init();
	
};
protoData ParserZ::parserdbc(bool &res)
{
	
	if (INVALID_DBC_HANDLE == m_hDBC)
	{
		res = false;
		return protoData();
	}
	FileInfo fileinfo;
	
	//string strFile = zy::W2AEx(m_sPath.toStdWString().c_str());
	std::string strFile = m_sPath.toLocal8Bit();
	strcpy_s(fileinfo.strFilePath, _MAX_FILE_PATH_, strFile.c_str());
	fileinfo.merge = false;
	if (!ZDBC_LoadFile(m_hDBC, &fileinfo))
	{
		res = false;
		return protoData();
	}
	if (0 == ZDBC_GetMessageCount(m_hDBC))
	{
		res = false;
		return protoData();
	}
	protoData pData;
	DBCMessage msg;
	wchar_t a[100];
	wcscpy(a,L"新型号");
	pData.modelName = QString::fromWCharArray(a);
	if (ZDBC_GetFirstMessage(m_hDBC, &msg))
	{
		pData.bStandardId = !msg.nExtend;
		pData.circle = msg.nCycleTime;
		if (msg.nSignalCount > 0)
			pData.agreement = msg.vSignals[0].is_motorola;
		else
			pData.agreement = 0;
		pData.bundRate = 1;
		pData.sPlatform = "Non";
		AddMsgToList(msg, pData);
		
		while (ZDBC_GetNextMessage(m_hDBC, &msg))
		{
			AddMsgToList(msg, pData);
		}
		res = true;
	}
	else
	{
		res = false;
	}
	return pData;
}


void ParserZ::AddMsgToList(DBCMessage msg, protoData& pData)
{
	canIdData cData;
	cData.strCanId = QString::number(msg.nID,16);
	cData.opt = 0;
	int len = msg.nSignalCount;
	
	for (int i = 0; i < len; ++i)
	{
		protoItem pItem;
		if (msg.vSignals[i].strComment != "")
		{
			
			pItem.bitName = QString::fromLocal8Bit(msg.vSignals[i].strComment);
		}	
		else
		{
			pItem.bitName = QString::fromLocal8Bit(msg.vSignals[i].strName);;
		}
		//zy::W2AEx((wchar_t*)msg.vSignals[i].strName);
		QString str = QString::fromLocal8Bit(msg.vSignals[i].unit);
		if (str.size()>0)
			pItem.bitName += "(" + str +")";
		pItem.startByte = msg.vSignals[i].nStartBit / 8;
		pItem.startBit = msg.vSignals[i].nStartBit;
		pItem.bitLeng = msg.vSignals[i].nLen;
		pItem.precision = msg.vSignals[i].nFactor;
		pItem.offset = msg.vSignals[i].nOffset;
		pItem.dataFrom = "-1";
		pItem.isRoll=false;
		pItem.octhex=false;
		pItem.send=0;
		int n = ZDBC_GetValDescPairCount(m_hDBC, msg.nID, msg.vSignals[i].strName);
		if (n > 0)
		{
			//值对含义的缓冲区
			ValDescPair* vp = new ValDescPair[n];
			//获取值对含义
			ZDBC_GetValDescPair(m_hDBC, msg.nID, msg.vSignals[i].strName, vp);
			for (int k = 0; k < n; k++)
			{
				cellProperty cp;
				if ((int)vp[k].value == 0)
				{
					cp.b = 255;
					cp.g = 255;
					cp.r = 255;
					cp.isStand = true;
				}
				else
				{
					cp.b = 0;
					cp.g = 0;
					cp.r = 255;
					cp.isStand = false;
				}
				
				cp.value = QString::number(vp[k].value);
				cp.toWord = QString::fromLocal8Bit(vp[k].desc);
				pItem.stl_itemProperty.push_back(cp);
			}
			delete[] vp;
			vp = nullptr;
		}
		else
		{
			pItem.isRoll = true;
		}
		cData.pItem.push_back(pItem);

	}
	pData.cItem.push_back(cData);
	
}
