#include "regkey.h"
#include <QBitArray>
#include <QDebug>

#include <qt_windows.h>        // Windows Platform SDK

#define QSTR2WSTR(s) reinterpret_cast<const wchar_t *>(s.utf16())

//static HKEY toHkey(const RegKey::HKEY &hive)
//{
//    switch (hive) {
//    case RegKey::CLASSES_ROOT: return HKEY_CLASSES_ROOT;
//    case RegKey::CURRENT_USER: return HKEY_CURRENT_USER;
//    case RegKey::LOCAL_MACHINE: return HKEY_LOCAL_MACHINE;
//    case RegKey::USERS: return HKEY_USERS;
////    case PERFORMANCE_DATA: return HKEY_PERFORMANCE_DATA;
////    case PERFORMANCE_TEXT: return HKEY_PERFORMANCE_TEXT;
////    case PERFORMANCE_NLSTEXT: return HKEY_PERFORMANCE_NLSTEXT;
//    case RegKey::CURRENT_CONFIG: return HKEY_CURRENT_CONFIG;
////    case DYN_DATA: return HKEY_DYN_DATA;
//    default:
//        break;
//    }
//}

QStringList RegKey::EnumSubKeys() const
{
    if (!IsValid())
        return QStringList();
//    _ASSERTE(IsValid());

    // Get some useful enumeration info, like the total number of subkeys
    // and the maximum length of the subkey names
    DWORD subKeyCount{};
    DWORD maxSubKeyNameLen{};
    LONG retCode = ::RegQueryInfoKey(
                m_hKey,
                nullptr,    // no user-defined class
                nullptr,    // no user-defined class size
                nullptr,    // reserved
                &subKeyCount,
                &maxSubKeyNameLen,
                nullptr,    // no subkey class length
                nullptr,    // no value count
                nullptr,    // no value name max length
                nullptr,    // no max value length
                nullptr,    // no security descriptor
                nullptr     // no last write time
                );
    if (retCode != ERROR_SUCCESS)
    {
        return QStringList();
//        throw RegException{ "RegQueryInfoKey failed while preparing for subkey enumeration.", retCode };
    }

    // NOTE: According to the MSDN documentation, the size returned for subkey name max length
    // does *not* include the terminating NUL, so let's add +1 to take it into account
    // when I allocate the buffer for reading subkey names.
    maxSubKeyNameLen++;

    // Preallocate a buffer for the subkey names
    auto nameBuffer = std::make_unique<wchar_t[]>(maxSubKeyNameLen);

    // The result subkey names will be stored here
    QStringList subkeyNames;

    // Enumerate all the subkeys
    for (DWORD index = 0; index < subKeyCount; index++)
    {
        // Get the name of the current subkey
        DWORD subKeyNameLen = maxSubKeyNameLen;
        retCode = ::RegEnumKeyEx(
                    m_hKey,
                    index,
                    nameBuffer.get(),
                    &subKeyNameLen,
                    nullptr, // reserved
                    nullptr, // no class
                    nullptr, // no class
                    nullptr  // no last write time
                    );
        if (retCode != ERROR_SUCCESS)
        {
            return QStringList();
//            throw RegException{ "Cannot enumerate subkeys: RegEnumKeyEx failed.", retCode };
        }

        // On success, the ::RegEnumKeyEx API writes the length of the
        // subkey name in the subKeyNameLen output parameter
        // (not including the terminating NUL).
        // So I can build a wstring based on that length.
        subkeyNames.append(QString::fromWCharArray(nameBuffer.get(), subKeyNameLen));
    }

    return subkeyNames;
}

QMap<QString, quint32> RegKey::EnumValues() const
{
    _ASSERTE(IsValid());

    // Get useful enumeration info, like the total number of values
    // and the maximum length of the value names
    DWORD valueCount{};
    DWORD maxValueNameLen{};
    LONG retCode = ::RegQueryInfoKey(
                m_hKey,
                nullptr,    // no user-defined class
                nullptr,    // no user-defined class size
                nullptr,    // reserved
                nullptr,    // no subkey count
                nullptr,    // no subkey max length
                nullptr,    // no subkey class length
                &valueCount,
                &maxValueNameLen,
                nullptr,    // no max value length
                nullptr,    // no security descriptor
                nullptr     // no last write time
                );
    if (retCode != ERROR_SUCCESS)
    {
        return QMap<QString, quint32>();
//        throw RegException{
//            "RegQueryInfoKey failed while preparing for value enumeration.",
//            retCode
//        };
    }

    // NOTE: According to the MSDN documentation, the size returned for value name max length
    // does *not* include the terminating NUL, so let's add +1 to take it into account
    // when I allocate the buffer for reading value names.
    maxValueNameLen++;

    // Preallocate a buffer for the value names
    auto nameBuffer = std::make_unique<wchar_t[]>(maxValueNameLen);

    // The value names and types will be stored here
    QMap<QString, quint32> valueInfo;

    // Enumerate all the values
    for (DWORD index = 0; index < valueCount; index++)
    {
        // Get the name and the type of the current value
        DWORD valueNameLen = maxValueNameLen;
        DWORD valueType{};
        retCode = ::RegEnumValue(
                    m_hKey,
                    index,
                    nameBuffer.get(),
                    &valueNameLen,
                    nullptr,    // reserved
                    &valueType,
                    nullptr,    // no data
                    nullptr     // no data size
                    );
        if (retCode != ERROR_SUCCESS)
        {
            return QMap<QString, quint32>();
//            throw RegException{ "Cannot enumerate values: RegEnumValue failed.", retCode };
        }

        if (valueType == REG_SZ) {
//            auto v = GetStringValue(std::wstring{nameBuffer.get(), valueNameLen});
//            qDebug() << v;
        }
        // On success, the RegEnumValue API writes the length of the
        // value name in the valueNameLen output parameter
        // (not including the terminating NUL).
        // So we can build a wstring based on that.
        valueInfo.insert(QString::fromWCharArray(nameBuffer.get(), valueNameLen),
                         valueType);
    }

    return valueInfo;
}



RegKey::RegKey(const HKEY hKey)
    : m_hKey{ hKey }
{}

RegKey::RegKey(const HKEY &hKeyParent, const QString &subKey)
{
    Create(hKeyParent, subKey);
}

RegKey::RegKey(const HKEY hKeyParent, const QString &subKey, REGSAM desiredAccess)
{
    Create(hKeyParent, subKey, desiredAccess);
}

RegKey::RegKey(RegKey &&other)
    : m_hKey{ other.m_hKey }
{
    // Other doesn't own the handle anymore
    other.m_hKey = nullptr;
}

RegKey::~RegKey()
{
    // Release the owned handle (if any)
    Close();
}

HKEY RegKey::Get() const
{
    return m_hKey;
}

void RegKey::Close()
{
    if (IsValid())
    {
        // Do not call RegCloseKey on predefined keys
        if (! IsPredefined())
        {
            ::RegCloseKey(m_hKey);
        }

        // Avoid dangling references
        m_hKey = nullptr;
    }
}

bool RegKey::IsValid() const
{
    return m_hKey != nullptr;
}

RegKey::operator bool() const
{
    return IsValid();
}

RegKey &RegKey::operator=(RegKey &&other)
{
    // Prevent self-move-assign
    if ((this != &other) && (m_hKey != other.m_hKey))
    {
        // Close current
        Close();

        // Move from other (i.e. take ownership of other's raw handle)
        m_hKey = other.m_hKey;
        other.m_hKey = nullptr;
    }
    return *this;
}

bool RegKey::IsPredefined() const
{
    // Predefined keys
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms724836(v=vs.85).aspx

    if (   (m_hKey == HKEY_CURRENT_USER)
           || (m_hKey == HKEY_LOCAL_MACHINE)
           || (m_hKey == HKEY_CLASSES_ROOT)
           || (m_hKey == HKEY_CURRENT_CONFIG)
           //        || (m_hKey == HKEY_CURRENT_USER_LOCAL_SETTINGS)
           || (m_hKey == HKEY_PERFORMANCE_DATA)
           || (m_hKey == HKEY_PERFORMANCE_NLSTEXT)
           || (m_hKey == HKEY_PERFORMANCE_TEXT)
           || (m_hKey == HKEY_USERS))
    {
        return true;
    }

    return false;
}

//HKEY RegKey::Detach()
//{
//    HKEY hKey{ m_hKey };

//    // We don't own the HKEY handle anymore
//    m_hKey = nullptr;

//    // Transfer ownership to the caller
//    return hKey;
//}

//void RegKey::Attach(const HKEY hKey)
//{
//    // Prevent self-attach
//    if (m_hKey != hKey)
//    {
//        // Close any open registry handle
//        Close();

//        // Take ownership of the input hKey
//        m_hKey = hKey;
//    }
//}

void RegKey::SwapWith(RegKey &other)
{
    // Enable ADL (not necessary in this case, but good practice)
    using std::swap;

    // Swap the raw handle members
    swap(m_hKey, other.m_hKey);
}

void swap(RegKey &a, RegKey &b)
{
    a.SwapWith(b);
}

bool RegKey::Create(const HKEY hKeyParent, const QString &subKey, const REGSAM desiredAccess)
{
    constexpr DWORD kDefaultOptions = REG_OPTION_NON_VOLATILE;

    return Create(hKeyParent, subKey, desiredAccess, kDefaultOptions,
           nullptr, // no security attributes,
           nullptr  // no disposition
           );
}

bool RegKey::Create(const HKEY hKeyParent, const QString &subKey, const REGSAM desiredAccess, const DWORD options, SECURITY_ATTRIBUTES * const securityAttributes, DWORD * const disposition)
{
    HKEY hKey{ nullptr };
    LONG retCode = ::RegCreateKeyEx(
                hKeyParent,
                QSTR2WSTR(subKey),
                0,          // reserved
                REG_NONE,   // user-defined class type parameter not supported
                options,
                desiredAccess,
                securityAttributes,
                &hKey,
                disposition
                );
    if (retCode != ERROR_SUCCESS)
    {
        return false;
//        throw RegException{ "RegCreateKeyEx failed.", retCode };
    }

    // Safely close any previously opened key
    Close();

    // Take ownership of the newly created key
    m_hKey = hKey;
    return true;
}

bool RegKey::Open(const HKEY hKeyParent, const QString &subKey, const REGSAM desiredAccess)
{
    HKEY hKey{ nullptr };
    LONG retCode = ::RegOpenKeyEx(
                hKeyParent,
                QSTR2WSTR(subKey),
                REG_NONE,           // default options
                desiredAccess,
                &hKey
                );
    if (retCode != ERROR_SUCCESS)
    {
        return false;
//        throw RegException{ "RegOpenKeyEx failed.", retCode };
    }

    // Safely close any previously opened key
    Close();

    // Take ownership of the newly created key
    m_hKey = hKey;
    return true;
}

bool RegKey::SetDwordValue(const QString &valueName, const DWORD data)
{
    _ASSERTE(IsValid());

    LONG retCode = ::RegSetValueEx(
                m_hKey,
                QSTR2WSTR(valueName),
                0, // reserved
                REG_DWORD,
                reinterpret_cast<const BYTE*>(&data),
                sizeof(data)
                );
//    if (retCode != ERROR_SUCCESS)
//    {
//        throw RegException{ "Cannot write DWORD value: RegSetValueEx failed.", retCode };
//    }
    return retCode == ERROR_SUCCESS;
}

bool RegKey::SetQwordValue(const QString &valueName, const qulonglong &data)
{
    _ASSERTE(IsValid());

    LONG retCode = ::RegSetValueEx(
                m_hKey,
                QSTR2WSTR(valueName),
                0, // reserved
                REG_QWORD,
                reinterpret_cast<const BYTE*>(&data),
                sizeof(data)
                );
//    if (retCode != ERROR_SUCCESS)
//    {
//        throw RegException{ "Cannot write QWORD value: RegSetValueEx failed.", retCode };
//    }
    return retCode == ERROR_SUCCESS;
}

bool RegKey::SetStringValue(const QString &valueName, const QString &data)
{
    _ASSERTE(IsValid());

    // String size including the terminating NUL, in bytes
    const DWORD dataSize = static_cast<DWORD>((data.length() + 1) * sizeof(wchar_t));

    LONG retCode = ::RegSetValueEx(
                m_hKey,
                QSTR2WSTR(valueName),
                0, // reserved
                REG_SZ,
                reinterpret_cast<const BYTE*>(QSTR2WSTR(data)),
                dataSize
                );
//    if (retCode != ERROR_SUCCESS)
//    {
//        throw RegException{ "Cannot write string value: RegSetValueEx failed.", retCode };
//    }
    return retCode == ERROR_SUCCESS;
}

bool RegKey::SetExpandStringValue(const QString &valueName, const QString &data)
{
    _ASSERTE(IsValid());

    // String size including the terminating NUL, in bytes
    const DWORD dataSize = static_cast<DWORD>((data.length() + 1) * sizeof(wchar_t));

    LONG retCode = ::RegSetValueEx(
                m_hKey,
                QSTR2WSTR(valueName),
                0, // reserved
                REG_EXPAND_SZ,
                reinterpret_cast<const BYTE*>(QSTR2WSTR(data)),
                dataSize
                );
//    if (retCode != ERROR_SUCCESS)
//    {
//        throw RegException{ "Cannot write expand string value: RegSetValueEx failed.", retCode };
//    }
    return retCode == ERROR_SUCCESS;
}

bool RegKey::SetMultiStringValue(const QString &valueName, const QStringList &data)
{
    _ASSERTE(IsValid());

    // First, we have to build a double-NUL-terminated multi-string from the input data
    const std::vector<wchar_t> multiString = details::BuildMultiString(data);

    // Total size, in bytes, of the whole multi-string structure
    const DWORD dataSize = static_cast<DWORD>(multiString.size() * sizeof(wchar_t));

    LONG retCode = ::RegSetValueEx(
                m_hKey,
                QSTR2WSTR(valueName),
                0, // reserved
                REG_MULTI_SZ,
                reinterpret_cast<const BYTE*>(&multiString[0]),
            dataSize
            );
//    if (retCode != ERROR_SUCCESS)
//    {
//        throw RegException{ "Cannot write multi-string value: RegSetValueEx failed.", retCode };
//    }
    return retCode == ERROR_SUCCESS;
}

bool RegKey::SetBinaryValue(const QString &valueName, const QByteArray &data)
{
    _ASSERTE(IsValid());

    // Total data size, in bytes
    std::vector<BYTE> binaryData(data.begin(), data.end());
    const DWORD dataSize = static_cast<DWORD>(data.size());

    LONG retCode = ::RegSetValueEx(
                m_hKey,
                QSTR2WSTR(valueName),
                0, // reserved
                REG_BINARY,
                &binaryData[0],
                dataSize
            );
//    if (retCode != ERROR_SUCCESS)
//    {
//        throw RegException{ "Cannot write binary data value: RegSetValueEx failed.", retCode };
//    }
    return retCode == ERROR_SUCCESS;
}

//qint32 RegKey::GetType(const QString &valueName) const
//{
//    _ASSERTE(IsValid());

//    LPDWORD type = nullptr;
//    const DWORD flags = RRF_RT_REG_DWORD;
//    DWORD dataSize = sizeof(DWORD);  // size of data, in bytes

//    LONG retCode = ::RegGetValue(
//                m_hKey,
//                nullptr, // no subkey
//                QSTR2WSTR(valueName),
//                flags,
//                type, // type not required
//                nullptr,
//                &dataSize
//                );
//    if (retCode != ERROR_SUCCESS)
//        return 0;
////    {
////        throw RegException{ "Cannot get DWORD value: RegGetValue failed.", retCode };
////    }

//    return *type;
//}

bool RegKey::SetBinaryValue(const QString &valueName, const void * const data, const DWORD dataSize)
{
    _ASSERTE(IsValid());

    LONG retCode = ::RegSetValueEx(
                m_hKey,
                QSTR2WSTR(valueName),
                0, // reserved
                REG_BINARY,
                static_cast<const BYTE*>(data),
                dataSize
                );
//    if (retCode != ERROR_SUCCESS)
//    {
//        throw RegException{ "Cannot write binary data value: RegSetValueEx failed.", retCode };
//    }
    return retCode == ERROR_SUCCESS;
}

DWORD RegKey::GetDwordValue(const QString &valueName) const
{
    _ASSERTE(IsValid());

    DWORD data{};                   // to be read from the registry
    DWORD dataSize = sizeof(data);  // size of data, in bytes

    const DWORD flags = RRF_RT_REG_DWORD;
    LONG retCode = ::RegGetValue(
                m_hKey,
                nullptr, // no subkey
                QSTR2WSTR(valueName),
                flags,
                nullptr, // type not required
                &data,
                &dataSize
                );
    if (retCode != ERROR_SUCCESS)
        return 0;
//    {
//        throw RegException{ "Cannot get DWORD value: RegGetValue failed.", retCode };
//    }

    return data;
}

ULONGLONG RegKey::GetQwordValue(const QString &valueName) const
{
    _ASSERTE(IsValid());

    ULONGLONG data{};               // to be read from the registry
    DWORD dataSize = sizeof(data);  // size of data, in bytes

    const DWORD flags = RRF_RT_REG_QWORD;
    LONG retCode = ::RegGetValue(
                m_hKey,
                nullptr, // no subkey
                QSTR2WSTR(valueName),
                flags,
                nullptr, // type not required
                &data,
                &dataSize
                );
    if (retCode != ERROR_SUCCESS)
        return 0;
//    {
//        throw RegException{ "Cannot get QWORD value: RegGetValue failed.", retCode };
//    }

    return data;
}

QString RegKey::GetStringValue(const QString &valueName) const
{
    _ASSERTE(IsValid());

    // Get the size of the result string
    DWORD dataSize = 0; // size of data, in bytes
    const DWORD flags = RRF_RT_REG_SZ;

    LONG retCode = ::RegGetValue(
                m_hKey,
                nullptr, // no subkey
                reinterpret_cast<const wchar_t *>(valueName.utf16()),
                flags,
                nullptr, // type not required
                nullptr, // output buffer not needed now
                &dataSize
                );
    if (retCode != ERROR_SUCCESS)
    {
        return QString();
//        throw RegException{ "Cannot get size of string value: RegGetValue failed.", retCode };
    }

    // Allocate a string of proper size.
    // Note that dataSize is in bytes and includes the terminating NUL;
    // we have to convert the size from bytes to wchar_ts for wstring::resize.
    std::wstring result;
    result.resize(dataSize / sizeof(wchar_t));

    // Call RegGetValue for the second time to read the string's content
    retCode = ::RegGetValue(
                m_hKey,
                nullptr,    // no subkey
                reinterpret_cast<const wchar_t *>(valueName.utf16()),
                flags,
                nullptr,    // type not required
                &result[0], // output buffer
            &dataSize
            );
    if (retCode != ERROR_SUCCESS)
    {
        return QString();
//        throw RegException{ "Cannot get string value: RegGetValue failed.", retCode };
    }

    // Remove the NUL terminator scribbled by RegGetValue from the wstring
    result.resize((dataSize / sizeof(wchar_t)) - 1);

    return QString::fromStdWString(result);
}

QString RegKey::GetExpandStringValue(const QString &valueName, const ExpandStringOption expandOption) const
{
    _ASSERTE(IsValid());

    DWORD flags = RRF_RT_REG_EXPAND_SZ;

    // Adjust the flag for RegGetValue considering the expand string option specified by the caller
    if (expandOption == ExpandStringOption::DontExpand)
    {
        flags |= RRF_NOEXPAND;
    }

    // Get the size of the result string
    DWORD dataSize = 0; // size of data, in bytes
    LONG retCode = ::RegGetValue(
                m_hKey,
                nullptr,    // no subkey
                QSTR2WSTR(valueName),
                flags,
                nullptr,    // type not required
                nullptr,    // output buffer not needed now
                &dataSize
                );
    if (retCode != ERROR_SUCCESS)
    {
        return QString();
//        throw RegException{ "Cannot get size of expand string value: RegGetValue failed.", retCode };
    }

    // Allocate a string of proper size.
    // Note that dataSize is in bytes and includes the terminating NUL.
    // We must convert from bytes to wchar_ts for wstring::resize.
    std::wstring result;
    result.resize(dataSize / sizeof(wchar_t));

    // Call RegGetValue for the second time to read the string's content
    retCode = ::RegGetValue(
                m_hKey,
                nullptr,    // no subkey
                QSTR2WSTR(valueName),
                flags,
                nullptr,    // type not required
                &result[0], // output buffer
            &dataSize
            );
    if (retCode != ERROR_SUCCESS)
    {
        return QString();
//        throw RegException{ "Cannot get expand string value: RegGetValue failed.", retCode };
    }

    // Remove the NUL terminator scribbled by RegGetValue from the wstring
    result.resize((dataSize / sizeof(wchar_t)) - 1);

    return QString::fromStdWString(result);
}

QStringList RegKey::GetMultiStringValue(const QString &valueName) const
{
    _ASSERTE(IsValid());

    // Request the size of the multi-string, in bytes
    DWORD dataSize = 0;
    const DWORD flags = RRF_RT_REG_MULTI_SZ;
    LONG retCode = ::RegGetValue(
                m_hKey,
                nullptr,    // no subkey
                QSTR2WSTR(valueName),
                flags,
                nullptr,    // type not required
                nullptr,    // output buffer not needed now
                &dataSize
                );
    if (retCode != ERROR_SUCCESS)
    {
        return QStringList();
//        throw RegException{ "Cannot get size of multi-string value: RegGetValue failed.", retCode };
    }

    // Allocate room for the result multi-string.
    // Note that dataSize is in bytes, but our vector<wchar_t>::resize method requires size
    // to be expressed in wchar_ts.
    std::vector<wchar_t> data;
    data.resize(dataSize / sizeof(wchar_t));

    // Read the multi-string from the registry into the vector object
    retCode = ::RegGetValue(
                m_hKey,
                nullptr,    // no subkey
                QSTR2WSTR(valueName),
                flags,
                nullptr,    // no type required
                &data[0],   // output buffer
            &dataSize
            );
    if (retCode != ERROR_SUCCESS)
    {
        return QStringList();
//        throw RegException{ "Cannot get multi-string value: RegGetValue failed.", retCode };
    }

    // Resize vector to the actual size returned by GetRegValue.
    // Note that the vector is a vector of wchar_ts, instead the size returned by GetRegValue
    // is in bytes, so we have to scale from bytes to wchar_t count.
    data.resize(dataSize / sizeof(wchar_t));

    // Parse the double-NUL-terminated string into a vector<wstring>,
    // which will be returned to the caller
    std::vector<std::wstring> result;
    QStringList list;
    const wchar_t* currStringPtr = &data[0];
    while (*currStringPtr != L'\0')
    {
        // Current string is NUL-terminated, so get its length calling wcslen
        const size_t currStringLength = wcslen(currStringPtr);

        // Add current string to the result vector
        result.push_back(std::wstring{ currStringPtr, currStringLength });
        list.append(QString::fromWCharArray(currStringPtr, currStringLength));
        // Move to the next string
        currStringPtr += currStringLength + 1;
    }

    return list;
}

std::vector<BYTE> RegKey::GetBinaryValue(const QString &valueName) const
{
    _ASSERTE(IsValid());

    // Get the size of the binary data
    DWORD dataSize = 0; // size of data, in bytes
    const DWORD flags = RRF_RT_REG_BINARY;
    LONG retCode = ::RegGetValue(
                m_hKey,
                nullptr,    // no subkey
                QSTR2WSTR(valueName),
                flags,
                nullptr,    // type not required
                nullptr,    // output buffer not needed now
                &dataSize
                );
    if (retCode != ERROR_SUCCESS)
    {
        return std::vector<BYTE>();
//        throw RegException{ "Cannot get size of binary data: RegGetValue failed.", retCode };
    }

    // Allocate a buffer of proper size to store the binary data
    std::vector<BYTE> data(dataSize);
    QByteArray d;
    d.reserve(dataSize);

    // Call RegGetValue for the second time to read the data content
    retCode = ::RegGetValue(
                m_hKey,
                nullptr,    // no subkey
                QSTR2WSTR(valueName),
                flags,
                nullptr,    // type not required
                &data[0],   // output buffer
            &dataSize
            );
    if (retCode != ERROR_SUCCESS)
    {
        return std::vector<BYTE>();
//        throw RegException{ "Cannot get binary data: RegGetValue failed.", retCode };
    }

    return data;
}

DWORD RegKey::QueryValueType(const QString &valueName) const
{
    _ASSERTE(IsValid());

    DWORD typeId{};     // will be returned by RegQueryValueEx

    LONG retCode = ::RegQueryValueEx(
                m_hKey,
                QSTR2WSTR(valueName),
                nullptr,    // reserved
                &typeId,
                nullptr,    // not interested
                nullptr     // not interested
                );

    if (retCode != ERROR_SUCCESS)
    {
        return 0;
//        throw RegException{ "Cannot get the value type: RegQueryValueEx failed.", retCode };
    }

    return typeId;
}

bool RegKey::QueryInfoKey(DWORD &subKeys, DWORD &values, FILETIME &lastWriteTime) const
{
    _ASSERTE(IsValid());

    LONG retCode = ::RegQueryInfoKey(
                m_hKey,
                nullptr,
                nullptr,
                nullptr,
                &subKeys,
                nullptr,
                nullptr,
                &values,
                nullptr,
                nullptr,
                nullptr,
                &lastWriteTime
                );
//    if (retCode != ERROR_SUCCESS)
//    {
//        throw RegException{ "RegQueryInfoKey failed.", retCode };
//    }
    return retCode == ERROR_SUCCESS;
}

bool RegKey::DeleteValue(const QString &valueName)
{
    _ASSERTE(IsValid());

    LONG retCode = ::RegDeleteValue(m_hKey, QSTR2WSTR(valueName));
//    if (retCode != ERROR_SUCCESS)
//    {
//        throw RegException{ "RegDeleteValue failed.", retCode };
//    }
    return retCode == ERROR_SUCCESS;
}


bool RegKey::DeleteKey(const QString &subKey, const REGSAM desiredAccess)
{
    _ASSERTE(IsValid());

    LONG retCode = ::RegDeleteKeyEx(m_hKey, QSTR2WSTR(subKey), desiredAccess, 0);
//    if (retCode != ERROR_SUCCESS)
//    {
//        throw RegException{ "RegDeleteKeyEx failed.", retCode };
//    }
    return retCode == ERROR_SUCCESS;
}

bool RegKey::FlushKey()
{
    _ASSERTE(IsValid());

    LONG retCode = ::RegFlushKey(m_hKey);
//    if (retCode != ERROR_SUCCESS)
//    {
//        throw RegException{ "RegFlushKey failed.", retCode };
//    }
    return retCode == ERROR_SUCCESS;
}

bool RegKey::LoadKey(const QString &subKey, const QString &filename)
{
    Close();

    LONG retCode = ::RegLoadKey(m_hKey, QSTR2WSTR(subKey), QSTR2WSTR(filename));
//    if (retCode != ERROR_SUCCESS)
//    {
//        throw RegException{ "RegLoadKey failed.", retCode };
//    }
    return retCode == ERROR_SUCCESS;
}

bool RegKey::SaveKey(const QString &filename, SECURITY_ATTRIBUTES * const securityAttributes)
{
    _ASSERTE(IsValid());

    LONG retCode = ::RegSaveKey(m_hKey, QSTR2WSTR(filename), securityAttributes);
//    if (retCode != ERROR_SUCCESS)
//    {
//        throw RegException{ "RegSaveKey failed.", retCode };
//    }
    return retCode == ERROR_SUCCESS;
}

bool RegKey::EnableReflectionKey()
{
    LONG retCode = ::RegEnableReflectionKey(m_hKey);
//    if (retCode != ERROR_SUCCESS)
//    {
//        throw RegException{ "RegEnableReflectionKey failed.", retCode };
//    }
    return retCode == ERROR_SUCCESS;
}

bool RegKey::DisableReflectionKey()
{
    LONG retCode = ::RegDisableReflectionKey(m_hKey);
//    if (retCode != ERROR_SUCCESS)
//    {
//        throw RegException{ "RegDisableReflectionKey failed.", retCode };
//    }
    return retCode == ERROR_SUCCESS;
}

bool RegKey::QueryReflectionKey()
{
    BOOL isReflectionDisabled = FALSE;
    LONG retCode = ::RegQueryReflectionKey(m_hKey, &isReflectionDisabled);
    if (retCode != ERROR_SUCCESS)
        return false;
//    {
//        throw RegException{ "RegQueryReflectionKey failed.", retCode };
//    }

    return (isReflectionDisabled ? true : false);
}

//void RegKey::ConnectRegistry(const std::wstring &machineName, const HKEY hKeyPredefined)
//{
//    // Safely close any previously opened key
//    Close();

//    HKEY hKeyResult{ nullptr };
//    LONG retCode = ::RegConnectRegistry(machineName.c_str(), hKeyPredefined, &hKeyResult);
//    if (retCode != ERROR_SUCCESS)
//    {
//        throw RegException{ "RegConnectRegistry failed.", retCode };
//    }

//    // Take ownership of the result key
//    m_hKey = hKeyResult;
//}

QString RegKey::RegTypeToString(const DWORD regType)
{
    switch (regType)
    {
    case REG_SZ:        return "REG_SZ";
    case REG_EXPAND_SZ: return "REG_EXPAND_SZ";
    case REG_MULTI_SZ:  return "REG_MULTI_SZ";
    case REG_DWORD:     return "REG_DWORD";
    case REG_QWORD:     return "REG_QWORD";
    case REG_BINARY:    return "REG_BINARY";

    default:            return "Unknown/unsupported registry type";
    }
}
