#define _AFXDLL
#include "QtCanPlatform.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QtCanPlatform w;
    w.show();
    w.setWindowIcon(QIcon(":/QtCanPlatform/app-logo.ico"));
    return a.exec();
}
