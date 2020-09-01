#ifndef Socks5RequestMessage_H
#define Socks5RequestMessage_H

#include "SocksProtocolMessage.h"

#include <QHostAddress>

class Socks5RequestMessage : public SocksProtocolMessage
{
public:
    enum Socks5Command
    {
        Connect = 0x01,
        Bind = 0x02,
        UDPAssociate = 0x03
    };

public:
    //pure-virtual from SocksProtocolMessage
    virtual ParseResult parse(QByteArray &bytes,QString * error =0);

    //pure-virtual from SocksProtocolMessage
    virtual bool toBytes(QByteArray * output, QString * error=0);

    //pure-virtual from SocksProtocolMessage
    virtual qint64 minimumMessageLength() const;

    static bool byte2Socks5Command(quint8 byte, Socks5Command * output);
    static bool socks5Command2Byte(Socks5Command command, quint8 * output);

    Socks5Command command() const;
    SocksProtocolMessage::AddressType addressType() const;
    QHostAddress address() const;
    void setAddress(const QHostAddress& nAddress);
    QString domainName() const;
    quint16 port() const;


private:
    Socks5Command _command;
    SocksProtocolMessage::AddressType _addressType;
    QHostAddress _address;
    QString _domainName;
    quint16 _port;
};

#endif // Socks5RequestMessage_H
