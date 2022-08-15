#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtCanPlatform.h"
#include <QTableView>
#include <qtextbrowser.h>
#include <iostream>
#include <memory>
#include <QStringList>
#include <QStringListModel>
#include "QCanSetting.h"
#pragma execution_character_set("utf-8")  
class QtCanPlatform : public QMainWindow
{
    Q_OBJECT

public:
    QtCanPlatform(QWidget *parent = nullptr);
    ~QtCanPlatform();

private:
    Ui::QtCanPlatformClass ui;
    void initUi();
    void initData();
    QStringListModel *model=nullptr;

    //UI
    QTableView* tableView=nullptr;
    QTextBrowser* textBrowser=nullptr;

    QCanSetting* canSetting = nullptr;
private slots:
    void qCanSettingShow();
};
