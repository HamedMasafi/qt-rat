#include "QTcpSocketDecorator.h"

#include <QtDebug>

QTcpSocketDecorator::QTcpSocketDecorator(QPointer<QTcpSocket> decorated, QObject *parent) :
    QTcpSocket(parent)
{
    if (decorated.isNull())
        decorated = new QTcpSocket(this);
    else
        decorated->setParent(this);

    _decorated = decorated;

    this->connectSignalsAndSlots();
}

QTcpSocketDecorator::QTcpSocketDecorator(QObject *parent) :
    QTcpSocket(parent)
{
    _decorated = new QTcpSocket(this);

    this->connectSignalsAndSlots();
}

QTcpSocketDecorator::~QTcpSocketDecorator()
{
}

//virtual from QAbstractSocket
bool QTcpSocketDecorator::atEnd() const
{
    return _decorated->atEnd();
}

//virtual from QAbstractSocket
qint64 QTcpSocketDecorator::bytesAvailable() const
{
    return _decorated->bytesAvailable();
}

//virtual from QAbstractSocket
qint64 QTcpSocketDecorator::bytesToWrite() const
{
    return _decorated->bytesToWrite();
}

//virtual from QAbstractSocket
bool QTcpSocketDecorator::canReadLine() const
{
    return _decorated->canReadLine();
}

//virtual from QAbstractSocket
void QTcpSocketDecorator::close()
{
    return _decorated->close();
}

//virtual from QAbstractSocket
bool QTcpSocketDecorator::isSequential() const
{
    return _decorated->isSequential();
}

//virtual from QAbstractSocket
bool QTcpSocketDecorator::waitForBytesWritten(int msecs)
{
    return _decorated->waitForBytesWritten(msecs);
}

//virtual from QAbstractSocket
bool QTcpSocketDecorator::waitForReadyRead(int msecs)
{
    return _decorated->waitForReadyRead(msecs);
}

//protected
//pure-virtual from QIODevice
qint64 QTcpSocketDecorator::readData(char *data, qint64 maxlen)
{
    return _decorated->read(data,maxlen);
}

//protected
//pure-virtual from QIODevice
qint64 QTcpSocketDecorator::readLineData(char *data, qint64 maxlen)
{
    return _decorated->readLine(data,maxlen);
}

//protected
//pure-virtual from QIODevice
qint64 QTcpSocketDecorator::writeData(const char *data, qint64 len)
{
    return _decorated->write(data,len);
}

//protected
QPointer<QTcpSocket> QTcpSocketDecorator::decorated() const
{
    return _decorated;
}

//private
void QTcpSocketDecorator::connectSignalsAndSlots()
{
    if (_decorated.isNull())
    {
        qWarning() << this << "has null decorated. Can't connect signals and slots";
        return;
    }

    QTcpSocket * decorated = _decorated.data();
    connect(decorated,
            SIGNAL(connected()),
            this,
            SLOT(handleDecoratedConnected()));
    connect(decorated,
            SIGNAL(disconnected()),
            this,
            SLOT(handleDecoratedDisconnected()));
    connect(decorated,
            SIGNAL(error(QAbstractSocket::SocketError)),
            this,
            SLOT(handleDecoratedError(QAbstractSocket::SocketError)));
    connect(decorated,
            SIGNAL(hostFound()),
            this,
            SLOT(handleDecoratedHostFound()));
    connect(decorated,
            SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),
            this,
            SLOT(handleDecoratedProxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));
    connect(decorated,
            SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this,
            SLOT(handleDecoratedStateChanged(QAbstractSocket::SocketState)));

    connect(decorated,
            SIGNAL(aboutToClose()),
            this,
            SLOT(handleDecoratedAboutToClose()));
    connect(decorated,
            SIGNAL(bytesWritten(qint64)),
            this,
            SLOT(handleDecoratedBytesWritten(qint64)));
    connect(decorated,
            SIGNAL(readChannelFinished()),
            this,
            SLOT(handleDecoreatedReadChannelFinished()));
    connect(decorated,
            SIGNAL(readyRead()),
            this,
            SLOT(handleDecoratedReadyRead()));

    connect(decorated,
            SIGNAL(destroyed()),
            this,
            SLOT(handleDecoratedDestroyed()));
    connect(decorated,
            SIGNAL(destroyed(QObject*)),
            this,
            SLOT(handleDecoratedDestroyed(QObject*)));

}

//private
void QTcpSocketDecorator::updateAddressesFromDecorated()
{
    this->setLocalAddress(_decorated->localAddress());
    this->setLocalPort(_decorated->localPort());
    this->setPeerAddress(_decorated->peerAddress());
    this->setPeerName(_decorated->peerName());
    this->setPeerPort(_decorated->peerPort());
}

//protected slot
//signal from QAbstractSocket
void QTcpSocketDecorator::handleDecoratedConnected()
{
    this->connected();
}

//protected slot
//signal from QAbstractSocket
void QTcpSocketDecorator::handleDecoratedDisconnected()
{
    this->disconnected();
}

//protected slot
//signal from QAbstractSocket
void QTcpSocketDecorator::handleDecoratedError(QAbstractSocket::SocketError error)
{
    this->setSocketError(error);
    this->error(error);
}

//protected slot
//signal from QAbstractSocket
void QTcpSocketDecorator::handleDecoratedHostFound()
{
    this->hostFound();
}

//protected slot
//signal from QAbstractSocket
void QTcpSocketDecorator::handleDecoratedProxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator)
{
    this->proxyAuthenticationRequired(proxy,authenticator);
}

//protected slot
//signal from QAbstractSocket
void QTcpSocketDecorator::handleDecoratedStateChanged(QAbstractSocket::SocketState state)
{
    this->updateAddressesFromDecorated();
    this->setOpenMode(_decorated->openMode());
    this->setSocketState(state);
    this->stateChanged(state);
}

//protected slot
//signal from QIODevice
void QTcpSocketDecorator::handleDecoratedAboutToClose()
{
    this->aboutToClose();
}

//protected slot
//signal from QIODevice
void QTcpSocketDecorator::handleDecoratedBytesWritten(qint64 bytes)
{
    this->bytesWritten(bytes);
}

//protected slot
//signal from QIODevice
void QTcpSocketDecorator::handleDecoreatedReadChannelFinished()
{
    this->readChannelFinished();
}

//protected slot
//signal from QIODevice
void QTcpSocketDecorator::handleDecoratedReadyRead()
{
    this->readyRead();
}

//protected slot
//signal from QObject
void QTcpSocketDecorator::handleDecoratedDestroyed(QObject *obj)
{
    Q_UNUSED(obj)
    qWarning() << "Warning:" << this << "had decorated object deleted out from underneath.";
}

void QTcpSocketDecorator::connectToHostImplementation(const QString &hostName, quint16 port, QIODevice::OpenMode mode)
{
    _decorated->connectToHost(hostName,port,mode);
}

void QTcpSocketDecorator::disconnectFromHostImplementation()
{
    _decorated->disconnectFromHost();
}
