#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include <QFile>
#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;
class FileDownloader : public QObject
{
    Q_OBJECT
    QNetworkAccessManager *_nat;
    QNetworkReply *_reply;
    QString _url;
    QFile _localFile;
    bool _autoExec;
    bool _autoDelete;

public:
    FileDownloader(const QString &url, QObject *parent = nullptr);
    void startDownload();

    bool autoExec() const;
    void setAutoExec(bool autoExec);

    bool autoDelete() const;
    void setAutoDelete(bool autoDelete);

private slots:
    void reply_readyRead();
    void reply_finished();
};

#endif // FILEDOWNLOADER_H
