#include "mainwindow.h"
#include "queryrunner.h"
#include <QApplication>

int main(int argc, char *argv[])
{

    qRegisterMetaType<QSqlQuery>("QSqlQuery");
    qRegisterMetaType<TomSqlResult>("TomSqlResult");

    QApplication a(argc, argv);

    a.setApplicationName("ConductorLogFoo");
    a.setOrganizationName("ETC Inc");

    MainWindow w;
    w.show();

    return a.exec();
}
