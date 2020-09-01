#ifndef ABSTRACTBROWSER_H
#define ABSTRACTBROWSER_H

#include <QObject>
#include <QVariantList>
#include <QStringList>

class AbstractBrowser : public QObject
{
    Q_OBJECT

public:
    AbstractBrowser();

    virtual QString browserPath() = 0;
    virtual QStringList tabs() = 0;
    virtual QVariantList passwords() = 0;

    static bool copyFile(QString &file, bool rawCopy = false);
protected:
    void addPasswordToList(QVariantList &list, const QString &browser, const QString &url, const QString &username, const QString &password);
};

#endif // ABSTRACTBROWSER_H
