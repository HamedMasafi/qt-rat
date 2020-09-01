#include "pairsocket.h"
#include <QDebug>
#include <QPointer>
#include <QThread>

PairSocket::PairSocket(QTcpSocket *first, QObject *parent) : QObject(parent), _socket1(first), _socket2(nullptr)
{
    first->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    connect(first, &QTcpSocket::readyRead, this, &PairSocket::socket1readyRead);
}

void PairSocket::setSecond(QTcpSocket *second)
{
    _socket2 = second;
    second->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    connect(second, &QTcpSocket::readyRead, this, &PairSocket::socket2readyRead);

    if (buffer.size()) {
        second->write(buffer);
        second->flush();
        buffer.clear();
    }
}

void PairSocket::socket1readyRead()
{
    if (!_socket2) {
        qDebug() << "Data for socket 1; but there is not socket 2";
        buffer.append(_socket1->readAll());
        return;
    }

    auto buffer = _socket1->readAll();
    qDebug() << "Data read from socket 1" << buffer.toHex(' ');
    auto l = _socket2->write(buffer);
    _socket2->flush();
    if (!l)
        qDebug() << "Unable to write to socket 2";
}

void PairSocket::socket2readyRead()
{
    if (!_socket1) {
        qDebug() << "Data for socket 2; but there is not socket 1";
        return;
    }

    auto buffer = _socket2->readAll();
    auto l = _socket1->write(buffer);
    qDebug() << "Data read from socket 2" << buffer.toHex(' ');
    _socket1->flush();
    if (!l)
        qDebug() << "Unable to write to socket 1";
}

