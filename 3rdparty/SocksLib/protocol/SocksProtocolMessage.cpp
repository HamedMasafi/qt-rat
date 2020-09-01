#include "SocksProtocolMessage.h"

SocksProtocolMessage::~SocksProtocolMessage()
{
}

//static
bool SocksProtocolMessage::byte2SocksVersion(quint8 byte, SocksVersion *output)
{
    if (output == 0)
        return false;

    if (byte == 0x04)
        *output = SOCKS4;
    else if (byte == 0x05)
        *output = SOCKS5;
    else
        return false;
    return true;
}

//static
bool SocksProtocolMessage::socksVersion2Byte(SocksVersion input, quint8 *output)
{
    if (output == 0)
        return false;

    if (input == SOCKS4)
        *output = 0x04;
    else if (input == SOCKS5)
        *output = 0x05;
    else
        return false;
    return true;
}

//static
bool SocksProtocolMessage::byte2AddressType(quint8 byte, SocksProtocolMessage::AddressType *output)
{
    if (output == 0)
        return false;

    if (byte == 0x01)
        *output = IPv4;
    else if (byte == 0x03)
        *output = DomainName;
    else if (byte == 0x04)
        *output = IPv6;
    else
        return false;
    return true;
}

//static
bool SocksProtocolMessage::addressType2Byte(SocksProtocolMessage::AddressType type, quint8 *output)
{
    if (output == 0)
        return false;

    if (type == IPv4)
        *output = 0x01;
    else if (type == DomainName)
        *output = 0x03;
    else if (type == IPv6)
        *output = 0x04;
    else
        return false;
    return true;
}
