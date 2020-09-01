#include "Socks4RequestMessage.h"

#include <QDataStream>
#include <QString>

SocksProtocolMessage::ParseResult Socks4RequestMessage::parse(QByteArray &bytes, QString *error)
{
    if (bytes.size() < this->minimumMessageLength())
    {
        if (error)
            *error = "Cannnot parse SocksProtocolMessage. Not enough bytes";
        return NotEnoughData;
    }

    //Make sure version number is correct
    QDataStream stream(&bytes,QIODevice::ReadOnly);

    //Get the socks version number
    quint8 socksVersionByte;
    stream >> socksVersionByte;
    SocksProtocolMessage::SocksVersion  socksVersion;
    if (!SocksProtocolMessage::byte2SocksVersion(socksVersionByte,&socksVersion))
    {
        if (error)
            *error = "Invalid socks version in Socks4RequestMessage";
        return Failure;
    }

    //Make sure this is the SOCKS version we expect
    if (socksVersion != SocksProtocolMessage::SOCKS4)
    {
        if (error)
            *error = "Expected SOCKSv4 message, got something else";
        return Failure;
    }

    //Check command
    quint8 socksCommandByte;
    stream >> socksCommandByte;

    if (!Socks4RequestMessage::byte2Socks4Command(socksCommandByte,&_command))
    {
        if (error)
            *error = "Invalid socks command " + QString::number(socksCommandByte);
        return Failure;
    }

    //Now parse the port
    stream >> _port;

    //Parse the IP address. If it's 0.0.0.x, then we know to expect a DomainName at the end, as per SOCKSv4A
    quint32 ipv4Bytes;
    stream >> ipv4Bytes;
    QHostAddress v4(ipv4Bytes);
    if (((ipv4Bytes & 0x000000ff) == ipv4Bytes) && (ipv4Bytes > 0))
        _addressType = SocksProtocolMessage::DomainName;
    else
    {
        _addressType = SocksProtocolMessage::IPv4;
        _address = v4;
    }

    const int userIDIndex = 8;
    //Get the userID
    //First find the length of the userid WITHOUT its null
    int indexFirstNull = bytes.indexOf((char)0,userIDIndex);
    if (indexFirstNull == -1)
    {
        if (error)
            *error = "Not enough bytes for SOCKS4 userid";
        return NotEnoughData;
    }
    int userIDLength = indexFirstNull - userIDIndex;

    //Eat enough bytes for the userID and its null. We don't care about it
    stream.skipRawData(userIDLength + 1);

    //If there's to be no domain name (as per socks4a), we return successfully
    if (this->addressType() == IPv4)
    {
        //Eat the bytes
        bytes.remove(0,stream.device()->pos());
        return Success;
    }

    //If we haven't got another null yet, we must not yet have received the whole domain name
    int indexSecondNull = bytes.indexOf((char)0,indexFirstNull + 1);
    if (indexSecondNull == -1)
    {
        if (error)
            *error ="Not enough bytes for SOCKS4a domain name";
        return NotEnoughData;
    }

    //The length of the domain name WITHOUT its null
    int domainNameLength = indexSecondNull - indexFirstNull + userIDLength - 1;

    //Fetch the socks4a domain name
    char * domainNameBytes = new char[domainNameLength];
    stream.readRawData(domainNameBytes,domainNameLength);
    const QString domainName = QString::fromLatin1(domainNameBytes,domainNameLength);
    delete[] domainNameBytes;

    _domainName = domainName;

    //Eat the zero
    stream.skipRawData(1);

    //Eat the bytes
    bytes.remove(0,stream.device()->pos());

    return Success;
}

bool Socks4RequestMessage::toBytes(QByteArray *output, QString *error)
{
    Q_UNUSED(output)
    Q_UNUSED(error)
    return false;
}

qint64 Socks4RequestMessage::minimumMessageLength() const
{
    return 9;
}

Socks4RequestMessage::Socks4Command Socks4RequestMessage::command() const
{
    return _command;
}

SocksProtocolMessage::AddressType Socks4RequestMessage::addressType() const
{
    return _addressType;
}

QHostAddress Socks4RequestMessage::address() const
{
    return _address;
}

void Socks4RequestMessage::setAddress(const QHostAddress & address)
{
    _address = address;
}

QString Socks4RequestMessage::domainName() const
{
    return _domainName;
}

quint16 Socks4RequestMessage::port() const
{
    return _port;
}

//static
bool Socks4RequestMessage::byte2Socks4Command(quint8 byte, Socks4RequestMessage::Socks4Command *output)
{
    if (output == 0)
        return false;

    if (byte == 0x01)
        *output = Connect;
    else if (byte == 0x02)
        *output = Bind;
    else
        return false;
    return true;
}

//static
bool Socks4RequestMessage::socks4Command2Byte(Socks4RequestMessage::Socks4Command command, quint8 *output)
{
    if (output == 0)
        return false;

    if (command == Connect)
        *output = 0x01;
    else if (command == Bind)
        *output = 0x02;
    else
        return false;
    return true;
}
