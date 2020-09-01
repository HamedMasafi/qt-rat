#include "firefox.h"

#ifdef USE_LZ4
#   include "lz4.h"
#endif

#include <QByteArray>
#include <QFile>
#include <QDebug>
#include <QDir>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QLibrary>
#include <QSysInfo>

#ifdef Q_OS_WIN
#include <QSettings>
#include <Windows.h>
//HMODULE libnss = NULL;
//HMODULE libplc = NULL;
#endif

typedef struct PK11SlotInfoStr PK11SlotInfo;
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

// NSS Library functions
typedef SECStatus      (*NSS_Init) (const char *configdir);
typedef SECStatus      (*NSS_Shutdown) (void);
typedef PK11SlotInfo * (*PK11_GetInternalKeySlot) (void);
typedef void           (*PK11_FreeSlot) (PK11SlotInfo *slot);
typedef SECStatus      (*PK11_CheckUserPassword) (PK11SlotInfo *slot,char *pw);
typedef SECStatus      (*PK11_Authenticate) (PK11SlotInfo *slot, PRBool loadCerts, void *wincx);
typedef SECStatus      (*PK11SDR_Decrypt) (SECItem *data, SECItem *result, void *cx);

// PLC Library functions
typedef char *         (*PL_Base64Decode)( const char *src, PRUint32 srclen, char *dest);

NSS_Init                NSSInit;
NSS_Shutdown            NSSShutdown;
PK11_GetInternalKeySlot PK11GetInternalKeySlot;
PK11_CheckUserPassword  PK11CheckUserPassword;
PK11_FreeSlot           PK11FreeSlot;
PK11_Authenticate       PK11Authenticate;
PK11SDR_Decrypt         PK11SDRDecrypt;
PL_Base64Decode         PLBase64Decode;

char masterPassword[1024] = {0};
int IsNSSInitialized = 0;

Firefox::Firefox()
{

}

void Firefox::TerminateFirefoxLibrary() {
    if( IsNSSInitialized  && (NSSShutdown != NULL) )
        (*NSSShutdown)();

    if(libnss->isLoaded())
        libnss->unload();

//    if(libplc)
//        libplc->unload();
}
int Firefox::CheckMasterPassword(char *password) {
    PK11SlotInfo *slot = NULL;
    int ret = 0;

    slot = (*PK11GetInternalKeySlot)();

    if (NULL == slot) {
        printf("PK11_GetInternalKeySlot failed\n");
        return 0;
    }

    // First check if the master password set
    //**********************
    //    if( (*PK11CheckUserPassword)(slot, "") == SECSuccess )
    //    {
    //        printf("Master Password is NOT set\n");
    //        (*PK11FreeSlot) (slot);
    //        return 1;
    //    }

    // Check password
    if( (*PK11CheckUserPassword)(slot, password) == SECSuccess ) {
        printf("Master password is correct\n");
        ret = 1;
    } else {
        printf("Specified master password %s is wrong", password);
        ret = 0;
    }

    (*PK11FreeSlot) (slot);

    return ret;
}
bool Firefox::InitializeNSSLibrary(char *profilePath, char *password){
    IsNSSInitialized = 0;

    auto m = NSSInit(profilePath);
    if( m != SECSuccess ) {
        printf("Initialization failed , Make sure key3.db and cert8.db "\
               "files are present in the specified directory\n");
        TerminateFirefoxLibrary();
//        auto m = (*NSSInit) (profilePath);
        qDebug() << "Init failed" << profilePath << m << "*****";
        return false;
    }

    IsNSSInitialized = 1;

    // Check if master password is correct
    if( password != NULL){
        strcpy(masterPassword, password);
    }

    if( CheckMasterPassword( masterPassword ) != 1) {
        TerminateFirefoxLibrary();
        qDebug() << "Unable to check master password";
        return false;
    }

    printf("NSS library initiliazed successfully\n");

    return true;
}

QString Firefox::getLibPath()
{
#ifdef Q_OS_WIN
    QSettings reg("HKEY_CLASSES_ROOT\\Applications\\firefox.exe\\shell\\open\\command", QSettings::NativeFormat);
    auto p = reg.value("Default").toString();
    if (p.startsWith("\""))
        p = p.remove(0, 1);
    p = p.mid(0, p.indexOf("\""));
    p = p.mid(0, p.lastIndexOf("\\"));
    qDebug() << "p=" <<p;
    return p;
#endif
#ifdef Q_OS_LINUX
    if (QSysInfo::buildCpuArchitecture() == "arm64" || QSysInfo::buildCpuArchitecture() == "x86_64")
        return "/usr/lib64";
    else
        return "/usr/lib";
#endif
}
bool Firefox::LoadFirefoxLibraries(QString firefoxPath) {

    QFileInfo fi(firefoxPath);
    QStringList libs = QStringList()
            << /*"mozglue.dll" << "nspr4.dll" << "plc4.dll" << "plds4.dll"
                                  << "nssutil3.dll" << "mozsqlite3.dll" << "softokn3.dll" <<*/ "nss3.dll";

    // Load the libraries from firefox path.
    if (firefoxPath == QString()) {
        return false;
    }


    /*foreach(QString lib, libs) {
        auto libPath = QDir::toNativeSeparators(QString(fi.absolutePath() + "\\" + lib));
        qDebug() << "Loading" << libPath;
        if (QFile::exists(libPath)) {
            if(NULL == LoadLibrary2(fi.absolutePath().toLocal8Bit().data(), lib.toLocal8Bit().data())){
                return 1;
            }
        } else {
            qDebug() << "Not exists" << libPath;
        }
    }*/
    QDir::setCurrent(firefoxPath);

    libnss = new QLibrary("nss3");
//    libplc = new QLibrary("libplc4");

    if (!libnss->load() /*|| !libplc->load()*/) {
        qDebug() << "Error loading libs" << QDir::toNativeSeparators(QString(firefoxPath + "/nss3.dll"));
        return false;
    }
    qDebug() << "Firefox libraries loaded successfully";

    // Load required functions
    NSSInit = (NSS_Init) libnss->resolve("NSS_Init");
    NSSShutdown = (NSS_Shutdown) libnss->resolve("NSS_Shutdown");
    PK11GetInternalKeySlot = (PK11_GetInternalKeySlot) libnss->resolve("PK11_GetInternalKeySlot");
    PK11FreeSlot = (PK11_FreeSlot) libnss->resolve("PK11_FreeSlot");
    PK11Authenticate = (PK11_Authenticate) libnss->resolve("PK11_Authenticate");
    PK11SDRDecrypt = (PK11SDR_Decrypt) libnss->resolve("PK11SDR_Decrypt");
    PK11CheckUserPassword = (PK11_CheckUserPassword) libnss->resolve("PK11_CheckUserPassword");
    PLBase64Decode     = ( PL_Base64Decode ) libnss->resolve("PL_Base64Decode");

    if( !NSSInit || !NSSShutdown || !PK11GetInternalKeySlot || !PK11Authenticate ||
            !PK11SDRDecrypt || !PK11FreeSlot || !PK11CheckUserPassword || !PLBase64Decode )  {
        TerminateFirefoxLibrary();
        qDebug() << "Error in resolve";
        return false;
    }

    qDebug() << "Firefox functions loaded successfully";

    return true;

}

int Firefox::Base64Decode(char *encodedData, char **decodedData, int *decodedLen) {
    int len = strlen( encodedData );
    int adjust = 0;

    // Compute length adjustment
    if (encodedData[len-1] == '=') {
        adjust++;
        if (encodedData[len-2] == '=')
            adjust++;
    }

    *decodedData = ( char *)(*PLBase64Decode)(encodedData, len, NULL);

    if( *decodedData == NULL ) {
        printf("Base64 decoding failed\n");
        return 1;
    }

    *decodedLen = (len*3)/4 - adjust;

    printf("Length of decoded data: %d\n", *decodedLen);

    return 0;
}

int Firefox::PK11Decrypt(char *encryptedData, int encryptedLen, char **clearData, int *finalLen) {
    PK11SlotInfo *slot = 0;
    SECStatus status;
    SECItem request;
    SECItem reply;

    // Find token with SDR key
    slot = (*PK11GetInternalKeySlot)();

    if (!slot) {
        printf("PK11_GetInternalKeySlot failed\n");
        return 1;
    }

    if ( (*PK11Authenticate)(slot, PR_TRUE, NULL) != SECSuccess) {
        printf("PK11_Authenticate failed\n");
        return 1;
    }

    // Decrypt the string
    request.data = (unsigned char *)encryptedData;
    request.len = encryptedLen;

    reply.data = 0;
    reply.len = 0;

    status = (*PK11SDRDecrypt)(&request, &reply, NULL);

    if (status != SECSuccess) {
        printf("PK11SDR_Decrypt failed (status code: %d)\n", status);
        return 1;
    }

    *clearData = (char*)reply.data;
    *finalLen  = reply.len;

    // Free the slot
    (*PK11FreeSlot)(slot);

    return 0;
}
int Firefox::DecryptSecretString(char *encodedData, char **clearData) {
    char *decodedData = NULL;
    int decodedLen = 0;
    char *finalData = NULL;
    int finalLen = 0;

    // First do base64 decoding
    if( Base64Decode(encodedData, &decodedData, &decodedLen) || !decodedData ) {
        printf("Base64 decoding of data failed\n");
        return 1;
    }

    // Now do actual PK11 decryption
    if( PK11Decrypt(decodedData, decodedLen, &finalData, &finalLen) || !finalData ) {
        printf("Failed to decrypt the string\n");
        return 1;
    }

    // Decrypted string is not NULL terminated
    *clearData = (char*) malloc( finalLen + 1 );
    memcpy(*clearData, finalData, finalLen);
    *(*clearData + finalLen) = 0;

    return 0;
}




QByteArray Firefox::parseJsonlz4RecoveryFilePath(const QString &recoveryFilePath)
{
    const char mozlz4_magic[] = {109, 111, 122, 76, 122, 52, 48, 0};  /* "mozLz40\0" */
    const int decomp_size = 4;  /* 4 bytes size come after the header */
    const size_t magic_size = sizeof mozlz4_magic;
    char *encryptedData = nullptr;
    char *decryptedData = nullptr;

    int readSize = 0;
    size_t outputBufferSize = 0;

    QFile f(recoveryFilePath);
    f.open(QIODevice::ReadOnly);
    QByteArray encryptedByteData = f.readAll();
    f.close();
    if (!encryptedByteData.isEmpty()) {
        encryptedData = encryptedByteData.data();
        readSize = encryptedByteData.size();
    } else {
        qDebug() << "[FirefoxUtils::parseJsonlz4RecoveryFilePath] Empty file";
        return QByteArray();
    }

    if (readSize < magic_size + decomp_size || memcmp(mozlz4_magic, encryptedData, magic_size) != 0) {
        qDebug() << "[FirefoxUtils::parseJsonlz4RecoveryFilePath] Unsupported file format: " + recoveryFilePath;
        return QByteArray();
    }

    size_t i = 0;
    for (i = magic_size; i < magic_size + decomp_size; i++) {
        outputBufferSize += (unsigned char) encryptedData[i] << (8 * (i - magic_size));
    }

    if (!(decryptedData = (char *) malloc(outputBufferSize))) {
        qDebug() << "[FirefoxUtils::parseJsonlz4RecoveryFilePath] Failed to allocate a buffer for an output.";
        return QByteArray();
    }

    int decryptedDataSize = LZ4_decompress_safe(encryptedData + i, decryptedData, (int) (readSize - i), (int) outputBufferSize);
    if (decryptedDataSize < 0) {
        qDebug() << "[FirefoxUtils::parseJsonlz4RecoveryFilePath] Failed to decompress a file: " + recoveryFilePath;
        free(decryptedData);
        return QByteArray();
    }
    QByteArray qDecryptedData = QByteArray(decryptedData);
    free(decryptedData);

    int indexOfLastProperChar = qDecryptedData.lastIndexOf(QLatin1String("}"));
    QByteArray cutData = qDecryptedData.left(indexOfLastProperChar + 1);

    //    qDebug() << "[FirefoxUtils::parseJsonlz4RecoveryFilePath] Successfully decompressed a file: " + recoveryFilePath;
    //    // Debug: write to a D:/fx.json file
    //    QString filename = "/home/karol/Documents/fx.json";
    //    QFile file(filename);
    //    if (file.open(QIODevice::ReadWrite)) {
    //        QTextStream stream(&file);
    //        stream << cutData;
    //    }
    return cutData;
}

QString Firefox::browserPath()
{
    QString profilePath;


#ifdef Q_OS_WIN
    profilePath = qEnvironmentVariable("UserProfile")
            + "/Application Data/Mozilla/Firefox/";
#else
    profilePath = QDir::homePath() + "/.mozilla/firefox/";
#endif

    QFile f(profilePath + "profiles.ini");
    if (!f.open(QIODevice::ReadOnly))
        return QString();

    QByteArray line;
    QString profileName;
    bool isDefaultFound = false;
    while ((line = f.readLine()) != QByteArray()) {
        if( !isDefaultFound && line.contains("Name=default")) {
            isDefaultFound = true;
            continue;
        }

        if (isDefaultFound && line.startsWith("Path=")) {
            profileName = line.mid(5).replace("\n", "").replace("\r", "");
        }

//        if (line.startsWith("[") && isDefaultFound)
//            break;
    }

    return profilePath + profileName;
}

QVariantList Firefox::passwords()
{
//    auto pp = getFirefoxPath();

    auto path = getLibPath();

//    if (path == QString())
//        return QVariantList();

    char mk[1024] = {0};
    QVariantList passwords;
    auto ff_prof_path = browserPath();
    if (!LoadFirefoxLibraries(path)){
        qDebug() << "error load" << path;
        return QVariantList();
    }
    if(!InitializeNSSLibrary(QDir::toNativeSeparators(ff_prof_path).toLocal8Bit().data()) )  {
        qDebug() << "error in initalize library";
        return QVariantList();
    }

    QFile loginsJson(ff_prof_path + "/logins.json");
    if (!loginsJson.open(QIODevice::ReadOnly)) {
        qDebug() << "Unable to open json file"<<ff_prof_path + "/logins.json";
        return QVariantList();
    }
    //        dump_ff(ff_prof_path);
    QJsonDocument json = QJsonDocument::fromJson(loginsJson.readAll());
    loginsJson.close();

    auto loginsArray = json.object().value("logins").toArray();
    qDebug() << loginsArray;

    foreach(auto v, loginsArray) {
        auto obj = v.toObject();
        if (obj == QJsonObject())
            continue;
        auto url = obj.value("hostname").toString();
        auto u = obj.value("encryptedUsername").toString();
        auto p = obj.value("encryptedPassword").toString();
        char *clearPassword = NULL;
        char *clearUsername = NULL;
        DecryptSecretString(u.toLocal8Bit().data(), &clearUsername);
        DecryptSecretString(p.toLocal8Bit().data(), &clearPassword);

        addPasswordToList(passwords, "firefox", url,
                          QString(clearUsername),
                          QString(clearPassword));
    }
    TerminateFirefoxLibrary();

    return passwords;
}

QStringList Firefox::tabs()
{
#ifdef USE_LZ4

    QString lz4file = browserPath() + "\\sessionstore-backups\\recovery.jsonlz4";
    QStringList list;

    auto content = parseJsonlz4RecoveryFilePath(lz4file);
    auto jsonObj = QJsonDocument::fromJson(content).object();
    if (jsonObj == QJsonObject())
        return QStringList();
    auto windows = jsonObj.value("windows").toArray();
    foreach (auto window, windows) {
        auto obj = window.toObject();
        if (obj == QJsonObject())
            continue;

        auto tabs = obj.value("tabs").toArray();
        foreach (auto tab, tabs) {
            auto entries = tab.toObject().value("entries").toArray();
            foreach (auto e, entries) {
                list.append(e.toObject().value("url").toString());
            }
        }
    }
    return list;
#else
    return QStringList();
#endif
}
