/*
 * qGboleData.h - header file of qGboleData class
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
#include <QObject>
#include <thread>
#include<Mutex>
#include "dataStruct.h"
class qGboleData
{

public:
	//qGboleData(QObject *parent);
	static qGboleData* getInstance()
	{
		//std::lock_guard<std::mutex>lock(mut);
		if (pGboleInstance == nullptr)
		{
			pGboleInstance = new qGboleData();
			return pGboleInstance;
		}
		else
		{
			return pGboleInstance;
		}
	}
	void save();
	void read();
	std::vector<struct protoData>pGboleData;
	bool getIsInit() { return isInit; }
	
private:
	bool isInit = false;
	~qGboleData();
	qGboleData();
	static qGboleData* pGboleInstance;
	//static std::mutex mut;
	std::mutex dMut;
	uint fIndex[10000];
signals:
	void dataChange();
};
