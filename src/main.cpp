#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator myTranslator;
    myTranslator.load(QApplication::applicationDirPath() + "/upx-test." + QLocale::system().name());
    a.installTranslator(&myTranslator);

    MainWindow w;
    w.show();

    return a.exec();
}
