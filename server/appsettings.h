#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QSettings>

#define PROP_DECL(type, name, write) \
    private: \
    type _##name; \
    public: \
    type name() const; \
    void write(type name);

#define PROP_IMPL(type, name, write, def) \
    type AppSettings::name() const \
    { \
        return this->value(#name, def).value<type>(); \
    } \
    void AppSettings::write(type name) \
    { \
        this->setValue(#name, name); \
    }

#define RatAppSettings AppSettings::instance()

class AppSettings : public QSettings
{
    AppSettings();
public:

    static AppSettings *instance();

    PROP_DECL(int, ratPort, setRatPort)
    PROP_DECL(int, broadcastPort, setBroadcastPort)
    PROP_DECL(int, installerPort, setInstallerPort)
    PROP_DECL(int, remoteDesktopPort, setRemoteDesktopPort)
    PROP_DECL(int, localProxyPort, setLocalProxyPort)
    PROP_DECL(int, clientConnectionProxyPort, setClientConnectionProxy)
    PROP_DECL(int, micSpyPort, setMicSpyPort)

    PROP_DECL(bool, enableBroadcasting, setEnableBroadcasting)

    static bool supportsInstaller();
};

#endif // APPSETTINGS_H
