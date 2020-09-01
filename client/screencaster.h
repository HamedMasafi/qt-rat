#ifndef SCREENCASTER_H
#define SCREENCASTER_H

#include <QImage>
#include <QThread>

class QUdpSocket;
class QScreen;
class ScreenCaster : public QThread
{
    Q_OBJECT
    QScreen *_screen;
    QString _serverAddress;
    quint16 _port;
    QImage _lastFrame, _frame;
    QUdpSocket *_socket;
    QImage::Format _format;

    int _width;
    int _heigth;

    bool _running;

    void sendImage(const QImage &image, int x, int y);

public:
    explicit ScreenCaster(QObject *parent = nullptr);

    int width() const;
    void setWidth(int width);

    int heigth() const;
    void setHeigth(int heigth);

    QString serverAddress() const;
    void setServerAddress(const QString &serverAddress);

    quint16 port() const;
    void setPort(quint16 port);

    void stop();
    QImage::Format format() const;
    void setFormat(const QImage::Format &format);

signals:

public slots:

    // QThread interface
protected:
    void run();
private slots:
    void sendChunk(const char &command, int x, int y, const QByteArray &buffer = QByteArray());
};

#endif // SCREENCASTER_H
