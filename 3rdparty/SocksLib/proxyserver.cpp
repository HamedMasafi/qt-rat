#include "pairsocket.h"
#include "proxyserver.h"
#include "../common/abstractclient.h"

#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

Q_LOGGING_CATEGORY(ProxyServerCat, "ProxyServer")

ProxyServer::ProxyServer(AbstractClient *client, QObject *parent) : QObject(parent), _client(client),
    _controllerSocket(nullptr), m_localPort(0), m_remotePort(0)
{
    _localServer = new QTcpServer(this);
    connect(_localServer, &QTcpServer::newConnection, this, &ProxyServer::localServer_newConnection);

    _remoteServer = new QTcpServer(this);
    connect(_remoteServer, &QTcpServer::newConnection, this, &ProxyServer::remoteServer_newConnection);
}

bool ProxyServer::start()
{
    bool ok = _localServer->listen(QHostAddress::Any, m_localPort);

    if (!ok)
        return false;
    ok = _remoteServer->listen(QHostAddress::Any, m_remotePort);

    if (!ok)
        return false;

    if (!m_localPort)
        setLocalPort(_localServer->serverPort());

    if (!m_remotePort)
        setLocalPort(_remoteServer->serverPort());

    qDebug () << "***Proxy server running";
    qDebug () << "      Local port" << m_localPort;
    qDebug () << "      Remote port" << m_remotePort;
    return true;
}

void ProxyServer::stop()
{
    _localServer->close();
    _remoteServer->close();
}

quint16 ProxyServer::localPort() const
{
    return m_localPort;
}

quint16 ProxyServer::remotePort() const
{
    return m_remotePort;
}

void ProxyServer::localServer_newConnection()
{
    if (!_controllerSocket) {
        qDebug() << "Controller socket is null";
        return;
    }
    while (_localServer->hasPendingConnections()) {
        qDebug() << "new connection from local";
        auto socket = _localServer->nextPendingConnection();
        auto pair = new PairSocket(socket, this);
        _pendindSockets.append(pair);
//        bool ok = _client->requestNewConnection();
        _controllerSocket->write("*SEND*");
        _controllerSocket->flush();

//        if (!ok) {
//            _pendindSockets.removeOne(pair);
//            qDebug() << "request failed";
//        }
    }
}

void ProxyServer::remoteServer_newConnection()
{
    if (!_remoteServer->hasPendingConnections()) {
        qDebug() << "No pending connection";
        return;
    }
    while (_remoteServer->hasPendingConnections()) {
        if (!_controllerSocket) {
            _controllerSocket = _remoteServer->nextPendingConnection();
            qDebug() << "Controller socker set";
            continue;
        }
        qDebug() << "new connection from remote";
        if (!_pendindSockets.size()) {
            qDebug() << "No pending socket";
            return;
        }
        auto socket = _remoteServer->nextPendingConnection();
        auto pair = _pendindSockets.takeFirst();
        pair->setSecond(socket);
    }
}

void ProxyServer::setLocalPort(quint16 localPort)
{
    if (m_localPort == localPort)
        return;

    m_localPort = localPort;
    emit localPortChanged(m_localPort);
}

void ProxyServer::setRemotePort(quint16 remotePort)
{
    if (m_remotePort == remotePort)
        return;

    m_remotePort = remotePort;
    emit remotePortChanged(m_remotePort);
}
