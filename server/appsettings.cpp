#include <QApplication>
#include "appsettings.h"
#include "../common/consts.h"

AppSettings::AppSettings() : QSettings(qApp->applicationDirPath() + "/settings.ini", QSettings::IniFormat)
{

}

AppSettings *AppSettings::instance()
{
    static AppSettings *in = nullptr;
    if (!in)
        in = new AppSettings;
    return in;
}

bool AppSettings::supportsInstaller()
{
#ifdef WEB_SERVER_POR
    return true;
#else
    return false;
#endif
}

PROP_IMPL(int, ratPort, setRatPort, NEURON_PORT)
PROP_IMPL(int, broadcastPort, setBroadcastPort, BROADCAST_PORT)
PROP_IMPL(int, remoteDesktopPort, setRemoteDesktopPort, REMOTE_DESKTOP_PORT)
PROP_IMPL(int, micSpyPort, setMicSpyPort, NEURON_PORT)
PROP_IMPL(int, localProxyPort, setLocalProxyPort, LOCAL_PROXY_PORT)
PROP_IMPL(int, clientConnectionProxyPort, setClientConnectionProxy, CLIENT_PROXY_CONNECTION_PORT)
PROP_IMPL(bool, enableBroadcasting, setEnableBroadcasting, true)

#ifdef WEB_SERVER_POR
    PROP_IMPL(int, installerPort, setInstallerPort, WEB_SERVER_POR)
#else
    PROP_IMPL(int, installerPort, setInstallerPort, 0)
#endif
