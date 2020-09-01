#ifndef OBJECTCACHE_H
#define OBJECTCACHE_H

#include <QMap>
#include <QObject>
#include <QTime>
#include <functional>

#define ObjectCacheInstance ObjectCache::instance()

class QElapsedTimer;
class ObjectCache : public QObject
{
    typedef std::function<void(QObject*)> Callback;

    struct DataEntry{
        QObject *obj;
        QElapsedTimer *timer;
        qint64 timeout;
        Callback callback;

        ~DataEntry();
    };
    QMap<QString, DataEntry*> _dataList;

public:
    ObjectCache();

    static ObjectCache *instance();

    void add(QObject *object, qint64 aliveSecs = -1, Callback callback = {});
    QObject *get(const QString &name) const;
    bool remove(const QString &name, const bool &deleteLater = true);

    template<class T>
    T *add(qint64 aliveSecs, Callback callback = {}) {
        auto o = new T;
        add(o, aliveSecs, callback);
        return o;
    }

    template<class T>
    T *get() const{
        return qobject_cast<T*>(get(T::staticMetaObject.className()));
    }

    template<class T>
    bool remove(const bool &deleteLater = true){
        return remove(T::staticMetaObject.className(), deleteLater);
    }

    template<class T>
    bool contains() {
        return _dataList.contains(T::staticMetaObject.className());
    }

    // QObject interface
protected:
    void timerEvent(QTimerEvent *event);
};

#endif // OBJECTCACHE_H
