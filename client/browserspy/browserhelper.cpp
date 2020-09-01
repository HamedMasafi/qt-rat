#include "browserhelper.h"

#include "ie.h"
#include "firefox.h"
#include "chrome.h"

BrowserHelper::BrowserHelper()
{

}

QVariantList BrowserHelper::passwords()
{
    QVariantList list;
    IE ie;
    Chrome chrome;
    Firefox firefox;
//    list.append(ie.passwords());
//    list.append(chrome.passwords());
    list.append(firefox.passwords());
    return list;
}

QStringList BrowserHelper::tabs()
{
    QStringList tabs;
    IE ie;
    Chrome chrome;
    Firefox firefox;
    tabs.append(ie.tabs());
    tabs.append(chrome.tabs());
    tabs.append(firefox.tabs());
    return tabs;
}
