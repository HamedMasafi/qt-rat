#include "chrome.h"

#include <QStandardPaths>
#include <QDebug>
#include <QFile>
#include <QCryptographicHash>
#include <QtEndian>
#include <QMessageAuthenticationCode>
#include <QProcess>
#include <QRegularExpression>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include "../../3rdparty/Qt-AES/qaesencryption.h"
#include <limits>

#ifdef Q_OS_LINUX
#   define USE_AES
#endif
#ifdef Q_OS_WIN
#   include <windows.h>
#endif

#ifdef Q_OS_WIN
#   include <Windows.h>
#   pragma comment(lib, "crypt32.lib")
#   pragma comment (lib, "advapi32")
#endif

struct FileHeader {
    qint32 signature;
    qint32 version;
};

Chrome::Chrome()
{

}

QString Chrome::browserPath()
{
    auto locs = QStandardPaths::standardLocations(QStandardPaths::GenericConfigLocation);
    if (!locs.count()) {
        qDebug() << "Unable to find cache dir";
        return QString();
    }

#ifdef Q_OS_WIN
    auto path = locs.first() + "/Google/Chrome/User Data/Default/";
#else
    auto path = locs.first() + "/google-chrome/Default/";
#endif
    return path;
}

QStringList Chrome::tabs()
{
    auto path = browserPath() + "Current Session";

    if (!QFile::exists(path)) {
        qDebug() << "the file not found:" << path;
        return QStringList();
    }

    if (!copyFile(path))
        return QStringList();

    QFile f(path);
    if (f.open(QIODevice::ReadOnly)) {
        FileHeader header;
        f.read(reinterpret_cast<char*>(&header),
               sizeof(header));
        qDebug() << header.signature << header.version << (header.signature == 0x53534E53);

        QMap<int, QString> urls;
        while (!f.atEnd()) {
            quint16 size;
            quint8 id;

            f.read(reinterpret_cast<char *>(&size), 2);
            f.read(reinterpret_cast<char *>(&id), 1);

            if (!size) {
                qDebug() << "Corrupted File!";
                return QStringList();
            }
            QByteArray data;
            if (id == 6) {
                int payload_size;
                int tab_id;
                int index;
                int ssize;
                f.read(reinterpret_cast<char *>(&payload_size), 4);
                f.read(reinterpret_cast<char *>(&tab_id), 4);
                f.read(reinterpret_cast<char *>(&index), 4);
                f.read(reinterpret_cast<char *>(&ssize), 4);
                auto url = QString(f.read(ssize));
                data = f.read(size - 17 - ssize);
                urls.insert(tab_id, url);
                //                qDebug() << tab_id << index<< "url=" << url;
            } else {
                data = f.read(size - 1);
            }
            //            qDebug() << "#" <<id << "   size=" <<size;
            //            qDebug() << data;
        }


        f.close();
        return urls.values();
    } else {
        qDebug() << "unable to open file";
    }
    return QStringList();
}

QVariantList Chrome::passwords()
{
    auto dbFile = browserPath() + "/Login Data";
    if (!QFile::exists(dbFile)) {
        qDebug() << "The file" << dbFile << "does not exists";
        return QVariantList();
    }


    if (!copyFile(dbFile)) {
        qDebug() << "Unable to copy file" << dbFile;
        return QVariantList();
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbFile);
    if (!db.open()) {
        qDebug() << "Unable to open sqlite database" << dbFile << "does not exists";
        return QVariantList();
    }

    QVariantList passwords;
    auto query = db.exec("SELECT * FROM logins");


#ifdef USE_AES
    QByteArray key;
    key = deriveKeyPbkdf2(QCryptographicHash::Sha1,
                              getMasterPassword(), //"inZC98gCLnE5pXDdS6wQ9Q==",
                              "saltysalt", 1, 16);
    QAESEncryption aes(QAESEncryption::AES_128,
                       QAESEncryption::CBC, QAESEncryption::PKCS7);
    QByteArray iv("                ");
#endif

    while (query.next()) {
        QString origin_url = query.value("origin_url").toString();
        QString username_value = query.value("username_value").toString();
        QByteArray password_value = query.value("password_value").toByteArray();

#ifdef Q_OS_WIN
        DATA_BLOB dec_data;
        DATA_BLOB enc_data;
        enc_data.cbData = password_value.size();
        enc_data.pbData = (BYTE*)password_value.data();
        if(CryptUnprotectData(&enc_data, NULL, NULL, NULL, NULL, 0, &dec_data))
        {
            password_value = QByteArray((const char *)dec_data.pbData, dec_data.cbData);
        } else
        {
            password_value = "";
        }
#endif
#ifdef USE_AES
        password_value = aes.removePadding(aes.decode(password_value.remove(0, 3), key, iv));
#endif
        addPasswordToList(passwords, "chrome",
                          origin_url, username_value,
                          QString(password_value));
    }
    return passwords;
}

#ifdef Q_OS_LINUX
QByteArray Chrome::getMasterPassword()
{
    auto walletsOut = sendDbus(QStringList() << "--print-reply=literal" << "--dest=org.kde.kwalletd" << "/modules/kwalletd" << "org.kde.KWallet.wallets");
    QRegularExpression reWallets("array\\s*\\[\\s*(\\S+)\\s*]");
    auto m = reWallets.match(walletsOut.replace("\n", ""));

    if (!m.hasMatch())
        return QByteArray();
    qDebug() << "wallet name=" << m.captured(1);
    auto appIdOut = sendDbus(QStringList() << "--print-reply=literal" << "--dest=org.kde.kwalletd"
                                           << "/modules/kwalletd" << "org.kde.KWallet.open"
                                           << "string:" + m.captured(1)  << "int64:0" << "string:''");
    QRegularExpression reAppId("int32\\s(\\d+)");
    m = reAppId.match(appIdOut);

    if (!m.hasMatch())
        return QByteArray();
    bool ok;
    int appId = m.captured(1).toInt(&ok);
    if (!ok)
        return QByteArray();
    qDebug() << "appId=" << appId;
    auto keyOut = sendDbus(QStringList() << "--print-reply=literal" << "--dest=org.kde.kwalletd"
                                         << "/modules/kwalletd" << "org.kde.KWallet.readPassword"
                                         << "int32:" + QString::number(appId)
                                         << "string:Chrome Keys" << "string:Chrome Safe Storage"
                                         << "string:");

    return keyOut.trimmed().toUtf8();
    /*
 dbus-send --print-reply=literal --dest=org.kde.kwalletd /modules/kwalletd org.kde.KWallet.open string:'hamed' int64:0 string:''
 int32 253115632

    */

    /*
dbus-send --print-reply=literal --dest=org.kde.kwalletd /modules/kwalletd org.kde.KWallet.wallets
array [
    hamed   ]
*/

    /*
    dbus-send --print-reply=literal --dest=org.kde.kwalletd /modules/kwalletd org.kde.KWallet.readEntry int32:253115632 string:'Chrome Keys' string:'Chrome Safe Storage' string:''
array of bytes [
      00 00 00 30 00 69 00 6e 00 5a 00 43 00 39 00 38 00 67 00 43 00 4c 00 6e
      00 45 00 35 00 70 00 58 00 44 00 64 00 53 00 36 00 77 00 51 00 39 00 51
      00 3d 00 3d
   ]
*/

}

QByteArray Chrome::deriveKeyPbkdf2(QCryptographicHash::Algorithm algorithm,
                                            const QByteArray &data, const QByteArray &salt,
                                            int iterations, quint64 dkLen)
{
    // https://tools.ietf.org/html/rfc8018#section-5.2
    // The RFC recommends checking that 'dkLen' is not greater than '(2^32 - 1) * hLen'
    int hashLen = QCryptographicHash::hashLength(algorithm);
    const quint64 maxLen = quint64(std::numeric_limits<quint32>::max() - 1) * hashLen;
    if (dkLen > maxLen) {
        qWarning().nospace() << "Derived key too long:\n"
                             << algorithm << " was chosen which produces output of length "
                             << maxLen << " but " << dkLen << " was requested.";
        return QByteArray();
    }
    if (iterations < 1 || dkLen < 1)
        return QByteArray();
    QByteArray key;
    quint32 currentIteration = 1;
    QMessageAuthenticationCode hmac(algorithm, data);
    QByteArray index(4, Qt::Uninitialized);
    while (quint64(key.length()) < dkLen) {
        hmac.addData(salt);
        qToBigEndian(currentIteration, index.data());
        hmac.addData(index);
        QByteArray u = hmac.result();
        hmac.reset();
        QByteArray tkey = u;
        for (int iter = 1; iter < iterations; iter++) {
            hmac.addData(u);
            u = hmac.result();
            hmac.reset();
            std::transform(tkey.cbegin(), tkey.cend(), u.cbegin(), tkey.begin(),
                           std::bit_xor<char>());
        }
        key += tkey;
        currentIteration++;
    }
    return key.left(dkLen);
}

QString Chrome::sendDbus(QStringList params)
{
    QProcess p;
    p.setProgram("dbus-send");
    p.setArguments(params);
    p.start();
    p.waitForFinished();
    auto b = p.readAllStandardOutput();
    if (p.exitStatus() == QProcess::CrashExit)
        qDebug() << "Error in proc" << params.join(" ");
    return b;
}
#endif
