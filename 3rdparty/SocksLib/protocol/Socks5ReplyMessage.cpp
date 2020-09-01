#include "Socks5ReplyMessage.h"

#include <QDataStream>

Socks5ReplyMessage::Socks5ReplyMessage(Socks5ReplyStatus replyStatus,
                                     QHostAddress myBindAddress,
                                     quint16 myBindPort) :
    _replyStatus(replyStatus), _myBindAddress(myBindAddress), _myBindPort(myBindPort)
{
}

//pure-virtual from SocksProtocolMessage
SocksProtocolMessage::ParseResult Socks5ReplyMessage::parse(QByteArray &bytes, QString *error)
{
    Q_UNUSED(bytes)
    Q_UNUSED(error)
    return Failure;
}

//pure-virtual from SocksProtocolMessage
bool Socks5ReplyMessage::toBytes(QByteArray *output, QString *error)
{
    if (output == 0)
    {
        if (error)
            *error = "Null output pointer";
        return false;
    }

    QDataStream stream(output,QIODevice::ReadWrite);

    //Write SOCKS version
    quint8 versionByte;
    if (!SocksProtocolMessage::socksVersion2Byte(SocksProtocolMessage::SOCKS5,&versionByte))
    {
        if (error)
            *error = "Bad SOCKS version byte";
        return false;
    }
    stream << versionByte;

    //Write status (success, etc.)
    quint8 statusByte;
    if (!Socks5ReplyMessage::socks5ReplyStatus2Byte(_replyStatus,&statusByte))
    {
        if (error)
            *error = "Bad reply status";
        return false;
    }
    stream << statusByte;

    //Write reserved null byte
    quint8 reservedByte = 0x00;
    stream << reservedByte;

    //Write address type byte
    quint8 addressTypeByte;
    SocksProtocolMessage::AddressType addressType;
    if (_myBindAddress.protocol() == QAbstractSocket::IPv4Protocol)
        addressType = SocksProtocolMessage::IPv4;
    else if (_myBindAddress.protocol() == QAbstractSocket::IPv6Protocol)
        addressType = SocksProtocolMessage::IPv6;
    else
    {
        if (error)
            *error = "Bad bind address - unknown protocol";
        return false;
    }

    if (!SocksProtocolMessage::addressType2Byte(addressType,&addressTypeByte))
    {
        if (error)
            *error = "Bad address type";
        return false;
    }
    stream << addressTypeByte;

    //Write address
    if (addressType == SocksProtocolMessage::IPv4)
        stream << _myBindAddress.toIPv4Address();
    else if (addressType == SocksProtocolMessage::IPv6)
    {
        Q_IPV6ADDR addr = _myBindAddress.toIPv6Address();
        for (int i = 0; i < 16; i++)
            stream << addr[i];
    }
    else
    {
        if (error)
            *error = "Unknown address type -- can't output address";
        return false;
    }

    //Write bind port
    stream << _myBindPort;

    return true;
}

//pure-virtual from SocksProtocolMessage
qint64 Socks5ReplyMessage::minimumMessageLength() const
{
    return 10;
}

//static
bool Socks5ReplyMessage::socks5ReplyStatus2Byte(Socks5ReplyMessage::Socks5ReplyStatus status, quint8 *output)
{
    if (output == 0)
        return false;

    if (status == Success)
        *output = 0x00;
    else if (status == GeneralError)
        *output = 0x01;
    else if (status == DisallowedByRules)
        *output = 0x02;
    else if (status == NetworkUnreachable)
        *output = 0x03;
    else if (status == HostUnreachable)
        *output = 0x04;
    else if (status == ConnectionRefused)
        *output = 0x05;
    else if (status == TTLExpired)
        *output = 0x06;
    else if (status == CommandNotSupported)
        *output = 0x07;
    else if (status == AddressTypeNotSupported)
        *output = 0x08;
    else
        return false;
    return true;
}

//static
bool Socks5ReplyMessage::byte2socks5ReplyStatus(quint8 byte, Socks5ReplyMessage::Socks5ReplyStatus *output)
{
    if (output == 0)
        return false;

    if (byte == 0x00)
        *output = Success;
    else if (byte == 0x01)
        *output = GeneralError;
    else if (byte == 0x02)
        *output = DisallowedByRules;
    else if (byte == 0x03)
        *output = NetworkUnreachable;
    else if (byte == 0x04)
        *output = HostUnreachable;
    else if (byte == 0x05)
        *output = ConnectionRefused;
    else if (byte == 0x06)
        *output = TTLExpired;
    else if (byte == 0x07)
        *output = CommandNotSupported;
    else if (byte == 0x08)
        *output = AddressTypeNotSupported;
    else
        return false;
    return true;
}
