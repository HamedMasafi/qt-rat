#ifndef SOCKS4REPLYMESSAGE_H
#define SOCKS4REPLYMESSAGE_H

#include "SocksProtocolMessage.h"

#include <QHostAddress>

class Socks4ReplyMessage : public SocksProtocolMessage
{
public:
    enum Socks4ReplyStatus
    {
        Granted,
        RejectedOrFailed,
        IdentdFailed,
        IdentdRejected
    };

public:
    Socks4ReplyMessage(Socks4ReplyStatus status, QHostAddress address, quint16 port);

    //pure-virtual from SocksProtocolMessage
    virtual ParseResult parse(QByteArray& bytes,QString * error =0);

    //pure-virtual from SocksProtocolMessage
    virtual bool toBytes(QByteArray * output,QString * error=0);

    //pure-virtual from SocksProtocolMessage
    virtual qint64 minimumMessageLength() const;


    static bool socks4ReplyStatus2Byte(Socks4ReplyStatus status, quint8 * output);
    static bool byte2socks4ReplyStatus(quint8 byte, Socks4ReplyStatus * output);


private:
    Socks4ReplyStatus _status;
    QHostAddress _address;
    quint16 _port;
};

#endif // SOCKS4REPLYMESSAGE_H
