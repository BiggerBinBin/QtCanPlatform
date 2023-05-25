#pragma once

#include <QObject>

class CoolantAbstract  : public QObject
{
	Q_OBJECT

public:
	CoolantAbstract(QObject *parent);
	~CoolantAbstract();
};
