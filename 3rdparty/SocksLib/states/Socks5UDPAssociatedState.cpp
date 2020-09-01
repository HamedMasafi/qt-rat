#include "Socks5UDPAssociatedState.h"

#include <QtDebug>
#include <QSharedPointer>

#include "protocol/Socks5UDPRequestMessage.h"

Socks5UDPAssociatedState::Socks5UDPAssociatedState(QUdpSocket * clientFacing,
                                                   QUdpSocket * remoteFacing,
                                                   SocksConnection *parent) :
    SocksState(parent), _clientFacing(clientFacing), _remoteFacing(remoteFacing)
{
}

void Socks5UDPAssociatedState::handleIncomingBytes(QByteArray &bytes)
{
    Q_UNUSED(bytes)
    qWarning() << this << "received unexpected TCP bytes";
}

void Socks5UDPAssociatedState::handleSetAsNewState()
{
    //Make sure we got good sockets!
    if (_remoteFacing.isNull() || _clientFacing.isNull())
    {
        qWarning() << this << "got a null UDP socket!";
        _parent->close();
        return;
    }

    //Just in case the state we transitioned from didn't do this!
    _clientFacing->setParent(this);
    _remoteFacing->setParent(this);

    //Set up signals/slots to do relaying
    connect(_clientFacing.data(),
            SIGNAL(readyRead()),
            this,
            SLOT(handleClientReadyRead()));
    connect(_remoteFacing.data(),
            SIGNAL(readyRead()),
            this,
            SLOT(handleRemoteReadyRead()));

    qDebug() << "UDPAssociate initialized";
}

//private slot
void Socks5UDPAssociatedState::handleRemoteReadyRead()
{
    while (_remoteFacing->hasPendingDatagrams())
    {
        QHostAddress senderHost;
        quint16 senderPort;
        QByteArray bytes;
        if (this->readDatagram(_remoteFacing,&senderHost,&senderPort,&bytes))
            this->handleRemoteDatagram(senderHost,senderPort,bytes);
        else
            qWarning() << "Failed to read remote datagram";
    }
}

//private slot
void Socks5UDPAssociatedState::handleClientReadyRead()
{
    while (_clientFacing->hasPendingDatagrams())
    {
        QHostAddress senderHost;
        quint16 senderPort;
        QByteArray bytes;
        if (this->readDatagram(_clientFacing,&senderHost,&senderPort,&bytes))
            this->handleClientDatagram(senderHost,senderPort,bytes);
        else
            qWarning() << "Failed to read client datagram";
    }
}

//private slot
void Socks5UDPAssociatedState::handleRemoteDatagram(QHostAddress senderHost, quint16 senderPort, QByteArray bytes)
{
    //Package it up in the UDP header and send it OVER UDP
    QSharedPointer<Socks5UDPRequestMessage> toSend(new Socks5UDPRequestMessage(senderHost,senderPort,bytes));

    QByteArray toSendBytes;
    QString toBytesError;
    if (!toSend->toBytes(&toSendBytes,&toBytesError))
    {
        qWarning() << "Failed to serialize remote datagram to client. Error:" << toBytesError;
        return;
    }

    _clientFacing->writeDatagram(toSendBytes,_parent->peerAddress(),_lastClientUDPPort);
    qDebug() << "Wrote" << toSendBytes.size() << "bytes to" << _parent->peerAddress() << _lastClientUDPPort;
}

//private slot
void Socks5UDPAssociatedState::handleClientDatagram(QHostAddress senderHost, quint16 senderPort,QByteArray bytes)
{
    if (senderHost != _parent->peerAddress())
    {
        qWarning() << "Got datagram from unauthorized/unknown client --- dropped!";
        return;
    }

    //Should this be the way it's done!?
    _lastClientUDPPort = senderPort;

    //Parse the UDP relay header
    QSharedPointer<Socks5UDPRequestMessage> msg(new Socks5UDPRequestMessage());
    QString parseErrorString;
    SocksProtocolMessage::ParseResult parseResult = msg->parse(bytes,&parseErrorString);
    if (parseResult != SocksProtocolMessage::Success)
    {
        qWarning() << "Bad datagram from client. Error:" << parseErrorString;
        return;
    }

    //We don't handle domain name requests right now
    if (msg->addressType() == SocksProtocolMessage::DomainName)
    {
        qWarning() << "Client sent a datagram bound for a domain name --- dropped!";
        return;
    }

    //Forward to the remote the client specified
    _remoteFacing->writeDatagram(msg->data(),msg->address(),msg->port());
    qDebug() << msg->data().toHex();
}

//private
bool Socks5UDPAssociatedState::readDatagram(QUdpSocket *socket, QHostAddress *senderHost, quint16 *senderPort, QByteArray *bytes)
{
    if (socket == 0 || senderHost == 0 || senderPort == 0 || bytes == 0)
    {
        qWarning() << "Null argument to readDatagram";
        return false;
    }

    if (!socket->hasPendingDatagrams())
        return false;

    qint64 pendingSize = socket->pendingDatagramSize();
    QByteArray tempBytes(pendingSize,(char)0);
    qint64 readSize = socket->readDatagram(tempBytes.data(),pendingSize,senderHost,senderPort);

    if (readSize != pendingSize)
        return false;

    *bytes = tempBytes;
    return true;
}
