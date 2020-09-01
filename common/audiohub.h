#ifndef AUDIOHUB_H
#define AUDIOHUB_H

#include <QAudioFormat>
#include <QObject>

class QAudioInput;
class QUdpSocket;
class QTcpSocket;
class QTcpServer;
class AudioHub : public QObject
{
    Q_OBJECT
    quint16 _bufferSize;
    QAudioInput *audio;
    QTcpSocket *socket;

    Q_PROPERTY(QString device READ device WRITE setDevice NOTIFY deviceChanged)

    QString m_device;

    QTcpServer *server;

    bool _isRunning;

public:
    enum Type {
        Tcp,
        Udp
    };
    Type _type;

    explicit AudioHub(Type type, QObject *parent = nullptr);

    static QStringList devices();
    void startServer(quint16 port);
    void startClient(const QString &deviceName, const QString &serverAddress, quint16 port);
    void stopServer();

    quint16 bufferSize() const;
    void setBufferSize(const quint16 &bufferSize);

    QString device() const;

    bool isRunning() const;

public slots:
    void setDevice(QString device);

signals:

    void deviceChanged(QString device);
private slots:
    QAudioFormat format() const;
};

#endif // AUDIOHUB_H
