#include "SysSetting.h"
#include <QString>
#include <QFile>
#include <QSettings>

SysSetting::SysSetting(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	iniPath = QApplication::applicationDirPath() + "/Data/syssetting.ini";
	readIni(iniPath);
	connect(this->ui.tableWidget, &QTableWidget::itemChanged, this, &SysSetting::on_tableWidgetItemChang);
	
}

SysSetting::~SysSetting()
{

}
bool SysSetting::readIni(QString path)
{
	QFile f(path);
	if (!f.exists())
	{
		//technologyPW = "1234";
		//adminPW = "kus@1234";
		return false;
	}
	std::shared_ptr<QSettings>s = std::make_shared<QSettings>(path, QSettings::IniFormat);//   new QSettings();
	m_sSavePath = s->value("m_sSavePath").toString();
	m_sPassword = s->value("m_sPassword").toString();
	m_iAutoSaveNum = s->value("m_iAutoSaveNum").toInt();
	m_bIsAutoSave = s->value("m_bIsAutoSave").toInt();
	auto it = this->ui.tableWidget->item(0, 1);
	if (it)it->setText(QString::number(m_iAutoSaveNum));
	auto it2 = this->ui.tableWidget->item(2, 1);
	if (it2)it2->setText(QString::number(m_bIsAutoSave));
	return true;
}
bool SysSetting::saveIni(QString path, QString name, QVariant var)
{
	QFile f(path);
	if (!f.exists())
	{
		return false;
	}
	std::shared_ptr<QSettings>s = std::make_shared<QSettings>(path, QSettings::IniFormat);
	s->setValue(name, var);
	return false;
}
void SysSetting::on_tableWidgetItemChang(QTableWidgetItem* item)
{
	int c = item->column();
	if (c != 1)return;
	int r = item->row();
	switch (r)
	{
	case 0:
	{
		QString text = item->text();
		this->saveIni(iniPath, "m_iAutoSaveNum", QVariant(text.toInt()));
		
	}
		break;
	case 2:
	{
		QString text = item->text();
		this->saveIni(iniPath, "m_bIsAutoSave", QVariant(text.toInt()));

	}
	break;
	default:
		break;
	}
}
