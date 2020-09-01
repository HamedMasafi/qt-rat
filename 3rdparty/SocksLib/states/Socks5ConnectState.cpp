#include "Socks5ConnectState.h"

#include "protocol/Socks5ReplyMessage.h"
#include "Socks5ConnectedState.h"
#include "decorators/ThrottlingDecorator.h"

Socks5ConnectState::Socks5ConnectState(QSharedPointer<Socks5RequestMessage> request, SocksConnection *parent) :
    SocksState(parent), _request(request), _socket(0)
{
}

//pure-virtual from SocksState
void Socks5ConnectState::handleIncomingBytes(QByteArray &bytes)
{
    Q_UNUSED(bytes)
}

//virtual from SocksState
void Socks5ConnectState::handleSetAsNewState()
{
    //Verify that whoever we transitioned from gave us a request to handle
    if (_request.isNull())
    {
        qWarning() << this << "has null request";
        _parent->close();
        return;
    }

    //Verify that the request is of the proper type (connect)
    if (_request->command() != Socks5RequestMessage::Connect)
    {
        qWarning() << this << "expected Connect request, got" << _request->command();
        _parent->close();
        return;
    }

    //See about carrying out the client's request.
    //If they gave us an IP, handle it.
    //if they gave us a domain, handle it
    if (_request->addressType() == Socks5RequestMessage::IPv4 ||
            _request->addressType() == Socks5RequestMessage::IPv6)
        this->handleIP();
    else if (_request->addressType() == Socks5RequestMessage::DomainName)
        this->handleDomain();
    else
    {
        qWarning() << this << "got unknown address type in request:" << _request->addressType();
        _parent->close();
        return;
    }
}

void Socks5ConnectState::handleSocketConnected()
{
    //The socket is connected and ready to go. Notify our client and pass control to Socks5ConnectedState

    QSharedPointer<Socks5ReplyMessage> msg(new Socks5ReplyMessage(Socks5ReplyMessage::Success,
                                                                  _socket->localAddress(),
                                                                  _socket->localPort()));
    QString sendErrorString;
    if (!_parent->sendMessage(msg,&sendErrorString))
    {
        qWarning() << "Failed to notify client of successful Connect operation. Error:" << sendErrorString;
        _parent->close();
        return;
    }

    Socks5ConnectedState * nState = new Socks5ConnectedState(new ThrottlingDecorator(_socket),
                                                             _parent);
    _socket->setParent(nState);
    _parent->setState(nState);
    return;
}

//private slot
void Socks5ConnectState::handleSocketError(QAbstractSocket::SocketError error)
{
    Socks5ReplyMessage::Socks5ReplyStatus status;
    if (error == QAbstractSocket::ConnectionRefusedError)
        status = Socks5ReplyMessage::ConnectionRefused;
    else if (error == QAbstractSocket::SocketTimeoutError)
        status = Socks5ReplyMessage::ConnectionRefused;
    else
        status = Socks5ReplyMessage::GeneralError;

    QSharedPointer<Socks5ReplyMessage> msg(new Socks5ReplyMessage(status,
                                                                  QHostAddress::Any,
                                                                  0));
    QString sendError;
    if (!_parent->sendMessage(msg, &sendError))
        qWarning() << "Failed to notify client of remote socket error. Error:" << sendError;
    _parent->close();
    return;
}

//private slot
void Socks5ConnectState::handleDomainLookupResult(const QHostInfo &info)
{
    if (info.lookupId() != _dnsLookupID)
        return;

    //See if there were any results
    if (info.addresses().isEmpty())
    {
        QSharedPointer<Socks5ReplyMessage> msg(new Socks5ReplyMessage(Socks5ReplyMessage::GeneralError,
                                                                      QHostAddress::Any,
                                                                      0));
        QString sendError;
        if (!_parent->sendMessage(msg,&sendError))
            qWarning() << this << "failed to inform client of failed DNS lookup. Error:" << sendError;
        _parent->close();
        return;
    }

    //Choose a result at random and send it to handleIP
    QHostAddress chosen = info.addresses()[qrand() % info.addresses().size()];
    _request->setAddress(chosen);
    this->handleIP();
}


//private
void Socks5ConnectState::handleIP()
{
    //See if it's an address/port that we allow connection to
    bool addressPortOkay = true;
    if (!addressPortOkay)
    {
        QSharedPointer<Socks5ReplyMessage> msg(new Socks5ReplyMessage(Socks5ReplyMessage::DisallowedByRules,
                                                                      QHostAddress::Any,
                                                                      0));
        QString sendError;
        if (!_parent->sendMessage(msg,&sendError))
            qWarning() << this << "failed to inform client of disallowed address/port connect. Error:" << sendError;
        _parent->close();
        return;
    }

    //Start trying to connect to it
    _socket = new QTcpSocket(this);
    connect(_socket,
            SIGNAL(connected()),
            this,
            SLOT(handleSocketConnected()));
    connect(_socket,
            SIGNAL(error(QAbstractSocket::SocketError)),
            this,
            SLOT(handleSocketError(QAbstractSocket::SocketError)));
    _socket->connectToHost(_request->address(),_request->port());
}

//private
void Socks5ConnectState::handleDomain()
{
    //See if it's a domain we allow connection to
    bool domainOkay = true;
    if (!domainOkay)
    {
        QSharedPointer<Socks5ReplyMessage> msg(new Socks5ReplyMessage(Socks5ReplyMessage::DisallowedByRules,
                                                                      QHostAddress::Any,
                                                                      0));
        QString sendError;
        if (!_parent->sendMessage(msg,&sendError))
            qWarning() << this << "failed to inform client of disallowed domain connect. Error:" << sendError;
        _parent->close();
        return;
    }

    //Try to resolve the domain name
    //qDebug() << "Try to resolve" << _request->domainName();
    _dnsLookupID = QHostInfo::lookupHost(_request->domainName(),
                          this,
                          SLOT(handleDomainLookupResult(QHostInfo)));
}
