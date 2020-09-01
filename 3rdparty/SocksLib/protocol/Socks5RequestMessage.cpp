#include "Socks5RequestMessage.h"

#include <QString>
#include <QDataStream>
#include <QtDebug>
#include <QString>

//pure-virtual from SocksProtocolMessage
SocksProtocolMessage::ParseResult Socks5RequestMessage::parse(QByteArray &bytes,QString * error)
{
    //Check for enough bytes
    if (bytes.length() < this->minimumMessageLength())
    {
        if (error)
            *error = "Not enough bytes";
        return NotEnoughData;
    }

    QDataStream stream(&bytes,QIODevice::ReadOnly);

    //Get the socks version number
    quint8 socksVersionByte;
    stream >> socksVersionByte;
    SocksProtocolMessage::SocksVersion  socksVersion;
    if (!SocksProtocolMessage::byte2SocksVersion(socksVersionByte,&socksVersion))
    {
        if (error)
            *error = "Invalid socks version in Socks5RequestMessage";
        qDebug() << bytes.toHex();
        return Failure;
    }

    //Make sure it's Socks5
    if (socksVersion != SocksProtocolMessage::SOCKS5)
    {
        if (error)
            *error = "Got SOCKSv4 message, expected v5!";
        return Failure;
    }

    //Get the command
    quint8 socksCommandByte;
    stream >> socksCommandByte;
    if (!Socks5RequestMessage::byte2Socks5Command(socksCommandByte,&_command))
    {
        if (error)
            *error = "Invalid socks command " + QString::number(socksCommandByte);
        return Failure;
    }

    //Check the reserved byte
    quint8 reservedByte;
    stream >> reservedByte;
    if (reservedByte != 0x00)
        qWarning() << "Reserved byte != 0x00";

    //Get the address type
    quint8 addressTypeByte;
    stream >> addressTypeByte;
    if (!SocksProtocolMessage::byte2AddressType(addressTypeByte,&_addressType))
    {
        if (error)
            *error = "Invalid address type " + QString::number(addressTypeByte);
        return Failure;
    }

    //Get the dest address
    if (this->addressType() == SocksProtocolMessage::IPv4)
    {
        if (bytes.length() < 10)
        {
            if (error)
                *error = "Insufficient bytes for request with IPv4 address";
            return NotEnoughData;
        }
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
        if (bytes.length() < 22)
        {
            if (error)
                *error = "Insufficient bytes for request with IPv6 address";
            return NotEnoughData;
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
        if (bytes.length() < 7 + nameLengthByte)
        {
            if (error)
                *error = "Insufficient bytes for request with domain name";
            return NotEnoughData;
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

    //Now parse the port
    stream >> _port;

    //Eat all the bytes we used
    bytes.remove(0,stream.device()->pos());

    return Success;
}

//pure-virtual from SocksProtocolMessage
bool Socks5RequestMessage::toBytes(QByteArray * output, QString * error)
{
    Q_UNUSED(output);
    Q_UNUSED(error);
    return false;
}

//pure-virtual from SocksProtocolMessage
qint64 Socks5RequestMessage::minimumMessageLength() const
{
    return 8;
}

//static
bool Socks5RequestMessage::byte2Socks5Command(quint8 byte, Socks5RequestMessage::Socks5Command *output)
{
    if (output == 0)
        return false;


    if (byte == 0x01)
        *output = Connect;
    else if (byte == 0x02)
        *output = Bind;
    else if (byte == 0x03)
        *output = UDPAssociate;
    else
        return false;
    return true;
}

//static
bool Socks5RequestMessage::socks5Command2Byte(Socks5RequestMessage::Socks5Command command, quint8 *output)
{
    if (output == 0)
        return false;

    if (command == Connect)
        *output = 0x01;
    else if (command == Bind)
        *output = 0x02;
    else if (command == UDPAssociate)
        *output = 0x03;
    else
        return false;
    return true;
}

Socks5RequestMessage::Socks5Command Socks5RequestMessage::command() const
{
    return _command;
}

Socks5RequestMessage::AddressType Socks5RequestMessage::addressType() const
{
    return _addressType;
}

QHostAddress Socks5RequestMessage::address() const
{
    return _address;
}

void Socks5RequestMessage::setAddress(const QHostAddress &nAddress)
{
    _address = nAddress;
}

QString Socks5RequestMessage::domainName() const
{
    return _domainName;
}

quint16 Socks5RequestMessage::port() const
{
    return _port;
}
