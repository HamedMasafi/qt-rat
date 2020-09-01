#include "mainwindow.h"

#include <QtWidgets/QApplication>
#include <QStandardPaths>
#include <QDebug>
#include <QDir>
#include "global.h"
#include "../config.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationName("rat");
    a.setApplicationDisplayName("RAT");
    a.setApplicationVersion(RAT_SERVER_VERSION);

//    Global::loadCss(":/themes/modern.css");

    auto p = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir d(p);
    if (!d.exists())
        d.mkdir(p);

    MainWindow w;
//    w.installTranslator("fa");
    w.show();
    return a.exec();
}
