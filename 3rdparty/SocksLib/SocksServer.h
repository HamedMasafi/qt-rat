#ifndef SOCKSSERVER_H
#define SOCKSSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QPointer>
#include <QList>
#include <QHostAddress>

#include "SocksLib_global.h"
class SocksConnection;

class SOCKSLIBSHARED_EXPORT SocksServer : public QObject
{
    Q_OBJECT
public:
    explicit SocksServer(QHostAddress listenAddress = QHostAddress::Any,
                         quint16 listenPort = 1080,
                         qreal throttle = 500.0,
                         QObject *parent = 0);
    ~SocksServer();

    void start();

    bool isStarted() const;
    
    quint16 listenPort() const;
    void setListenPort(const quint16 &listenPort);

    qreal throttle() const;
    void setThrottle(const qreal &throttle);

signals:

private slots:
    void handleNewIncomingConnection();
    void handleConnectionDestroyed();

private:
    QHostAddress _listenAddress;
    quint16 _listenPort;
    qreal _throttle;
    QPointer<QTcpServer> _serverSock;
    QList<QPointer<SocksConnection> > _connections;
    
};

#endif // SOCKSSERVER_H
