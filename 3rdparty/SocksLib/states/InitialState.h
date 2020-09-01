#ifndef InitialState_H
#define InitialState_H

#include "SocksState.h"

#include <QByteArray>
#include <QTimer>

class InitialState : public SocksState
{
    Q_OBJECT
public:
    explicit InitialState(SocksConnection *parent = 0);

    //pure-virtual from SocksState
    virtual void handleIncomingBytes(QByteArray &bytes);
    
signals:
    
public slots:

private slots:
    void handleTimeout();

private:
    QTimer * _timeoutTimer;

    
};

#endif // InitialState_H
