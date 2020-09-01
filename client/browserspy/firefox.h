#ifndef FIREFOX_H
#define FIREFOX_H

#include "abstractbrowser.h"

class QLibrary;
class Firefox : public AbstractBrowser
{
    QLibrary *libnss;
//    QLibrary *libplc;

public:
    Firefox();

    QString browserPath() override;
    QVariantList passwords() override;
    QStringList tabs() override;

private:
    void TerminateFirefoxLibrary();
    int CheckMasterPassword(char *password);
    bool InitializeNSSLibrary(char *profilePath, char *password = NULL);
    bool LoadFirefoxLibraries(QString firefoxPath);
//    char *get_user_profile_path();
    int Base64Decode(char *encodedData, char **decodedData, int *decodedLen);
    int PK11Decrypt(char *encryptedData, int encryptedLen, char **clearData, int *finalLen);
    int DecryptSecretString(char *encodedData, char **clearData);
    QByteArray parseJsonlz4RecoveryFilePath(const QString &recoveryFilePath);
    QString getLibPath();
};

#endif // FIREFOX_H
