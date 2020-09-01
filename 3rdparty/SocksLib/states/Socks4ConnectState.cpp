#include "Socks4ConnectState.h"

#include "protocol/Socks4ReplyMessage.h"
#include "Socks4ConnectedState.h"

#include "decorators/ThrottlingDecorator.h"

Socks4ConnectState::Socks4ConnectState(QSharedPointer<Socks4RequestMessage> request, SocksConnection *parent) :
    SocksState(parent), _request(request)
{
}

void Socks4ConnectState::handleIncomingBytes(QByteArray &bytes)
{
    Q_UNUSED(bytes)
}

void Socks4ConnectState::handleSetAsNewState()
{
    if (_request.isNull())
    {
        qWarning() << this << "got null request";
        _parent->close();
        return;
    }

    //Verify that the request is a Connect request
    if (_request->command() != Socks4RequestMessage::Connect)
    {
        qWarning() << this << "received non-connect request";
        _parent->close();
        return;
    }

    //See if we got an IP or a domain request. Handle whichever it is
    if (_request->addressType() == Socks4RequestMessage::IPv4)
        this->handleIP();
    else if (_request->addressType() == Socks4RequestMessage::DomainName)
        this->handleDomain();
    else
    {
        qWarning() << this << "received request with unsupported address type";
        _parent->close();
        return;
    }

}

//private slot
void Socks4ConnectState::handleSocketConnected()
{
    //The socket is connected and ready to go. Notify our client and pass control to Socks5ConnectedState

    QSharedPointer<Socks4ReplyMessage> msg(new Socks4ReplyMessage(Socks4ReplyMessage::Granted,
                                                                  _socket->localAddress(),
                                                                  _socket->localPort()));
    QString sendErrorString;
    if (!_parent->sendMessage(msg,&sendErrorString))
    {
        qWarning() << "Failed to notify client of successful Connect operation. Error:" << sendErrorString;
        _parent->close();
        return;
    }

    Socks4ConnectedState * nState = new Socks4ConnectedState(new ThrottlingDecorator(_socket),
                                                             _parent);
    _socket->setParent(nState);
    _parent->setState(nState);
    return;
}

//private slot
void Socks4ConnectState::handleSocketError(QAbstractSocket::SocketError)
{
    Socks4ReplyMessage::Socks4ReplyStatus status = Socks4ReplyMessage::RejectedOrFailed;

    QSharedPointer<Socks4ReplyMessage> msg(new Socks4ReplyMessage(status,
                                                                  QHostAddress::Any,
                                                                  0));
    QString sendError;
    if (!_parent->sendMessage(msg, &sendError))
        qWarning() << this << "Failed to notify client of remote socket error. Error:" << sendError;
    _parent->close();
    return;
}

//private slot
void Socks4ConnectState::handleDomainLookupResult(const QHostInfo &info)
{
    //Make sure this result is for us
    if (info.lookupId() != _dnsLookupID)
        return;

    //Make sure we got a result!
    if (info.addresses().isEmpty())
    {
        QSharedPointer<Socks4ReplyMessage> msg(new Socks4ReplyMessage(Socks4ReplyMessage::RejectedOrFailed,
                                                                      QHostAddress::Any,
                                                                      0));
        if (!_parent->sendMessage(msg))
            qWarning() << this << "failed to notify client of DNS lookup failure";
        _parent->close();
        return;
    }

    //Use one of the addresses and pass control to handleIP!
    QHostAddress result = info.addresses()[qrand() % info.addresses().size()];
    _request->setAddress(result);
    this->handleIP();
}

//private
void Socks4ConnectState::handleIP()
{
    //See if it's an address/port combo that we're allowed to connect to
    bool addressPortOkay = true;
    if (!addressPortOkay)
    {
        QSharedPointer<Socks4ReplyMessage> msg(new Socks4ReplyMessage(Socks4ReplyMessage::RejectedOrFailed,
                                                                      QHostAddress::Any,
                                                                      0));
        QString sendError;
        if (!_parent->sendMessage(msg,&sendError))
            qWarning() << this << "failed to notify client of request rejection. Error:" << sendError;
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
void Socks4ConnectState::handleDomain()
{
    //See if the domain is one we're allowed to do a Connect to
    bool isAllowed = true;
    if (!isAllowed)
    {
        QSharedPointer<Socks4ReplyMessage> msg(new Socks4ReplyMessage(Socks4ReplyMessage::RejectedOrFailed,
                                                                      QHostAddress::Any,
                                                                      0));
        QString sendError;
        if (!_parent->sendMessage(msg,&sendError))
            qWarning() << this << "failed to notify client of request rejection. Error:" << sendError;
        _parent->close();
        return;
    }

    //Do a lookup
    //qDebug() << "Try to resolve" << _request->domainName();
    _dnsLookupID = QHostInfo::lookupHost(_request->domainName(),
                                         this,
                                         SLOT(handleDomainLookupResult(QHostInfo)));
}
