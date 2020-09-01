#ifndef PAIRSOCKET_H
#define PAIRSOCKET_H

#include <QPointer>
#include <QTcpSocket>

class PairSocket : public QObject
{
    Q_OBJECT
    QPointer<QTcpSocket> _socket1;
    QPointer<QTcpSocket> _socket2;

    QByteArray buffer;

public:
    PairSocket(QTcpSocket *first, QObject *parent = nullptr);
    void setSecond(QTcpSocket *second);

private slots:
    void socket1readyRead();
    void socket2readyRead();
};

#endif // PAIRSOCKET_H
