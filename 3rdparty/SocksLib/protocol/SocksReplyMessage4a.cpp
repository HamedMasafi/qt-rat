#include "SocksReplyMessage4a.h"

#include <QDataStream>

Socks5ReplyMessage4a::Socks5ReplyMessage4a(ReplyStatus4a status, QHostAddress address, quint16 port) :
    _status(status), _address(address), _port(port)
{
}

SocksProtocolMessage::ParseResult Socks5ReplyMessage4a::parse(QByteArray &bytes, QString *error)
{
    Q_UNUSED(bytes);
    Q_UNUSED(error);
    return Failure;
}

bool Socks5ReplyMessage4a::toBytes(QByteArray *output, QString *error)
{
    if (output == 0)
    {
        if (error)
            *error = "Null output pointer";
        return false;
    }

    QDataStream stream(output,QIODevice::ReadWrite);

    //Write 0x00 for the version of the reply message... SOCKS4 is lame
    stream << (quint8)0x00;

    //Write status byte
    quint8 statusByte;
    if (!Socks5ReplyMessage4a::replyStatus4a2Byte(_status,&statusByte))
    {
        if (error)
            *error = "Bad status4a byte";
        return false;
    }
    stream << statusByte;

    //Write port
    stream << _port;

    //Write the IP
    stream << _address.toIPv4Address();

    return true;
}

qint64 Socks5ReplyMessage4a::minimumMessageLength() const
{
    return 8;
}

bool Socks5ReplyMessage4a::replyStatus4a2Byte(ReplyStatus4a status, quint8 *output)
{
    if (output == 0)
        return false;

    if (status == Success)
        *output = 0x5a;
    else if (status == RejectedOrFailed)
        *output = 0x5b;
    else if (status == IdentdFailed)
        *output = 0x5c;
    else if (status == IdentdUnauthorized)
        *output = 0x5d;
    else
        return false;
    return true;
}
