#include "Login.h"
#include <qsettings.h>
#include <qfile.h>
#pragma execution_character_set("utf-8")
Login::Login(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	this->setWindowModified(true);
	ui.lineEdit->setEchoMode(QLineEdit::EchoMode::Password);
	
	getInitPW(QApplication::applicationDirPath()+"/Data/syssetting.ini");
}

Login::~Login()
{

}
void Login::showEvent(QShowEvent* event)
{
	//ui.label_3->clear();
}
bool Login::getInitPW(QString path)
{
	QFile f(path);
	if (!f.exists())
	{
		technologyPW = "1234";
		adminPW = "kus@1234";
		return false;
	}
	QSettings *s = new QSettings(path, QSettings::IniFormat);
	technologyPW = s->value("technologyPW").toString();
	adminPW = s->value("adminPW").toString();
	return true;
}
void Login::on_pbLogout_clicked()
{
	this->setUserType(0);
	this->close();
}
void Login::on_pbLogin_clicked()
{
	QString pw = ui.lineEdit->text();
	int index = ui.comboBox->currentIndex();
	if (0 == index)
	{
		if (pw == technologyPW)
		{
			//emit sigLogin(0);
			//ui.lineEdit->clear();
			this->setUserType(1);
			ui.label_3->clear();
			this->close();
			
		}
		else
		{
			ui.label_3->setText(QString("<font color=\"#f90000\">ÃÜÂë´íÎó</font>"));
		}
	}
	else if (1 == index)
	{
		if (pw == adminPW)
		{
			this->setUserType(2);
			ui.label_3->clear();
			this->close();
		}
		else
		{
			ui.label_3->setText(QString("<font color=\"#f90000\">ÃÜÂë´íÎó</font>"));
		}
	}
}