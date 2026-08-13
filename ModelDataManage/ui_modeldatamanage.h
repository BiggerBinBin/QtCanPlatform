/********************************************************************************
** Form generated from reading UI file 'modeldatamanage.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MODELDATAMANAGE_H
#define UI_MODELDATAMANAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ModelDataManage
{
public:

    void setupUi(QWidget *ModelDataManage)
    {
        if (ModelDataManage->objectName().isEmpty())
            ModelDataManage->setObjectName(QString::fromUtf8("ModelDataManage"));
        ModelDataManage->resize(1477, 722);

        retranslateUi(ModelDataManage);

        QMetaObject::connectSlotsByName(ModelDataManage);
    } // setupUi

    void retranslateUi(QWidget *ModelDataManage)
    {
        ModelDataManage->setWindowTitle(QCoreApplication::translate("ModelDataManage", "ModelDataManage", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ModelDataManage: public Ui_ModelDataManage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MODELDATAMANAGE_H
