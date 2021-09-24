#include "mainwindow.h"
#include "queryrunner.h"
#include <QApplication>

int main(int argc, char *argv[])
{

    qRegisterMetaType<QSqlQuery>("QSqlQuery");
    qRegisterMetaType<TomSqlResult>("TomSqlResult");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
