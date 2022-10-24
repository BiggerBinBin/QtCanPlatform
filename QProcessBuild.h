#pragma once

#include <QWidget>
#include "ui_QProcessBuild.h"
#include <QTableWidget>
#include <QPushButton>
#pragma execution_character_set("utf-8")
class QProcessBuild : public QWidget
{
	Q_OBJECT

public:
	QProcessBuild(QWidget *parent = nullptr);
	~QProcessBuild();

private:
	Ui::QProcessBuildClass ui;
	QTableWidget* processTable = nullptr;
	QPushButton* addButton = nullptr;
	QPushButton* removeButton = nullptr;
	QPushButton* upButton = nullptr;
	QPushButton* downButton = nullptr;
private:
	void InitUI();
private slots:
	void on_addButton_clicked();
	void on_removeButton_clicked();
	void on_upButton_clicked();
	void on_downButton_clicked();
};
