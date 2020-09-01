#ifndef SOCKS4CONNECTSTATE_H
#define SOCKS4CONNECTSTATE_H

#include "SocksState.h"
#include "protocol/Socks4RequestMessage.h"

#include <QSharedPointer>
#include <QTcpSocket>
#include <QHostInfo>

class Socks4ConnectState : public SocksState
{
    Q_OBJECT
public:
    explicit Socks4ConnectState(QSharedPointer<Socks4RequestMessage> request, SocksConnection *parent = 0);

    //pure-virtual from SocksState
    virtual void handleIncomingBytes(QByteArray &bytes);

    //virtual from SocksState
    virtual void handleSetAsNewState();
    
signals:
    
public slots:

private slots:
    void handleSocketConnected();
    void handleSocketError(QAbstractSocket::SocketError);
    void handleDomainLookupResult(const QHostInfo& info);

private:
    void handleIP();
    void handleDomain();

    QSharedPointer<Socks4RequestMessage> _request;

    QTcpSocket * _socket;

    int _dnsLookupID;

    
};

#endif // SOCKS4CONNECTSTATE_H
