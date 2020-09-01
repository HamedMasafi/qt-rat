#ifndef SOCKS5AUTHSTATE_H
#define SOCKS5AUTHSTATE_H

#include "SocksState.h"
#include "protocol/Socks5GreetingMessage.h"

#include <QSharedPointer>

class Socks5AuthState : public SocksState
{
    Q_OBJECT
public:
    explicit Socks5AuthState(QSharedPointer<Socks5GreetingMessage> clientGreeting, SocksConnection *parent = 0);

    //pure-virtual from SocksState
    virtual void handleIncomingBytes(QByteArray &bytes);

    //virtual from SocksState
    virtual void handleSetAsNewState();
    
signals:
    
public slots:

private:
    QSharedPointer<Socks5GreetingMessage> _clientGreeting;
    
};

#endif // SOCKS5AUTHSTATE_H
