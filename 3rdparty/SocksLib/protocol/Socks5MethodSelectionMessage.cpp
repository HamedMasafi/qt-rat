#include "Socks5MethodSelectionMessage.h"

#include <QDataStream>

Socks5MethodSelectionMessage::Socks5MethodSelectionMessage(quint8 authMethod)
{
    _authMethod = authMethod;
}

SocksProtocolMessage::ParseResult Socks5MethodSelectionMessage::parse(QByteArray &bytes, QString *error)
{
    Q_UNUSED(bytes)
    Q_UNUSED(error)
    return Failure;
}

bool Socks5MethodSelectionMessage::toBytes(QByteArray * output, QString *error)
{
    if (output == 0)
    {
        if (error)
            *error = "No output pointer";
        return false;
    }

    //Convert the version enum to a byte, make sure it's okay
    quint8 socksVersion;
    if (!SocksProtocolMessage::socksVersion2Byte(SOCKS5,&socksVersion))
    {
        if (error)
            *error = "Invalid socks version Socks5MethodSelectionMessage";
        return false;
    }

    //Output everything to the client-supplied QByteArray
    QDataStream stream(output,QIODevice::ReadWrite);
    stream << socksVersion;
    stream << _authMethod;
    return true;
}

qint64 Socks5MethodSelectionMessage::minimumMessageLength() const
{
    return 2;
}
