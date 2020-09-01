#ifndef METHODSELECTIONMESSAGE_H
#define METHODSELECTIONMESSAGE_H

#include "SocksProtocolMessage.h"

class Socks5MethodSelectionMessage : public SocksProtocolMessage
{
public:
    Socks5MethodSelectionMessage(quint8 authMethod);

    //pure-virtual from SocksProtocolMessage
    virtual ParseResult parse(QByteArray &bytes,QString * error =0);

    //pure-virtual from SocksProtocolMessage
    virtual bool toBytes(QByteArray *output, QString *error);

    //pure-virtual from SocksProtocolMessage
    virtual qint64 minimumMessageLength() const;

private:
    quint8 _authMethod;
};

#endif // METHODSELECTIONMESSAGE_H
