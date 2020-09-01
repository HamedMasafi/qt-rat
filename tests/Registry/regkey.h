#ifndef INCLUDE_GIOVANNI_DICANIO_WINREG_HPP
#define INCLUDE_GIOVANNI_DICANIO_WINREG_HPP


////////////////////////////////////////////////////////////////////////////////
//
//      *** Modern C++ Wrappers Around Windows Registry C API ***
//
//               Copyright (C) by Giovanni Dicanio
//
// First version: 2017, January 22nd
// Last update: 2019, September 9th
//
// E-mail: <giovanni.dicanio AT gmail.com>
//
// Registry key handles are safely and conveniently wrapped
// in the RegKey resource manager C++ class.
//
// Errors are signaled throwing exceptions of class RegException.
//
// Unicode UTF-16 strings are represented using the std::wstring class;
// ATL's CString is not used, to avoid dependencies from ATL or MFC.
//
// This is a header-only self-contained reusable module.
//
// Compiler: Visual Studio 2015
// Code compiles cleanly at /W4 on both 32-bit and 64-bit builds.
//
// ===========================================================================
//
// The MIT License(MIT)
//
// Copyright(c) 2017 by Giovanni Dicanio
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
////////////////////////////////////////////////////////////////////////////////


#include <crtdbg.h>         // _ASSERTE

#include <memory>           // std::unique_ptr
#include <stdexcept>        // std::runtime_error
#include <string>           // std::wstring
#include <utility>          // std::swap, std::pair
#include <vector>           // std::vector

#include <QMap>
#include <QStringList>
#include <qt_windows.h>        // Windows Platform SDK

//------------------------------------------------------------------------------
// Safe, efficient and convenient C++ wrapper around HiveKeys registry key handles.
//
// This class is movable but not copyable.
//
// This class is designed to be very *efficient* and low-overhead, for example:
// non-throwing operations are carefully marked as, so the C++ compiler
// can emit optimized code.
//
// Moreover, this class just wraps a raw HiveKeys handle, without any
// shared-ownership overhead like in std::shared_ptr; you can think of this
// class kind of like a std::unique_ptr for HiveKeyss.
//
// The class is also swappable (defines a custom non-member swap);
// relational operators are properly overloaded as well.
//------------------------------------------------------------------------------
class RegKey
{
public:
    enum HiveKeys {
        CLASSES_ROOT,
        CURRENT_USER,
        LOCAL_MACHINE,
        USERS,
        PERFORMANCE_DATA,
        PERFORMANCE_TEXT,
        PERFORMANCE_NLSTEXT,
        CURRENT_CONFIG,
        DYN_DATA,
    };
    //
    // Construction/Destruction
    //

    // Initialize as an empty key handle
    RegKey() = default;

    // Take ownership of the input key handle
    explicit RegKey(HiveKeys HiveKeys);

    // Open the given registry key if it exists, else create a new key.
    // Uses default KEY_READ|KEY_WRITE access.
    // For finer grained control, call the Create() method overloads.
    // Throw RegException on failure.
    RegKey(const HiveKeys &HiveKeysParent, const QString &subKey);

    // Open the given registry key if it exists, else create a new key.
    // Allow the caller to specify the desired access to the key (e.g. KEY_READ
    // for read-only access).
    // For finer grained control, call the Create() method overloads.
    // Throw RegException on failure.
    RegKey(const HiveKeys HiveKeysParent, const QString &subKey, REGSAM desiredAccess);


    // Take ownership of the input key handle.
    // The input key handle wrapper is reset to an empty state.
    RegKey(RegKey&& other);

    // Move-assign from the input key handle.
    // Properly check against self-move-assign (which is safe and does nothing).
    RegKey& operator=(RegKey&& other);

    // Ban copy
    RegKey(const RegKey&) = delete;
    RegKey& operator=(const RegKey&) = delete;

    // Safely close the wrapped key handle (if any)
    ~RegKey();


    //
    // Properties
    //

    // Access the wrapped raw HiveKeys handle
    HKEY Get() const;

    // Is the wrapped HiveKeys handle valid?
    bool IsValid() const;

    // Same as IsValid(), but allow a short "if (regKey)" syntax
    explicit operator bool() const;

    // Is the wrapped handle a predefined handle (e.g.HiveKeys_CURRENT_USER) ?
    bool IsPredefined() const;


    //
    // Operations
    //

    // Close current HiveKeys handle.
    // If there's no valid handle, do nothing.
    // This method doesn't close predefined HiveKeys handles (e.g. HiveKeys_CURRENT_USER).
    void Close();

    // Transfer ownership of current HiveKeys to the caller.
    // Note that the caller is responsible for closing the key handle!
//    HiveKeys Detach();

    // Take ownership of the input HiveKeys handle.
    // Safely close any previously open handle.
    // Input key handle can be nullptr.
//    void Attach(HiveKeys HiveKeys);

    // Non-throwing swap;
    // Note: There's also a non-member swap overload
    void SwapWith(RegKey& other);


    //
    // Wrappers around Windows Registry APIs.
    // See the official MSDN documentation for these APIs for detailed explanations
    // of the wrapper method parameters.
    //

    // Wrapper around RegCreateKeyEx, that allows you to specify desired access
    bool Create(const HiveKeys HiveKeysParent,
        const QString& subKey,
        REGSAM desiredAccess = KEY_READ | KEY_WRITE
    );

    // Wrapper around RegCreateKeyEx
    bool Create(
        HiveKeys HiveKeysParent,
        const QString& subKey,
        REGSAM desiredAccess,
        DWORD options,
        SECURITY_ATTRIBUTES* securityAttributes,
        DWORD* disposition
    );

    // Wrapper around RegOpenKeyEx
    bool Open(HiveKeys HiveKeysParent,
        const QString &subKey,
        REGSAM desiredAccess = KEY_READ | KEY_WRITE
    );


    //
    // Registry Value Setters
    //

    bool SetDwordValue(const QString &valueName, DWORD data);
    bool SetQwordValue(const QString &valueName, const qulonglong& data);
    bool SetStringValue(const QString &valueName, const QString &data);
    bool SetExpandStringValue(const QString &valueName, const QString &data);
    bool SetMultiStringValue(const QString &valueName, const QStringList &data);
    bool SetBinaryValue(const QString &valueName, const QByteArray &data);
    bool SetBinaryValue(const QString &valueName, const void* data, DWORD dataSize);


    //
    // Registry Value Getters
    //

    DWORD GetDwordValue(const QString &valueName) const;
    ULONGLONG GetQwordValue(const QString& valueName) const;
    QString GetStringValue(const QString &valueName) const;

    enum class ExpandStringOption
    {
        DontExpand,
        Expand
    };

    QString GetExpandStringValue(
        const QString &valueName,
        ExpandStringOption expandOption = ExpandStringOption::DontExpand
    ) const;

    QStringList GetMultiStringValue(const QString &valueName) const;
    std::vector<BYTE> GetBinaryValue(const QString &valueName) const;


    //
    // Query Operations
    //

    bool QueryInfoKey(DWORD& subKeys, DWORD &values, FILETIME& lastWriteTime) const;

    // Return the DWORD type ID for the input registry value
    DWORD QueryValueType(const QString &valueName) const;

    // Enumerate the subkeys of the registry key, using RegEnumKeyEx
    QStringList EnumSubKeys() const;

    // Enumerate the values under the registry key, using RegEnumValue.
    // Returns a vector of pairs: In each pair, the wstring is the value name,
    // the DWORD is the value type.
    QMap<QString, QString> EnumValues() const;


    //
    // Misc Registry API Wrappers
    //

    bool DeleteValue(const QString& valueName);
    bool DeleteKey(const QString &subKey, REGSAM desiredAccess);
//    void DeleteTree(const std::wstring& subKey);
//    void CopyTree(const std::wstring& sourceSubKey, const RegKey& destKey);
    bool FlushKey();
    bool LoadKey(const QString& subKey, const QString& filename);
    bool SaveKey(const QString& filename, SECURITY_ATTRIBUTES* securityAttributes);
    bool EnableReflectionKey();
    bool DisableReflectionKey();
    bool QueryReflectionKey();
//    void ConnectRegistry(const std::wstring& machineName, HiveKeys HiveKeysPredefined);


    // Return a string representation of Windows registry types
    static QString RegTypeToString(DWORD regType);

    //
    // Relational comparison operators are overloaded as non-members
    // ==, !=, <, <=, >, >=
    //


private:
    // The wrapped registry key handle
    HKEY m_hKey{ nullptr };
};


//------------------------------------------------------------------------------
// An exception representing an error with the registry operations
//------------------------------------------------------------------------------
/*class RegException
        : public std::runtime_error
{
public:
    RegException(const char* message, LONG errorCode)
        : std::runtime_error{ message }
        , m_errorCode{ errorCode }
    {}

    RegException(const std::string& message, LONG errorCode)
        : std::runtime_error{ message }
        , m_errorCode{ errorCode }
    {}

    // Get the error code returned by Windows registry APIs
    LONG ErrorCode() const
    {
        return m_errorCode;
    }

private:
    // Error code, as returned by Windows registry APIs
    LONG m_errorCode;
}*/;


//------------------------------------------------------------------------------
//          Overloads of relational comparison operators for RegKey
//------------------------------------------------------------------------------

inline bool operator==(const RegKey& a, const RegKey& b)
{
    return a.Get() == b.Get();
}

inline bool operator!=(const RegKey& a, const RegKey& b)
{
    return a.Get() != b.Get();
}

inline bool operator<(const RegKey& a, const RegKey& b)
{
    return a.Get() < b.Get();
}

inline bool operator<=(const RegKey& a, const RegKey& b)
{
    return a.Get() <= b.Get();
}

inline bool operator>(const RegKey& a, const RegKey& b)
{
    return a.Get() > b.Get();
}

inline bool operator>=(const RegKey& a, const RegKey& b)
{
    return a.Get() >= b.Get();
}


//------------------------------------------------------------------------------
//                          RegKey Inline Methods
//------------------------------------------------------------------------------


inline void swap(RegKey& a, RegKey& b);

namespace details
{

// Helper function to build a multi-string from a vector<wstring>
inline std::vector<wchar_t> BuildMultiString(const std::vector<std::wstring>& data)
{
    // Special case of the empty multi-string
    if (data.empty())
    {
        // Build a vector containing just two NULs
        return std::vector<wchar_t>(2, L'\0');
    }

    // Get the total length in wchar_ts of the multi-string
    size_t totalLen = 0;
    for (const auto& s : data)
    {
        // Add one to current string's length for the terminating NUL
        totalLen += (s.length() + 1);
    }

    // Add one for the last NUL terminator (making the whole structure double-NUL terminated)
    totalLen++;

    // Allocate a buffer to store the multi-string
    std::vector<wchar_t> multiString;
    multiString.reserve(totalLen);

    // Copy the single strings into the multi-string
    for (const auto& s : data)
    {
        multiString.insert(multiString.end(), s.begin(), s.end());

        // Don't forget to NUL-terminate the current string
        multiString.push_back(L'\0');
    }

    // Add the last NUL-terminator
    multiString.push_back(L'\0');

    return multiString;
}

inline std::vector<wchar_t> BuildMultiString(const QStringList& data)
{
    QByteArray ba;

    // Special case of the empty multi-string
    if (data.empty())
    {
        // Build a vector containing just two NULs
        return std::vector<wchar_t>(2, L'\0');
    }

    // Get the total length in wchar_ts of the multi-string
    size_t totalLen = 0;
    for (const auto& s : data)
    {
        // Add one to current string's length for the terminating NUL
        totalLen += (s.length() + 1);
    }

    // Add one for the last NUL terminator (making the whole structure double-NUL terminated)
    totalLen++;

    // Allocate a buffer to store the multi-string
    std::vector<wchar_t> multiString;
    multiString.reserve(totalLen);

    // Copy the single strings into the multi-string
    for (const auto& s : data)
    {
        auto ss = s.toStdWString();
        multiString.insert(multiString.end(), ss.begin(), ss.end());

        // Don't forget to NUL-terminate the current string
        multiString.push_back(L'\0');
    }

    // Add the last NUL-terminator
    multiString.push_back(L'\0');

    return multiString;
}

} // namespace details









//inline void RegKey::DeleteTree(const std::wstring& subKey)
//{
//    _ASSERTE(IsValid());

//    LONG retCode = ::RegDeleteTree(m_HiveKeys, subKey.c_str());
//    if (retCode != ERROR_SUCCESS)
//    {
//        throw RegException{ "RegDeleteTree failed.", retCode };
//    }
//}


//inline void RegKey::CopyTree(const std::wstring& sourceSubKey, const RegKey& destKey)
//{
//    _ASSERTE(IsValid());

//    LONG retCode = ::RegCopyTree(m_HiveKeys, sourceSubKey.c_str(), destKey.Get());
//    if (retCode != ERROR_SUCCESS)
//    {
//        throw RegException{ "RegCopyTree failed.", retCode };
//    }
//}


#endif
