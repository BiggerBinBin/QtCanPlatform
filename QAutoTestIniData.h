#pragma once

#include <QWidget>
#include "ui_QAutoTestIniData.h"

class QAutoTestIniData : public QWidget
{
	Q_OBJECT

public:
	QAutoTestIniData(QWidget *parent = nullptr);
	~QAutoTestIniData();

private:
	Ui::QAutoTestIniDataClass ui;
	void InitUi();
private slots:
	void on_pbSaveAData_clicked();
};
