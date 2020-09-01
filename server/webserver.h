#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <QJsonArray>
#include <QMap>
#include <QObject>

class QHttpServer;
class WebServer : public QObject
{
    Q_OBJECT

    QHttpServer *httpServer;
    QMap<QString, QByteArray> cache;

    QByteArray readFile(const QString &name);
    QString _filesPath;
    QJsonArray filesArray;

    void loadData();
    bool isEnable() const;
public:
    WebServer(QObject *parent = nullptr);
    WebServer(const QString &filesPath, QObject *parent = nullptr);
    void start(const quint16 &port);
};

#endif // WEBSERVER_H
