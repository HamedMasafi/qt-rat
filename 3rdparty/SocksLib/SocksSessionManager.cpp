#include "SocksSessionManager.h"

SocksSessionManager::SocksSessionManager(QObject *parent) :
    QObject(parent)
{
}

SocksSessionManager::~SocksSessionManager()
{
}

void SocksSessionManager::handleNewConnection(QPointer<SocksConnection> newConnection)
{
}
