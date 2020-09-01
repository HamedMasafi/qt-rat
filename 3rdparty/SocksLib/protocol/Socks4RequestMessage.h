#ifndef SOCKS4REQUESTMESSAGE_H
#define SOCKS4REQUESTMESSAGE_H

#include "SocksProtocolMessage.h"

#include <QHostAddress>

class Socks4RequestMessage : public SocksProtocolMessage
{
public:
    enum Socks4Command
    {
        Connect = 0x01,
        Bind = 0x02
    };

public:
    //pure-virtual from SocksProtocolMessage
    virtual ParseResult parse(QByteArray &bytes,QString * error =0);

    //pure-virtual from SocksProtocolMessage
    virtual bool toBytes(QByteArray * output, QString * error=0);

    //pure-virtual from SocksProtocolMessage
    virtual qint64 minimumMessageLength() const;

    Socks4Command command() const;
    SocksProtocolMessage::AddressType addressType() const;
    QHostAddress address() const;
    void setAddress(const QHostAddress&);
    QString domainName() const;
    quint16 port() const;

    static bool byte2Socks4Command(quint8 byte, Socks4Command * output);
    static bool socks4Command2Byte(Socks4Command command, quint8 * output);

private:
    Socks4Command _command;
    SocksProtocolMessage::AddressType _addressType;
    QHostAddress _address;
    QString _domainName;
    quint16 _port;
};

#endif // SOCKS4REQUESTMESSAGE_H
