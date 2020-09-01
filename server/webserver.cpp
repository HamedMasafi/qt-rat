#include "webserver.h"

#include "../3rdparty/QHttpServer/src/httpserver/qhttpserver.h"
#include "../3rdparty/QHttpServer/src/httpserver/qhttpserverresponse.h"

#include <QDebug>
#include <QJsonObject>
#include <QApplication>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>

QByteArray WebServer::readFile(const QString &name)
{
    if (cache.contains(name))
        return cache.value(name);

    QFile f(name);
    if (!f.open(QIODevice::ReadOnly))
        return QByteArray();
    auto buffer = f.readAll();

    cache.insert(name, buffer);
    f.close();
    return buffer;
}

void WebServer::loadData()
{
    QFile f(":/files.json");
    if (f.open(QIODevice::ReadOnly)) {
        auto doc = QJsonDocument::fromJson(f.readAll());
        f.close();
        filesArray = doc.array();
    }
}

bool WebServer::isEnable() const
{
#ifdef RAT_ENABLE_WEB_SERVER
    return true;
#else
    return false;
#endif
}

WebServer::WebServer(QObject *parent) : QObject(parent)
{
    loadData();
    _filesPath = qApp->applicationDirPath() + "/files/";
}

WebServer::WebServer(const QString &filesPath, QObject *parent) : QObject(parent)
{
    if (filesPath.endsWith("/"))
        _filesPath = filesPath;
    else
        _filesPath = filesPath + "/";
    loadData();
}

void WebServer::start(const quint16 &port)
{
#ifdef RAT_ENABLE_WEB_SERVER
    httpServer = new QHttpServer(this);
    httpServer->route("/", [this]() {
        QString buffer = readFile(":/html/index.htm");

        buffer = buffer.replace("%version%", qApp->applicationVersion());
        return buffer;
    });
    httpServer->route("/download/<arg>/<arg>/<arg>", [this](const QString &platform, const QString &arch, const QString &name) -> QHttpServerResponse{
//        if (!_platforms.contains(platform))
//            return QHttpServerResponse("Invalid platform");

//        if (!_platforms.value(platform).contains(arch))
//            return QHttpServerResponse("Invalid arch");

        QString filePath = QString("%1%2/%3/%4")
                .arg(QString(_filesPath), platform, arch, name);

        if (!QFile::exists(filePath))
            return QHttpServerResponse("The file not found");

        auto r = QHttpServerResponse::fromFile(filePath);
        r.setHeader("Content-Disposition", "filename=\"" + name.toLocal8Bit() + "\"");
        return r;
    });

    httpServer->route("/files/<arg>/<arg>", [this](const QString &platform, const QString &arch) -> QHttpServerResponse{
//        if (!_platforms.contains(platform))
//            return QHttpServerResponse("Invalid platform");

//        if (!_platforms.value(platform).contains(arch))
//            return QHttpServerResponse("Invalid arch");

        for (auto i = filesArray.begin(); i != filesArray.end(); ++i) {
            QJsonObject o = (*i).toObject();
            if (QJsonObject() == o)
                continue;

            if (platform.compare(o.value("platform").toString(), Qt::CaseInsensitive) == 0
                    && arch.compare(o.value("arch").toString(), Qt::CaseInsensitive) == 0)
                return o.value("files").toArray();
        }
        return QJsonArray();
    });

    httpServer->route("/client/<arg>/<arg>", [](const QString &platform, const QString &arch) -> QHttpServerResponse{
        Q_UNUSED(platform)
        Q_UNUSED(arch)

        QString filePath = qApp->applicationDirPath() + "/files/windows.zip";

        if (!QFile::exists(filePath))
            return QHttpServerResponse("The file not found");

        auto r = QHttpServerResponse::fromFile(filePath);
        r.setHeader("Content-Disposition", "filename=\"windows.zip\"");
        return r;
    });

    const auto p = httpServer->listen(QHostAddress::Any, port);
    if (p == -1) {
        qDebug() << QString("Could not run on http://127.0.0.1:%1/").arg(port);
        return;
    }
#endif
}

