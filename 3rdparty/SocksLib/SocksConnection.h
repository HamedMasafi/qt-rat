#ifndef SOCKSCONNECTION_H
#define SOCKSCONNECTION_H

#include <QObject>
#include <QPointer>
#include <QAbstractSocket>
#include <QTcpSocket>
#include <QByteArray>
#include <QSharedPointer>
#include <QIODevice>

#include "protocol/SocksProtocolMessage.h"

class SocksState;

class SocksConnection : public QObject
{
    Q_OBJECT
public:
    explicit SocksConnection(QAbstractSocket * socket, QObject *parent = 0);
    virtual ~SocksConnection();

    QPointer<SocksState> connectionState();

    SocksProtocolMessage::SocksVersion socksVersion() const;
    void setSocksVersion(SocksProtocolMessage::SocksVersion);
    bool socksVersionSet() const;

    bool sendMessage(QSharedPointer<SocksProtocolMessage> msg, QString * error=0);

    QHostAddress myBoundAddress() const;
    QHostAddress peerAddress() const;
    
signals:
    
public slots:
    void sendData(const QByteArray& toSend);
    void setState(SocksState * nState);
    void close();

protected slots:
    void handleIncomingBytes(QByteArray &bytes);

private slots:
    void handleReadyRead();
    void handleSocketClosed();

private:
    QPointer<SocksState> _connectionState;
    QPointer<QAbstractSocket> _rawSocket;
    QPointer<QIODevice> _socket;
    QByteArray _recvBuffer;

    bool _socksVersionSet;
    SocksProtocolMessage::SocksVersion _socksVersion;
    
};

#endif // SOCKSCONNECTION_H
