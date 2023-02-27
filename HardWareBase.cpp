#include "HardWareBase.h"

HardWareBase::HardWareBase(QObject *parent)
	: QObject(parent)
{}

HardWareBase::~HardWareBase()
{}

bool HardWareBase::OpenHardWare(QString dev, int bundrate, int attach)
{
	return false;
}
bool HardWareBase::CloseHardWare()
{
	return false;
}
bool HardWareBase::SendMessage(uint id, uchar data[], int resever[])
{
	return false;
}

//bool HardWareBase::ReadMessage(uint id, uchar data[], int resever)
//{
//	return false;
//}

QStringList HardWareBase::GetHardWare()
{
	return QStringList();
}
