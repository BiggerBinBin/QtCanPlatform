#include "CANMasterPlus.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.addLibraryPath(QCoreApplication::applicationDirPath() + "/plugins");
    CANMasterPlus w;
    w.setWindowIcon(QIcon(QCoreApplication::applicationDirPath() + "\\app-logo.ico"));
    w.showMaximized();
    
    return a.exec();
}
