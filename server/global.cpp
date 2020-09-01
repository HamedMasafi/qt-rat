#include "global.h"

#include <QFile>
#include <QApplication>
#include <QDebug>

Global::Global()
{

}

void Global::loadCss(const QString &cssFile)
{
//    QFile styleFile(cssFile);
//    if(!styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        qDebug() << "File not exists" << cssFile;
//        return;
//    }

//    qApp->setStyleSheet("");
//    QString content = styleFile.readAll();
//    QRegExp r("\\$\\w+\\=[^;]+\\;");

//    int pos = 0;
//    do{
//        pos = r.indexIn(content, pos);
//        if( pos == -1 ) break;

//        QString s = r.cap();
//        content = content.replace(s, "");
//        s = s.replace(";", "");
//        content = content.replace(s.split("=").at(0),
//                                  s.split("=").at(1));
//    }while(pos != -1);

//    qApp->setStyleSheet(content);
}
