#ifndef QTCPSOCKETDECORATOR_H
#define QTCPSOCKETDECORATOR_H

#include <QHostAddress>
#include <QTcpSocket>
#include <QPointer>

#include "SocksLib_global.h"

class SOCKSLIBSHARED_EXPORT QTcpSocketDecorator : public QTcpSocket
{
    Q_OBJECT
public:
    explicit QTcpSocketDecorator(QPointer<QTcpSocket> decorated, QObject *parent = 0);
    explicit QTcpSocketDecorator(QObject * parent = 0);
    virtual ~QTcpSocketDecorator();

    //virtual from QAbstractSocket
    virtual bool atEnd() const;
    virtual qint64 bytesAvailable() const;
    virtual qint64 bytesToWrite() const;
    virtual bool canReadLine() const;
    virtual void close();
    virtual bool isSequential() const;
    virtual bool waitForBytesWritten(int msecs = 30000);
    virtual bool waitForReadyRead(int msecs = 30000);

protected:
    //pure-virtual from QIODevice
    virtual qint64 readData(char *data, qint64 maxlen);
    virtual qint64 readLineData(char *data, qint64 maxlen);
    virtual qint64 writeData(const char *data, qint64 len);

    QPointer<QTcpSocket> decorated() const;

private:
    QPointer<QTcpSocket> _decorated;

    void connectSignalsAndSlots();
    void updateAddressesFromDecorated();

    
signals:
    
public slots:

protected slots:
    //These catch signals from QAbstractSocket
    virtual void handleDecoratedConnected();
    virtual void handleDecoratedDisconnected();
    virtual void handleDecoratedError(QAbstractSocket::SocketError error);
    virtual void handleDecoratedHostFound();
    virtual void handleDecoratedProxyAuthenticationRequired(const QNetworkProxy& proxy,
                                                    QAuthenticator * authenticator);
    virtual void handleDecoratedStateChanged(QAbstractSocket::SocketState state);

    //These catch signals from QIODevice
    virtual void handleDecoratedAboutToClose();
    virtual void handleDecoratedBytesWritten(qint64 bytes);
    virtual void handleDecoreatedReadChannelFinished();
    virtual void handleDecoratedReadyRead();

    //Catch signal from QObjet
    virtual void handleDecoratedDestroyed(QObject * obj=0);

    virtual void connectToHostImplementation(const QString &hostName, quint16 port, OpenMode mode);
    virtual void disconnectFromHostImplementation();
    
};

#endif // QTCPSOCKETDECORATOR_H
