#include "Socks5UDPAssociateState.h"

#include "protocol/Socks5ReplyMessage.h"
#include "Socks5UDPAssociatedState.h"

#include <QUdpSocket>

Socks5UDPAssociateState::Socks5UDPAssociateState(QSharedPointer<Socks5RequestMessage> request,
                                                 SocksConnection *parent) :
    SocksState(parent), _request(request)
{
}

//pure-virtual from SocksState
void Socks5UDPAssociateState::handleIncomingBytes(QByteArray &bytes)
{
    Q_UNUSED(bytes)
}

//virtual from SocksState
void Socks5UDPAssociateState::handleSetAsNewState()
{
    //Check out the request that we've been given
    if (_request.isNull())
    {
        qWarning() << this << "was given null request";
        _parent->close();
        return;
    }

    //Make sure the request is actually a UDPAssociate
    if (_request->command() != Socks5RequestMessage::UDPAssociate)
    {
        qWarning() << this << "was given non-UDPAssociate command";
        _parent->close();
        return;
    }

    qDebug() << "Client requested UDPAssociate";

    //Evaluate the address and port that the client gave us
    if (_request->addressType() == Socks5RequestMessage::IPv4 ||
            _request->addressType() == Socks5RequestMessage::IPv6)
        this->handleIP();
    else if (_request->addressType() == Socks5RequestMessage::DomainName)
        this->handleDomain();
    else
    {
        qWarning() << this << "received request with unknown address type";
        _parent->close();
    }
    return;
}

//private slot
void Socks5UDPAssociateState::handleDomainLookupResult(const QHostInfo &info)
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
void Socks5UDPAssociateState::handleIP()
{
    //Evaluate the IP/port combo --- are we allowed to relay for it?
    bool allowedToRelay = true;

    if (!allowedToRelay)
    {
        QSharedPointer<Socks5ReplyMessage> msg(new Socks5ReplyMessage(Socks5ReplyMessage::DisallowedByRules,
                                                                      QHostAddress::Any,
                                                                      0));
        QString sendError;
        if (!_parent->sendMessage(msg,&sendError))
            qWarning() << this << "failed to notify client of access denied. Error:" << sendError;
        _parent->close();
        return;
    }

    //If we're allowed, just go ahead and try to set it up!
    QUdpSocket::BindMode bindMode = QUdpSocket::DefaultForPlatform;
    QUdpSocket * clientFacingSocket = new QUdpSocket(this);
    QUdpSocket * remoteFacingSocket = new QUdpSocket(this);
    if (!clientFacingSocket->bind(QHostAddress::Any,0,bindMode) ||
            !remoteFacingSocket->bind(QHostAddress::Any,0,bindMode))
    {
        qWarning() << "Failed to bind UDP socket";

        QSharedPointer<Socks5ReplyMessage> msg(new Socks5ReplyMessage(Socks5ReplyMessage::GeneralError,
                                                                     QHostAddress::Any,
                                                                     0));
        QString sendError;
        if (!_parent->sendMessage(msg,&sendError))
            qWarning() << this << "failed to notify client of failed UDP bind";

        _parent->close();
        return;
    }

    //Send a success reply to the client with the ip/port of the client-facing UDP sock
    QSharedPointer<Socks5ReplyMessage> msg(new Socks5ReplyMessage(Socks5ReplyMessage::Success,
                                                                  _parent->myBoundAddress(),
                                                                  clientFacingSocket->localPort()));
    qDebug() << _parent->myBoundAddress();
    QString sendError;
    if (!_parent->sendMessage(msg,&sendError))
    {
        qWarning() << "Failed to send successful UDP bind reply. Error:" << sendError;
        _parent->close();
        return;
    }
    QByteArray bytes;
    msg->toBytes(&bytes);

    //Transition to Socks5UDPAssociatedState
    Socks5UDPAssociatedState * nState = new Socks5UDPAssociatedState(clientFacingSocket,
                                                                     remoteFacingSocket,
                                                                     _parent);
    clientFacingSocket->setParent(nState);
    remoteFacingSocket->setParent(nState);
    _parent->setState(nState);
    return;
}

//private
void Socks5UDPAssociateState::handleDomain()
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
            qWarning() << this << "failed to inform client of disallowed domain UDP bind. Error:" << sendError;
        _parent->close();
        return;
    }

    //Try to resolve the domain name
    _dnsLookupID = QHostInfo::lookupHost(_request->domainName(),
                          this,
                          SLOT(handleDomainLookupResult(QHostInfo)));
}
