#ifndef PROXYSERVER_H
#define PROXYSERVER_H

#include <QMap>
#include <QObject>
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(ProxyServerCat);

class PairSocket;
class QTcpServer;
class QTcpSocket;
class AbstractClient;
class ProxyServer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint16 localPort READ localPort WRITE setLocalPort NOTIFY localPortChanged)
    Q_PROPERTY(quint16 remotePort READ remotePort WRITE setRemotePort NOTIFY remotePortChanged)

    AbstractClient *_client;
    QTcpSocket *_controllerSocket;
    QTcpServer *_localServer;
    QTcpServer *_remoteServer;
    QList<PairSocket*> _pendindSockets;

    quint16 m_localPort;
    quint16 m_remotePort;

public:
    explicit ProxyServer(AbstractClient *client, QObject *parent = nullptr);

    bool start();
    void stop();

    quint16 localPort() const;
    quint16 remotePort() const;

    static ProxyServer *instance();
    static void setInstance(ProxyServer *instance);

public slots:
    void localServer_newConnection();
    void remoteServer_newConnection();
    void setLocalPort(quint16 localPort);
    void setRemotePort(quint16 remotePort);

signals:

    void localPortChanged(quint16 localPort);
    void remotePortChanged(quint16 remotePort);
};

#endif // PROXYSERVER_H
