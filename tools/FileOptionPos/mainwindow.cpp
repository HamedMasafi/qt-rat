#include "mainwindow.h"

#include <QFile>
#include <QByteArray>
#include <functional>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
                                          QMainWindow(parent)
{
    setupUi(this);
}

void MainWindow::on_pushButtonCheck_clicked()
{

    QByteArray content;
    auto replace_var = [&content](const QString &name, const QString &value) -> bool {
        auto i = content.indexOf("**" + name + "**");
        if (i == -1)
            return false;
        content.remove(i, 20);
        QByteArray extraSpaces(20 - value.length(), ' ');
        content.insert(i, value.toLatin1() + extraSpaces);

        qDebug() << name << i;
        return true;
    };
    QFile f(lineEditPath->text());
    if (!f.open(QIODevice::ReadOnly)) {
        qDebug() << "unable to open file";
        return;
    }
//    content = f.readAll();
    content = plainTextEdit->toPlainText().toUtf8();
    f.close();
    /*
12345678901234567890
**is_ready**        E
**server_address**  E
**port**            E
    */
    replace_var("is_ready", "true");
    replace_var("server_address", "127.0.0.1");
    replace_var("port", "8400");

    plainTextEdit->setPlainText(content);
}
