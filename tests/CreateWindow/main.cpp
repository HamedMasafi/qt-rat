#include "window.h"
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    CrossWindow w;
    w.show();

    QEventLoop loop;
    loop.exec();

    return a.exec();
}
