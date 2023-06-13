#include "ImplementPower.h"

GersImplementPower::GersImplementPower(QObject *parent)
	: AbstractPower(parent)
{

}

GersImplementPower::~GersImplementPower()
{

}

bool GersImplementPower::openDevice(QVariant param)
{
	ParamMsg pm = param.value<ParamMsg>();
	return false;
}

bool GersImplementPower::outPut(QVariant param)
{
	return false;
}

bool GersImplementPower::stopOutPut(QVariant param)
{
	return false;
}

bool GersImplementPower::setOutParam(QVariant param)
{
	return false;
}
