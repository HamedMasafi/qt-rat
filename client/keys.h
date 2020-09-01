#ifndef KEYS_H
#define KEYS_H

#include <QObject>
#include <QTextStream>
#include "../config.h"

#ifdef RAT_OS_LINUX
#   include <X11/X.h>
#endif

class Keys
{
public:
    Keys();

#ifdef RAT_OS_LINUX
    static Qt::Key xkToKey(KeySym xk);
    static KeySym keyToXk(Qt::Key key);
#endif
#ifdef RAT_OS_WINDOWS
    static Qt::Key vkToKey(int xk);
    static int keyToVk(Qt::Key key);
#endif

    static QByteArray toString(const Qt::Key &key);
};

#endif // KEYS_H
