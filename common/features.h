#ifndef FEATURES_H
#define FEATURES_H

#define FOR_EACH_FEATURES(x) \
    x(FileManager) \
    x(FileTransfer) \
    x(MessageBox) \
    x(OpenUrl) \
    x(Screenshot) \
    x(StartupManager) \
    x(TaskManager) \
    x(Webcam) \
    x(SystemInformations) \
    x(RemoteDesktop) \
    x(SocksProxy) \
    x(LockScreen) \
    x(KeyLogger) \
    x(BrowserTabs) \
    x(Power) \
    x(CommandLine) \
    x(Registry) \
    x(DownloadAndExec) \
    x(PassworgGraber)

#include <QString>
#include <QStringList>

class Features
{
public:
#define x(n) static QString n;
    FOR_EACH_FEATURES(x)
#undef x

    Features();

    static QStringList currentPlatformFeatures();

    static QStringList windowsFeatures();
    static QStringList linuxFeatures();
    static QStringList androidFeatures();
    static QStringList macosFeatures();
private:
    static QStringList windowsLinuxCommonFeatures();
};

#endif // FEATURES_H
