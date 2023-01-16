/*
 * DbcParser.h - header file of DbcParser class
 *
 * Copyright (C) 2021-2022 lyb <liyangbinbin@foxmail.com>
 *
 * This file is part of the QtCanPlatform.
 *
 * QtCanPlatform is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * QtCanPlatform is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libQtShadowsocks; see the file LICENSE. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include "dataStruct.h"
#include <qstring.h>
#include "zdbc.h"
class DbcParser
{
public:
	DbcParser();
	~DbcParser();
	virtual struct protoData parserdbc(bool& res)=0;

//protected:
//	QString m_sPath;
//	int m_iParserTYpe;
};



class ParserZ:public DbcParser
{
public:
	ParserZ(QString dbcFileName);
	virtual struct protoData parserdbc(bool &res);
private:
	DBCHandle m_hDBC;
private:
	void AddMsgToList(DBCMessage msg, protoData& pData);
protected:
	QString m_sPath;
	int m_iParserTYpe;
};