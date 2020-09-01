#include "SocksConnection.h"
#include "sockshub.h"

#include <QTcpSocket>
#include <QDebug>

SocksHub::SocksHub(QObject *parent) : QObject(parent)
{

}

void SocksHub::makeNewConnection()
{
    QTcpSocket * clientSock = new QTcpSocket(this);
    clientSock->connectToHost(m_serverAddress, m_port);
    clientSock->waitForConnected();

    if (clientSock->state() != QTcpSocket::ConnectedState)
        qDebug() << "Unable to connect to reverse proxy:" << clientSock->errorString();
    else
        qDebug() << "Connected to reverse proxy" << m_port;

    QPointer<SocksConnection> connection = new SocksConnection(clientSock,this);
    connect(clientSock,
            &QObject::destroyed,
            this,
            &SocksHub::handleConnectionDestroyed);
    _connections.append(connection);
}

QString SocksHub::serverAddress() const
{
    return m_serverAddress;
}

quint16 SocksHub::port() const
{
    return m_port;
}

void SocksHub::setServerAddress(QString serverAddress)
{
    if (m_serverAddress == serverAddress)
        return;

    m_serverAddress = serverAddress;
    emit serverAddressChanged(m_serverAddress);
}

void SocksHub::setPort(quint16 port)
{
    if (m_port == port)
        return;

    m_port = port;
    emit portChanged(m_port);
}

bool SocksHub::start()
{
    _controllerSocket = new QTcpSocket(this);
    connect(_controllerSocket, &QTcpSocket::readyRead, this, &SocksHub::controllerSocket_readyRead);
    _controllerSocket->connectToHost(m_serverAddress, m_port);
    bool ok =_controllerSocket->waitForConnected();
    if (!ok)
        qDebug() << "Controller socket unable to connect to" << m_serverAddress << ":" << m_port;
    return ok;
}

void SocksHub::controllerSocket_readyRead()
{
    auto buffer = _controllerSocket->readAll();
    auto c =  buffer.count("*SEND*");
    qDebug() << "Making" << c << "connection";
    for (int i = 0; i < c; ++i)
        makeNewConnection();
}

void SocksHub::handleConnectionDestroyed()
{
    QMutableListIterator<QPointer<SocksConnection> > iter(_connections);

    bool pruned = false;
    while (iter.hasNext())
    {
        const QPointer<SocksConnection>& conn = iter.next();
        if (conn.isNull())
        {
            iter.remove();
            pruned = true;
        }
    }

    if (!pruned)
        qWarning() << "handleConnectionDestroyed called but no dead connections were found...";
}
