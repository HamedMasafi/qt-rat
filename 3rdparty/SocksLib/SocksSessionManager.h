#ifndef SOCKSSESSIONMANAGER_H
#define SOCKSSESSIONMANAGER_H

#include <QObject>
#include <QSet>
#include <QString>
#include <QHash>
#include <QSharedPointer>

#include "SocksSession.h"

class SocksSessionManager : public QObject
{
    Q_OBJECT
public:
    explicit SocksSessionManager(QObject *parent = 0);
    virtual ~SocksSessionManager();

private:
    QHash<QString, QSharedPointer<SocksSession> > _sessions;
    
signals:
    
public slots:
    virtual void handleNewConnection(QPointer<SocksConnection> newConnection);
    
};

#endif // SOCKSSESSIONMANAGER_H
