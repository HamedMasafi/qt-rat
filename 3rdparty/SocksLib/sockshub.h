#ifndef SOCKSHUB_H
#define SOCKSHUB_H

#include <QObject>

#include "SocksConnection.h"
#include "SocksLib_global.h"

class SocksConnection;
class SocksHub : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString serverAddress READ serverAddress WRITE setServerAddress NOTIFY serverAddressChanged)
    Q_PROPERTY(quint16 port READ port WRITE setPort NOTIFY portChanged)

    QTcpSocket *_controllerSocket;
public:
    explicit SocksHub(QObject *parent = nullptr);

    void makeNewConnection();
    QString serverAddress() const;

    quint16 port() const;

public slots:
    void setServerAddress(QString serverAddress);

    void setPort(quint16 port);
    bool start();

signals:

    void serverAddressChanged(QString serverAddress);

    void portChanged(quint16 port);

private slots:
    void handleConnectionDestroyed();

    void controllerSocket_readyRead();
private:
    QList<QPointer<SocksConnection> > _connections;

    QString m_serverAddress;
    quint16 m_port;
};

#endif // SOCKSHUB_H
