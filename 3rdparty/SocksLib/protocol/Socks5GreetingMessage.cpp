#include "Socks5GreetingMessage.h"

#include <QDataStream>
#include <QtDebug>

//pure-virtual from SocksProtocolMessage
SocksProtocolMessage::ParseResult Socks5GreetingMessage::parse(QByteArray& bytes, QString *error)
{
    //Check for enough bytes
    if (bytes.length() < this->minimumMessageLength())
    {
        if (error)
            *error = "Not enough bytes";
        return NotEnoughData;
    }

    QDataStream stream(&bytes,QIODevice::ReadOnly);

    //Check for valid SOCKS version
    quint8 versionByte;
    stream >> versionByte;
    SocksProtocolMessage::SocksVersion version;
    if (!SocksProtocolMessage::byte2SocksVersion(versionByte,&version))
    {
        if (error)
            *error = "Invalid SOCKS version " + QString::number(versionByte);
        return Failure;
    }

    if (version == SOCKS4)
    {
        if (error)
            *error = "Got SOCKS4 command but expected SOCKS5. Cannot parse";
        return Failure;
    }

    //Get the number of authentication methods supported
    quint8 numAuthMethodsByte;
    stream >> numAuthMethodsByte;

    if (bytes.size() < numAuthMethodsByte + 2)
    {
        if (error)
            *error = "Not enough bytes in greeting for all authentication methods";
        return NotEnoughData;
    }

    if (numAuthMethodsByte == 0)
    {
        if (error)
            *error = "Greeting specifies zero authentication methods";
        return Failure;
    }

    for (int i = 0; i < numAuthMethodsByte; i++)
    {
        quint8 method;
        stream >> method;
        _authMethods.append(method);
    }

    //Remove the bytes we used
    bytes.remove(0,stream.device()->pos());

    return Success;
}

//pure-virtual from SocksProtocolMessage
bool Socks5GreetingMessage::toBytes(QByteArray *output, QString *error)
{
    //TODO
    Q_UNUSED(output);
    Q_UNUSED(error);
    return false;
}

qint64 Socks5GreetingMessage::minimumMessageLength() const
{
    return 3;
}

QList<quint8> Socks5GreetingMessage::authMethods() const
{
    return _authMethods;
}
