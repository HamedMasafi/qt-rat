#include "Socks5AuthenticatedState.h"

#include <QHostInfo>
#include <QTcpSocket>

#include "protocol/Socks5ReplyMessage.h"
#include "protocol/SocksReplyMessage4a.h"
#include "Socks5ConnectState.h"
#include "Socks5UDPAssociateState.h"

Socks5AuthenticatedState::Socks5AuthenticatedState(SocksConnection *parent) :
    SocksState(parent)
{
    _timeoutTimer = new QTimer(this);
    connect(_timeoutTimer,
            SIGNAL(timeout()),
            this,
            SLOT(handleTimeout()));
    _timeoutTimer->start(10 * 1000);
}

void Socks5AuthenticatedState::handleIncomingBytes(QByteArray &bytes)
{
    QSharedPointer<Socks5RequestMessage> msg = QSharedPointer<Socks5RequestMessage>(new Socks5RequestMessage());

    QString error;
    SocksProtocolMessage::ParseResult result = msg->parse(bytes,&error);
    if (result == SocksProtocolMessage::Failure)
    {
        qWarning() << "Request error:" << error;
        _parent->close();
        return;
    }
    else if (result == SocksProtocolMessage::NotEnoughData)
        return;

    //See if the request is a connect, bind, or udp thing
    if (msg->command() == Socks5RequestMessage::Connect)
    {
        Socks5ConnectState * nState = new Socks5ConnectState(msg, _parent);
        _parent->setState(nState);
        return;
    }
    else if (msg->command() == Socks5RequestMessage::Bind)
    {
        qDebug() << "BIND unsupported!";
        QSharedPointer<Socks5ReplyMessage> reply(new Socks5ReplyMessage(Socks5ReplyMessage::CommandNotSupported,
                                                                        QHostAddress::Any,
                                                                        0));
        QString sendErrorString;
        if (!_parent->sendMessage(reply,&sendErrorString))
            qWarning() << "Failed to notify client of unsupported BIND command";
        _parent->close();
        return;
    }
    else if(msg->command() == Socks5RequestMessage::UDPAssociate)
    {
        Socks5UDPAssociateState * nState = new Socks5UDPAssociateState(msg, _parent);
        _parent->setState(nState);
        return;
    }
    else
    {
        _parent->close();
        return;
    }
}

//private slot
void Socks5AuthenticatedState::handleTimeout()
{
    //Close the connection when we time out
    _parent->close();
    return;
}
