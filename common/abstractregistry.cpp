#include "abstractregistry.h"

#if defined(Q_OS_WIN) && defined(NEURON_CLIENT)
#   define ENABLE_REGISTRY
#endif

#ifdef ENABLE_REGISTRY
#   include "regkey.h"


static HKEY toHkey(const AbstractRegistry::HiveKeys &hive)
{
    switch (hive) {
    case AbstractRegistry::CLASSES_ROOT: return HKEY_CLASSES_ROOT;
    case AbstractRegistry::CURRENT_USER: return HKEY_CURRENT_USER;
    case AbstractRegistry::LOCAL_MACHINE: return HKEY_LOCAL_MACHINE;
    case AbstractRegistry::USERS: return HKEY_USERS;
//    case PERFORMANCE_DATA: return HKEY_PERFORMANCE_DATA;
//    case PERFORMANCE_TEXT: return HKEY_PERFORMANCE_TEXT;
//    case PERFORMANCE_NLSTEXT: return HKEY_PERFORMANCE_NLSTEXT;
    case AbstractRegistry::CURRENT_CONFIG: return HKEY_CURRENT_CONFIG;
//    case DYN_DATA: return HKEY_DYN_DATA;
    default:
        return nullptr;
    }
}
static HKEY toHkey(const int &hive)
{
    return toHkey(static_cast<AbstractRegistry::HiveKeys>(hive));
}
#endif

N_CLASS_IMPL(AbstractRegistry)
{
}

N_REMOTE_METHOD_IMPL(AbstractRegistry, QVariantMap, openKey, int, key, QString, path)
{
#ifdef ENABLE_REGISTRY
    QVariantMap map;
    RegKey r{toHkey(key), path};
    map.insert("keys", r.EnumSubKeys());
    map.insert("values", valuesSlot(key, path));
    return map;
#endif
}
N_REMOTE_METHOD_IMPL(AbstractRegistry, QVariantList, values, int, key, QString, path)
{
#ifdef ENABLE_REGISTRY
    RegKey r{toHkey(key), path};

    QVariantList valuesList;
    auto values = r.EnumValues();
    for (auto i = values.begin(); i != values.end(); ++i) {
        QVariantMap v;
        auto type = i.value();
        v.insert("name", i.key());
        v.insert("type", RegKey::RegTypeToString(type));

        switch (type) {
        case REG_SZ:
            v.insert("value", r.GetStringValue(i.key()));
            break;

        case REG_DWORD:
            v.insert("value", static_cast<quint32>(r.GetDwordValue(i.key())));
            break;

        case REG_QWORD:
            v.insert("value", static_cast<qulonglong>(r.GetQwordValue(i.key())));
            break;

        case REG_BINARY:
//            v.insert("value", static_cast<qulonglong>(r.GetBinaryValue(i.key())));
            break;

        case REG_MULTI_SZ:
            v.insert("value", r.GetMultiStringValue(i.key()).join(", "));
            break;
        }

        valuesList.append(v);
    }
    return valuesList;
#endif
}

N_REMOTE_METHOD_IMPL(AbstractRegistry, QVariant, readValue, int, key, QString, path, QString, valueName)
{
    Q_UNUSED(key)
    Q_UNUSED(path)
    Q_UNUSED(valueName)
#ifdef ENABLE_REGISTRY
    return QVariant();
#else
    return QVariant();
#endif
}

N_REMOTE_METHOD_IMPL(AbstractRegistry, bool, setValue, int, key, QString, path, QString, name, QVariant, value)
{
#ifdef ENABLE_REGISTRY
    RegKey r{toHkey(key), path};
    auto type = r.QueryValueType(name);
    bool b = false;
    switch (type) {
    case REG_DWORD:
        b = r.SetDwordValue(name, value.value<quint32>());
        break;
    case REG_QWORD:
        b = r.SetQwordValue(name, value.toULongLong());
        break;
    case REG_SZ:
        b = r.SetStringValue(name, value.toString());
        break;
    case REG_MULTI_SZ:
        b = r.SetMultiStringValue(name, value.toStringList());
        break;
    case REG_EXPAND_SZ:
        b = r.SetExpandStringValue(name, value.toString());
        break;
    case REG_BINARY:
        b = r.SetBinaryValue(name, value.toByteArray());
        break;
    }

    return b;
#endif
}

N_REMOTE_METHOD_IMPL(AbstractRegistry, bool, deleteValue, int, key, QString, path, QString, name)
{
#ifdef ENABLE_REGISTRY
    RegKey r{toHkey(key), path};
    return r.DeleteValue(name);
#else
    Q_UNUSED(key)
    Q_UNUSED(path)
    Q_UNUSED(name)
    return false;
#endif
}
