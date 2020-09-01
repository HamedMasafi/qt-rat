#ifndef SOCKS5UDPASSOCIATESTATE_H
#define SOCKS5UDPASSOCIATESTATE_H

#include "SocksState.h"
#include "protocol/Socks5RequestMessage.h"

#include <QSharedPointer>
#include <QHostInfo>

class Socks5UDPAssociateState : public SocksState
{
    Q_OBJECT
public:
    explicit Socks5UDPAssociateState(QSharedPointer<Socks5RequestMessage> request,
                                     SocksConnection *parent = 0);

    //pure-virtual from SocksState
    virtual void handleIncomingBytes(QByteArray& bytes);

    //virtual from SocksState
    virtual void handleSetAsNewState();
    
signals:
    
public slots:

private slots:
    void handleDomainLookupResult(const QHostInfo& info);

private:
    void handleIP();
    void handleDomain();

    QSharedPointer<Socks5RequestMessage> _request;

    int _dnsLookupID;
    
};

#endif // SOCKS5UDPASSOCIATESTATE_H
