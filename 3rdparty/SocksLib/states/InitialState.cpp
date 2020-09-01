#include "InitialState.h"

#include <QtDebug>

#include "protocol/Socks5GreetingMessage.h"
#include "protocol/Socks5MethodSelectionMessage.h"

#include "Socks5AuthState.h"
#include "Socks4InitialState.h"

InitialState::InitialState(SocksConnection *parent) :
    SocksState(parent)
{
    _timeoutTimer = new QTimer(this);
    connect(_timeoutTimer,
            SIGNAL(timeout()),
            this,
            SLOT(handleTimeout()));
    _timeoutTimer->start(10 * 1000);
}

void InitialState::handleIncomingBytes(QByteArray &bytes)
{
    if (bytes.isEmpty())
    {
        qWarning() << "handleIncomingBytes got empty byte array";
        return;
    }

    //Try to parse a Socks5GreetingMessage out of the bytes
    QSharedPointer<Socks5GreetingMessage> socks5Greeting(new Socks5GreetingMessage());
    QString socks5ParseErrorString;
    SocksProtocolMessage::ParseResult socks5ParseResult = socks5Greeting->parse(bytes,&socks5ParseErrorString);
    if (socks5ParseResult == SocksProtocolMessage::Success)
    {
        //Tell parent that we're SOCKS5
        _parent->setSocksVersion(SocksProtocolMessage::SOCKS5);

        //Transition to Socks5AuthState
        Socks5AuthState * nState = new Socks5AuthState(socks5Greeting, _parent);
        _parent->setState(nState);
        return;
    }
    //if that fails, see if Socks4 may be applicable
    else if (bytes.at(0) == 0x04)
    {
        //Tell parent that we're SOCKS4
        _parent->setSocksVersion(SocksProtocolMessage::SOCKS4);

        //Transition to Socks4InitialState
        Socks4InitialState * nState = new Socks4InitialState(_parent);
        _parent->setState(nState);
        return;
    }
    //Otherwise, we might just need more bytes to make a SOCKS5 message
    else if (socks5ParseResult == SocksProtocolMessage::NotEnoughData)
    {
        //Try again later
        return;
    }
    //Something bad has happened
    else
    {
        qWarning() << socks5ParseErrorString;
        _parent->close();
        return;
    }
}

//private slot
void InitialState::handleTimeout()
{
    //Close the connection (give up)
    _parent->close();
}
