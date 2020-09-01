#ifndef SOCKS5CONNECTEDSTATE_H
#define SOCKS5CONNECTEDSTATE_H

#include "SocksState.h"

#include <QTcpSocket>
#include <QPointer>
#include <QIODevice>

class Socks5ConnectedState : public SocksState
{
    Q_OBJECT
public:
    explicit Socks5ConnectedState(QIODevice * remoteSocket, SocksConnection *parent = 0);

    //pure-virtual from SocksState
    virtual void handleIncomingBytes(QByteArray& bytes);

    //virtual from SocksState
    virtual void handleSetAsNewState();

private:
    QPointer<QIODevice> _socket;
    bool _shutdown;
    
signals:
    
public slots:

private slots:
    void handleRemoteReadyRead();
    void handleRemoteDisconnect();
    
};

#endif // SOCKS5CONNECTEDSTATE_H
