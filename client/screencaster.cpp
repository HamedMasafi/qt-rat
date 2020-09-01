#include "screencaster.h"

#include <QPixmap>
#include <QGuiApplication>
#include <QScreen>
#include <QUdpSocket>
#include <QBuffer>
#include <QDebug>

int ScreenCaster::width() const
{
    return _width;
}

void ScreenCaster::setWidth(int width)
{
    _width = width;
}

int ScreenCaster::heigth() const
{
    return _heigth;
}

void ScreenCaster::setHeigth(int heigth)
{
    _heigth = heigth;
}

QString ScreenCaster::serverAddress() const
{
    return _serverAddress;
}

void ScreenCaster::setServerAddress(const QString &serverAddress)
{
    _serverAddress = serverAddress;
}

quint16 ScreenCaster::port() const
{
    return _port;
}

void ScreenCaster::setPort(quint16 port)
{
    _port = port;
}

void ScreenCaster::stop()
{
    _running = false;
}

QImage::Format ScreenCaster::format() const
{
    return _format;
}

void ScreenCaster::setFormat(const QImage::Format &format)
{
    _format = format;
}

void ScreenCaster::sendChunk(const char &command, int x, int y, const QByteArray &buffer)
{
    _socket->write(qCompress(command
                                 + QByteArray::number(x)
                                 + "/"
                                 + QByteArray::number(y) + " "
                                 + buffer
                                 + "***", 9));
}

void ScreenCaster::sendImage(const QImage &image, int x, int y)
{
    for (int i = 0; i < image.height(); ++i) {
        auto tmp = image.copy(0, i, image.width(), 1);
        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        tmp.save(&buffer, "JPG");

        sendChunk('D', x, y + i, ba);

        if (_socket->error() != QUdpSocket::UnknownSocketError)
            qDebug() << "error sending udp" << _socket->errorString();
    }

    sendChunk('S', 0, 0);
}

ScreenCaster::ScreenCaster(QObject *parent) : QThread(parent)
{
    _screen = QGuiApplication::primaryScreen();

}

void ScreenCaster::run()
{
    _socket = new QUdpSocket;
    _socket->connectToHost(_serverAddress, _port);
    int isFirst = true;

    _running = true;
    while (_running) {
        QThread::msleep(500);
        QImage f;

        if (_width && _heigth)
            f = _screen->grabWindow(0).toImage().scaled(_width, _heigth);
        else
            f = _screen->grabWindow(0).toImage();

        if (isFirst) {
            sendChunk('I', f.width(), f.height());
            isFirst = false;
        }
        if (_socket->state() != QUdpSocket::ConnectedState)
            break;

        int rc_x = f.width();
        int rc_y = f.height();
        int rc_r = 0;
        int rc_b = 0;
        if (_lastFrame.isNull()) {
            rc_x = rc_y = 0;
            rc_r = f.width();
            rc_b = f.height();
        } else {
            for (int x = 0; x < f.width(); ++x) {
                for (int y = 0; y < f.height(); ++y) {
                    if (_lastFrame.pixel(x, y) != f.pixel(x, y)) {
                        if (rc_x > x) rc_x = x;
                        if (rc_y > y) rc_y = y;
                        if (rc_r < x) rc_r = x;
                        if (rc_b < y) rc_b = y;
                    }
                }
            }
        }
        _lastFrame = f;

        QRect rc(rc_x, rc_y, rc_r - rc_x, rc_b - rc_y);
        _frame = f.copy(rc);
#if QT_VERSION > QT_VERSION_CHECK(5, 13, 0)
        _frame.convertTo(_format);
#else
        _frame = _frame.convertToFormat(_format);
#endif
        sendImage(f, 0, 0);
    }

    deleteLater();
}
