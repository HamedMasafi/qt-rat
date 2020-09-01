#ifndef SOCKS4INITIALSTATE_H
#define SOCKS4INITIALSTATE_H

#include "SocksState.h"

class Socks4InitialState : public SocksState
{
    Q_OBJECT
public:
    explicit Socks4InitialState(SocksConnection *parent = 0);

    //pure-virtual from SocksState
    virtual void handleIncomingBytes(QByteArray &bytes);
    
signals:
    
public slots:
    
};

#endif // SOCKS4INITIALSTATE_H
