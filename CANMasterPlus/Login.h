#pragma once

#include <QDialog>
#include "ui_Login.h"
class Login : public QDialog
{
	Q_OBJECT
	Q_PROPERTY(int UserType READ getUserType WRITE setUserType)
public:
	Login(QWidget *parent = nullptr);
	virtual void showEvent(QShowEvent* event) override;
	~Login();
	int getUserType() 
	{
		return UserType;	
	}
	void setUserType(int type) 
	{
		UserType = type;
		emit sigUserTypeChanged(UserType);
	}
private:
	bool getInitPW(QString path);
	
private:
	int* user;
	Ui::LoginClass ui;
	QString technologyPW;
	QString adminPW;
	int UserType=0;
signals:
	void sigLogin(int n);
	void sigUserTypeChanged(int usertype);
private slots:
	void on_pbLogin_clicked();
	void on_pbLogout_clicked();
};
