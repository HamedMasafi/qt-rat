#include "Socks4InitialState.h"

#include "protocol/Socks4RequestMessage.h"
#include "Socks4ConnectState.h"

#include <QSharedPointer>
#include <QtDebug>

Socks4InitialState::Socks4InitialState(SocksConnection *parent) :
    SocksState(parent)
{
}

void Socks4InitialState::handleIncomingBytes(QByteArray &bytes)
{
    QSharedPointer<Socks4RequestMessage> msg(new Socks4RequestMessage());

    QString parseErrorString;
    SocksProtocolMessage::ParseResult parseResult = msg->parse(bytes,&parseErrorString);

    if (parseResult == SocksProtocolMessage::NotEnoughData)
    {
        //Try again next time we get incoming bytes
        return;
    }
    else if (parseResult == SocksProtocolMessage::Failure)
    {
        //Print an error and close the connection
        qWarning() << "Failed to parse SOCKS4 request. Error:" << parseErrorString;
        _parent->close();
        return;
    }
    else if (parseResult != SocksProtocolMessage::Success)
    {
        qWarning() << this << "fell to unknown ParseResult value";
        _parent->close();
        return;
    }

    if (msg->command() == Socks4RequestMessage::Connect)
    {
        //Transition to Socks4ConnectState
        Socks4ConnectState * nState = new Socks4ConnectState(msg,_parent);
        _parent->setState(nState);
        return;
    }
    else if (msg->command() == Socks4RequestMessage::Bind)
    {
        //Transition to Socks4BindState
    }
    else
    {
        qWarning() << this << "fell to unknown Socks4Command";
        _parent->close();
        return;
    }
}
