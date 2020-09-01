#ifndef Socks5AuthenticatedState_H
#define Socks5AuthenticatedState_H

#include "SocksState.h"
#include "SocksConnection.h"
#include "protocol/Socks5RequestMessage.h"

#include <QTimer>
#include <QHostInfo>

class Socks5AuthenticatedState : public SocksState
{
    Q_OBJECT
public:
    explicit Socks5AuthenticatedState(SocksConnection *parent = 0);

    //pure-virtual from SocksState
    virtual void handleIncomingBytes(QByteArray &bytes);
    
signals:
    
public slots:

private slots:
    void handleTimeout();

private:
    QTimer * _timeoutTimer;

    
};

#endif // Socks5AuthenticatedState_H
