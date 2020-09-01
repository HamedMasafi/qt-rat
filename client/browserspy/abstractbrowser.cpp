#include "abstractbrowser.h"

#include <QTime>
#include <QDir>
#include <QFile>
#include <QDebug>

#ifdef Q_OS_WIN
#   include "rawcopy.h"
#endif

AbstractBrowser::AbstractBrowser()
{

}

bool AbstractBrowser::copyFile(QString &file, bool rawCopy)
{
    qsrand(QTime::currentTime().msec());
    QString t = QDir::tempPath() + "/data." + QString::number(qrand());
    if (QFile::exists(t) && !QFile::remove(t))
        return false;

#ifdef Q_OS_WIN
    if (rawCopy) {
        if (ERROR_SUCCESS != RawCopy(file.toStdWString(), t.toStdWString()))
            return false;
    } else {
        if (!CopyFileA(file.toLocal8Bit().data(), t.toLocal8Bit().data(), false)) {
            qDebug() << "Unable to copy file" << file << "to"
                     << t;
            return false;
        }
    }
#else
    Q_UNUSED(rawCopy)

    if (!QFile::copy(file, t)) {
        return false;
    }
#endif

    file = t;
    return true;
}

void AbstractBrowser::addPasswordToList(QVariantList &list, const QString &browser, const QString &url, const QString &username, const QString &password)
{
    QVariantMap map;
    map.insert("browser", browser);
    map.insert("url", url);
    map.insert("username", username);
    map.insert("password", password);
    list.append(map);
}
