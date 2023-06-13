#pragma once

#include "../Abstract/AbstractPower.h"


class GersImplementPower  : public AbstractPower
{
	Q_OBJECT

public:
	GersImplementPower(QObject *parent);
	~GersImplementPower();
	virtual bool openDevice(QVariant param) override;
	virtual bool outPut(QVariant param) override;
	virtual bool stopOutPut(QVariant param) override;
	virtual bool setOutParam(QVariant param) override;
};
