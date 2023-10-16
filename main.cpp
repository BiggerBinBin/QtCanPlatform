#define _AFXDLL
#include "QtCanPlatform.h"
#include <QtWidgets/QApplication>
#include <qtranslator.h>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //���Ҫ��ǰ��
    QTranslator* translator = new QTranslator();
   
    QString apppath = QApplication::applicationDirPath() + "/translations/QtCanPlatform_en.qm";
    bool b = translator->load(apppath);
    if (b)
    {
        qApp->installTranslator(translator);
    }
    CanTestPlatform w;
    QObject::connect(&w, &CanTestPlatform::actionChinese_triggered, [&] () {
        if (!translator)
            translator = new QTranslator();
        
        bool b = translator->load(QApplication::applicationDirPath() + "/translations/qt_zh_TW.qm");
        if (b)
        {
            qApp->installTranslator(translator);
        }
        
        });
    QObject::connect(&w, &CanTestPlatform::actionEnglish_triggered, [&]() {
        if(!translator)
            translator = new QTranslator();
        bool b = translator->load(apppath);
        if (b)
        {
            qApp->installTranslator(translator);
        }
        });
    w.show();
    w.setWindowIcon(QIcon(":/QtCanPlatform/app-logo.ico"));
   
    
    return a.exec();
    //delete translator;

}
