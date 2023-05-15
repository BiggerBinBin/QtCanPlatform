#pragma once

#include <QDialog>
#include "ui_Login.h"
class Login : public QDialog
{
	Q_OBJECT

public:
	Login(QWidget *parent = nullptr);
	virtual void showEvent(QShowEvent* event) override;
	~Login();
private:
	bool getInitPW(QString path);
	
private:
	int* user;
	Ui::LoginClass ui;
	QString technologyPW;
	QString adminPW;
signals:
	void sigLogin(int n);
private slots:
	void on_pbLogin_clicked();
};
