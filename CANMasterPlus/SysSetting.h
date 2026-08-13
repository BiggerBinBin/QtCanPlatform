#pragma once

#include <QDialog>
#include "ui_SysSetting.h"

class SysSetting : public QDialog
{
	Q_OBJECT

public:
	SysSetting(QWidget *parent = nullptr);
	~SysSetting();

private:
	Ui::SysSettingClass ui;
	//void InitTable();
	int m_iAutoSaveNum = 3000;
	int m_bIsAutoSave = 1;
	QString m_sSavePath = "";
	QString m_sPassword = "";
	QString iniPath;
private:
	bool readIni(QString path);
	bool saveIni(QString path, QString name, QVariant var);
private Q_SLOTS:
	void on_tableWidgetItemChang(QTableWidgetItem* item);
};
