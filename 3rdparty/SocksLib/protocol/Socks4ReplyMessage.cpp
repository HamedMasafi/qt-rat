#include "Socks4ReplyMessage.h"

#include <QDataStream>

Socks4ReplyMessage::Socks4ReplyMessage(Socks4ReplyStatus status, QHostAddress address, quint16 port) :
    _status(status), _address(address), _port(port)
{
}

SocksProtocolMessage::ParseResult Socks4ReplyMessage::parse(QByteArray &bytes, QString *error)
{
    Q_UNUSED(bytes)
    Q_UNUSED(error)
    return Failure;
}

bool Socks4ReplyMessage::toBytes(QByteArray *output, QString *error)
{
    if (output == 0)
    {
        if (error)
            *error = "Null output pointer";
        return false;
    }

    QDataStream stream(output, QIODevice::ReadWrite);

    //Write the first null byte that SOCKS4 requires
    quint8 VN = 0;
    stream << VN;

    //Write the result of the command
    quint8 CD;
    if (!Socks4ReplyMessage::socks4ReplyStatus2Byte(_status,&CD))
    {
        if (error)
            *error = "Invalid Reply Status";
        return false;
    }
    stream << CD;

    //two arbitrary bytes where the port would go
    stream << _port;

    //Four arbitrary bytes where the IP would go in a bind request
    quint32 ipBytes = _address.toIPv4Address();
    stream << ipBytes;

    return true;
}

qint64 Socks4ReplyMessage::minimumMessageLength() const
{
    return 8;
}

//static
bool Socks4ReplyMessage::socks4ReplyStatus2Byte(Socks4ReplyMessage::Socks4ReplyStatus status, quint8 *output)
{
    if (output == 0)
        return false;

    if (status == Granted)
        *output = 0x5a;
    else if (status == RejectedOrFailed)
        *output = 0x5b;
    else if (status == IdentdFailed)
        *output = 0x5c;
    else if (status == IdentdRejected)
        *output = 0x5d;
    else
        return false;
    return true;
}

//static
bool Socks4ReplyMessage::byte2socks4ReplyStatus(quint8 byte, Socks4ReplyMessage::Socks4ReplyStatus *output)
{
    if (output == 0)
        return false;

    if (byte == 0x5a)
        *output = Granted;
    else if (byte == 0x5b)
        *output = RejectedOrFailed;
    else if (byte == 0x5c)
        *output = IdentdFailed;
    else if (byte == 0x5d)
        *output = IdentdRejected;
    else
        return false;
    return true;
}
