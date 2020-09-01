#include "Socks5AuthState.h"

#include "protocol/Socks5MethodSelectionMessage.h"
#include "Socks5AuthenticatedState.h"

Socks5AuthState::Socks5AuthState(QSharedPointer<Socks5GreetingMessage> clientGreeting, SocksConnection *parent) :
    SocksState(parent), _clientGreeting(clientGreeting)
{
}

//pure-virtual from SocksState
void Socks5AuthState::handleIncomingBytes(QByteArray &bytes)
{
    Q_UNUSED(bytes)
}

//virtual from SocksState
void Socks5AuthState::handleSetAsNewState()
{
    //Evaluate the greeting. If it's good, proceed with auth. Otherwise, reject!
    if (_clientGreeting.isNull())
    {
        qWarning() << this << "has null greeting!";
        _parent->close();
        return;
    }

    //Get list of acceptable auth methods from somewhere. Choose the most preferred, transition to that state
    if (_clientGreeting->authMethods().contains(0x00))
    {
        //Send message accepting auth method and transition to appropriate state
        //In this case, since 0x00 is the "no authentication" method, transition to already being authenticated
        QSharedPointer<Socks5MethodSelectionMessage> msg = QSharedPointer<Socks5MethodSelectionMessage>(new Socks5MethodSelectionMessage(0x00));

        QString sendError;
        if (!_parent->sendMessage(msg,&sendError))
        {
            qWarning() << "Failed to serialize/send auth selection method";
            _parent->close();
            return;
        }
        Socks5AuthenticatedState * nState = new Socks5AuthenticatedState(_parent);
        _parent->setState(nState);
        return;
    }
    //Send a nasty note and close the connection
    else
    {
        QSharedPointer<Socks5MethodSelectionMessage> msg = QSharedPointer<Socks5MethodSelectionMessage>(new Socks5MethodSelectionMessage(0xff));
        _parent->sendMessage(msg);
        _parent->close();
        return;
    }
}
