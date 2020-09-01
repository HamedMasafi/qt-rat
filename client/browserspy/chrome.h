#ifndef CHROME_H
#define CHROME_H

#include "abstractbrowser.h"

#include <QCryptographicHash>

class Chrome : public AbstractBrowser
{
public:
    Chrome();

    QString browserPath() override;
    QStringList tabs() override;
    QVariantList passwords() override;
private:
    QByteArray getMasterPassword();
    QByteArray deriveKeyPbkdf2(QCryptographicHash::Algorithm algorithm, const QByteArray &data, const QByteArray &salt, int iterations, quint64 dkLen);
    QString sendDbus(QStringList params);
};

#endif // CHROME_H
