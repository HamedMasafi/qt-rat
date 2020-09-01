#ifndef BROWSERHELPER_H
#define BROWSERHELPER_H

#include <QVariantList>



class BrowserHelper
{
public:
//    enum Browser {
//        IE = 1,
//        Firefox = 2,
//        Chrome = 4
//    };

    BrowserHelper();

    static QVariantList passwords();
    static QStringList tabs();
};

#endif // BROWSERHELPER_H
