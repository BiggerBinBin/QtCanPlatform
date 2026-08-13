/********************************************************************************
** Form generated from reading UI file 'QSetProperty.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QSETPROPERTY_H
#define UI_QSETPROPERTY_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QSetPropertyClass
{
public:

    void setupUi(QWidget *QSetPropertyClass)
    {
        if (QSetPropertyClass->objectName().isEmpty())
            QSetPropertyClass->setObjectName(QString::fromUtf8("QSetPropertyClass"));
        QSetPropertyClass->resize(600, 400);

        retranslateUi(QSetPropertyClass);

        QMetaObject::connectSlotsByName(QSetPropertyClass);
    } // setupUi

    void retranslateUi(QWidget *QSetPropertyClass)
    {
        QSetPropertyClass->setWindowTitle(QCoreApplication::translate("QSetPropertyClass", "QSetProperty", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QSetPropertyClass: public Ui_QSetPropertyClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QSETPROPERTY_H
