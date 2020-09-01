#include "SocksSession.h"

#include <QtDebug>

SocksSession::SocksSession(QObject *parent) :
    QObject(parent)
{
}

SocksSession::~SocksSession()
{
}

QSet<QPointer<SocksConnection> > SocksSession::connections() const
{
    return _connections;
}

//public slot
void SocksSession::addConnection(QPointer<SocksConnection> conn)
{
    _connections.insert(conn);
}

//public slot
void SocksSession::removeConnection(QPointer<SocksConnection> conn)
{
    if (!_connections.remove(conn))
        qWarning() << "Failed to remove" << conn << "from" << this;
}


uint qHash(const QPointer<SocksConnection> ptr)
{
    return qHash(ptr.data());
}
