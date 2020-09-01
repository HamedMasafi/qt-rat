#ifndef SOCKS5UDPREQUESTMESSAGE_H
#define SOCKS5UDPREQUESTMESSAGE_H

#include <QHostAddress>

#include "SocksProtocolMessage.h"

class Socks5UDPRequestMessage : public SocksProtocolMessage
{
public:
    Socks5UDPRequestMessage(QHostAddress dst, quint16 dstPort, QByteArray data, quint8 fragID=0);
    Socks5UDPRequestMessage(QString domainName, quint16 dstPort, QByteArray data, quint8 fragID=0);
    Socks5UDPRequestMessage();

    //pure-virtual from SocksProtocolMessage
    virtual ParseResult parse(QByteArray& bytes,QString * error =0);

    //pure-virtual from SocksProtocolMessage
    virtual bool toBytes(QByteArray * output,QString * error=0);

    //pure-virtual from SocksProtocolMessage
    virtual qint64 minimumMessageLength() const;

    SocksProtocolMessage::AddressType addressType() const;
    QHostAddress address() const;
    QString domainName() const;
    quint16 port() const;
    QByteArray data() const;

private:
    QHostAddress _address;
    QString _domainName;
    SocksProtocolMessage::AddressType _addressType;
    quint16 _port;
    QByteArray _data;
    quint8 _fragID;
};

#endif // SOCKS5UDPREQUESTMESSAGE_H
