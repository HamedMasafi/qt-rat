#ifndef SocksState_H
#define SocksState_H

#include <QObject>

#include "SocksConnection.h"

class SocksState : public QObject
{
    Q_OBJECT
public:
    explicit SocksState(SocksConnection *parent = 0);
    virtual ~SocksState();

    //Called when SocksConnection sets us as the new state OR when new incoming bytes come
    virtual void handleIncomingBytes(QByteArray& bytes)=0;

    //Called when SocksConnection sets us as the new state
    virtual void handleSetAsNewState();
    
signals:
    
public slots:

protected:
    SocksConnection * _parent;
    
};

#endif // SocksState_H
