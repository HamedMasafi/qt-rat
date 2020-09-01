#include "audiohub.h"

#include <QAudioInput>
#include <QAudioOutput>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QDebug>
#include <QAudioRecorder>

bool AudioHub::isRunning() const
{
    return _isRunning;
}

AudioHub::AudioHub(Type type, QObject *parent) : QObject(parent), _bufferSize(4000),
                                                 _type(type), _isRunning(false)
{
/*
QAudioFormat format;
format.setChannelCount(1);
format.setSampleRate(8000);
format.setSampleSize(8);
format.setCodec("audio/pcm");
format.setByteOrder(QAudioFormat::LittleEndian);
format.setSampleType(QAudioFormat::UnSignedInt);
*/
}

QStringList AudioHub::devices()
{
    auto list = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    QStringList ret;
    foreach (auto device, list)
        ret.append(device.deviceName());
    return ret;
}

quint16 AudioHub::bufferSize() const
{
    return _bufferSize;
}

void AudioHub::setBufferSize(const quint16 &bufferSize)
{
    _bufferSize = bufferSize;
}

QString AudioHub::device() const
{
    return m_device;
}

void AudioHub::setDevice(QString device)
{
    if (m_device == device)
        return;

    m_device = device;
    emit deviceChanged(m_device);
}

QAudioFormat AudioHub::format() const
{
    QAudioFormat format;
    format.setChannelCount(1);
    format.setSampleRate(8000);
    format.setSampleSize(8);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);
    return format;
}
void AudioHub::startServer(quint16 port)
{
    if (_type == Tcp) {
        server = new QTcpServer(this);
        connect(server, &QTcpServer::newConnection, [this](){
            qDebug() << "new connection";

            auto audio = new QAudioOutput(format(), this);
            audio->setBufferSize(_bufferSize);

            auto socket = server->nextPendingConnection();
            audio->start(socket);
        });
        server->listen(QHostAddress::Any, port);
    } else {
        auto socket = new QUdpSocket(this);
        socket->bind(QHostAddress::LocalHost, port);
        socket->waitForReadyRead();

        auto audio = new QAudioOutput(format(), this);
        audio->setBufferSize(_bufferSize);
        audio->start(socket);
    }
}

void AudioHub::startClient(const QString &deviceName, const QString &serverAddress, quint16 port)
{
    auto list = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    audio = nullptr;
    foreach (auto device, list)
        if (device.deviceName() == deviceName)
            audio = new QAudioInput(device, format(), this);

    if (!audio)
        return;

    audio->setBufferSize(_bufferSize);

    if (_type == Tcp) {
        socket = new QTcpSocket(this);

        socket->connectToHost(serverAddress, port);
        socket->waitForConnected();

        audio->start(socket);
    } else {
        auto s = new QUdpSocket(this);
        s->connectToHost(serverAddress, port);
        s->waitForConnected();

        audio->start(socket);
    }

    _isRunning = true;
}

void AudioHub::stopServer()
{
    if (_type == Tcp)
        server->close();
}
