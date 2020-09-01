#ifndef ABSTRACTREGISTRY_H
#define ABSTRACTREGISTRY_H

#include <SharedObject>

class AbstractRegistry : public Neuron::SharedObject
{
    Q_OBJECT
public:
    enum HiveKeys {
        CLASSES_ROOT,
        CURRENT_USER,
        LOCAL_MACHINE,
        USERS,
        CURRENT_CONFIG,
//        PERFORMANCE_DATA,
//        PERFORMANCE_TEXT,
//        PERFORMANCE_NLSTEXT,
//        DYN_DATA,
    };
    Q_ENUM(HiveKeys)

    N_CLASS_DECL(AbstractRegistry)

    N_REMOTE_METHOD_DECL(QVariantMap, openKey, int, key, QString, path)
    N_REMOTE_METHOD_DECL(QVariantList, values, int, key, QString, path)
    N_REMOTE_METHOD_DECL(QVariant, readValue, int, key, QString, path, QString, name)
    N_REMOTE_METHOD_DECL(bool, setValue, int, key, QString, path, QString, name, QVariant, value)
    N_REMOTE_METHOD_DECL(bool, deleteValue, int, key, QString, path, QString, name)

};

#endif // ABSTRACTREGISTRY_H
