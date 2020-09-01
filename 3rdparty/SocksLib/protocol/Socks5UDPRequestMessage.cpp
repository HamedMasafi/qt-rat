#include "Socks5UDPRequestMessage.h"

#include <QDataStream>

Socks5UDPRequestMessage::Socks5UDPRequestMessage(QHostAddress dst,
                                                 quint16 dstPort,
                                                 QByteArray data,
                                                 quint8 fragID) :
    _address(dst), _port(dstPort), _data(data), _fragID(fragID)
{
    if (_address.protocol() == QAbstractSocket::IPv4Protocol)
        _addressType = SocksProtocolMessage::IPv4;
    else if (_address.protocol() == QAbstractSocket::IPv6Protocol)
        _addressType = SocksProtocolMessage::IPv6;
}

Socks5UDPRequestMessage::Socks5UDPRequestMessage(QString domainName,
                                                 quint16 dstPort,
                                                 QByteArray data,
                                                 quint8 fragID) :
    _domainName(domainName), _port(dstPort), _data(data), _fragID(fragID)
{
    _addressType = SocksProtocolMessage::DomainName;
}

Socks5UDPRequestMessage::Socks5UDPRequestMessage()
{
}

//pure-virtual from SocksProtocolMessage
SocksProtocolMessage::ParseResult Socks5UDPRequestMessage::parse(QByteArray &bytes, QString *error)
{
    //Failure instead of NotEnoughBytes because we expect to receive this in a UDP datagram
    if (bytes.size() < this->minimumMessageLength())
    {
        if (error)
            *error = "Not enough bytes";
        return Failure;
    }

    QDataStream stream(bytes);

    //Check for the first two null bytes
    quint16 reservedNullBytes;
    stream >> reservedNullBytes;
    if (reservedNullBytes != 0)
    {
        if (error)
            *error = "Expected null bytes, received something else";
        return Failure;
    }

    //Parse the frag id/number
    stream >> _fragID;

    //Parse the address type
    quint8 addressTypeByte;
    stream >> addressTypeByte;
    if (!SocksProtocolMessage::byte2AddressType(addressTypeByte,&_addressType))
    {
        if (error)
            *error = "Incorrect address type";
        return Failure;
    }

    //Get the dest address
    if (this->addressType() == SocksProtocolMessage::IPv4)
    {
        quint32 ipBytes;
        stream >> ipBytes;
        _address = QHostAddress(ipBytes);
        if (_address.isNull())
        {
            if (error)
                *error = "Failed to parse IPv4 address";
            return Failure;
        }
    }
    else if (this->addressType() == SocksProtocolMessage::IPv6)
    {
        if (bytes.length() < 23)
        {
            if (error)
                *error = "Insufficient bytes for request with IPv6 address";
            return Failure;
        }
        quint8 * ipv6buf = new quint8[16];
        int read = stream.readRawData((char*)ipv6buf,16);
        if (read != 16)
        {
            if (error)
                *error = "Failed to read IPv6 address bytes";
            delete[] ipv6buf;
            return Failure;
        }
        _address = QHostAddress(ipv6buf);
        delete[] ipv6buf;
        if (_address.isNull())
        {
            if (error)
                *error = "Failed to parse IPv6 address";
            return Failure;
        }
    }
    else if (this->addressType() == SocksProtocolMessage::DomainName)
    {
        quint8 nameLengthByte;
        stream >> nameLengthByte;
        if (bytes.length() < 8 + nameLengthByte)
        {
            if (error)
                *error = "Insufficient bytes for request with domain name";
            return Failure;
        }
        QByteArray domainName = QByteArray(nameLengthByte,(char)0);
        if (stream.readRawData(domainName.data(),nameLengthByte) != nameLengthByte)
        {
            if (error)
                *error = "Failed to parse domain name from request";
            return Failure;
        }
        _domainName = QString(domainName);
    }

    //Parse port
    stream >> _port;

    bytes.remove(0,stream.device()->pos());
    _data = bytes;

    return Success;
}

//pure-virtual from SocksProtocolMessage
bool Socks5UDPRequestMessage::toBytes(QByteArray *output, QString *error)
{
    if (output == 0)
    {
        if (error)
            *error = "Null output pointer";
        return false;
    }

    QDataStream stream(output, QIODevice::ReadWrite);

    //Write the two null reserved bytes
    quint16 reservedNullBytes = 0;
    stream << reservedNullBytes;

    //Write the frag byte
    stream << _fragID;

    //Write the address type
    quint8 addressTypeByte;
    if (!SocksProtocolMessage::addressType2Byte(_addressType,&addressTypeByte))
    {
        if (error)
            *error = "Invalid address type";
        return false;
    }
    stream << addressTypeByte;

    //Write the address
    if (_addressType == SocksProtocolMessage::IPv4)
    {
        quint32 ipv4 = _address.toIPv4Address();
        stream << ipv4;
    }
    else if (_addressType == SocksProtocolMessage::IPv6)
    {
        Q_IPV6ADDR ipv6 = _address.toIPv6Address();
        for (int i = 0; i < 16; i++)
            stream << ipv6[i];
    }
    else if (_addressType == SocksProtocolMessage::DomainName)
    {
        if (error)
            *error = "Serializing domain name for UDP relay unsupported";
        return false;
    }
    else
    {
        if (error)
            *error = "Unhandled address type";
        return false;
    }

    //Write the port
    stream << _port;

    //Write the data

    //Don't use the stream to write the QByteArray!
    //stream << _data;
    //Do this instead:
    output->append(_data);

    return true;
}

//pure-virtual from SocksProtocolMessage
qint64 Socks5UDPRequestMessage::minimumMessageLength() const
{
    return 11;
}

SocksProtocolMessage::AddressType Socks5UDPRequestMessage::addressType() const
{
    return _addressType;
}

QHostAddress Socks5UDPRequestMessage::address() const
{
    return _address;
}

QString Socks5UDPRequestMessage::domainName() const
{
    return _domainName;
}

quint16 Socks5UDPRequestMessage::port() const
{
    return _port;
}

QByteArray Socks5UDPRequestMessage::data() const
{
    return _data;
}
