#include "objectcache.h"

#include <QDateTime>
#include <QDebug>
#include <QElapsedTimer>
#include <QThread>

ObjectCache::ObjectCache()
{
    startTimer(3000);
}

ObjectCache *ObjectCache::instance()
{
    static ObjectCache *instance = nullptr;
    if (!instance)
        instance = new ObjectCache;
    return instance;
}

void ObjectCache::add(QObject *object, qint64 aliveSecs, Callback callback)
{
    if (_dataList.contains(object->metaObject()->className()))
        return;

    QThread *t = qobject_cast<QThread*>(object);
    if (t)
        connect(t, &QThread::finished, [this, object](){
            remove(object->metaObject()->className());
        });

    connect(object, &QObject::destroyed, [this, object]() {
        remove(object->metaObject()->className());
    });

    auto d = new DataEntry;
    d->obj = object;
    d->timeout = aliveSecs * 1000;
    d->timer = new QElapsedTimer;
    d->timer->start();
    d->callback = callback;

    _dataList.insert(object->metaObject()->className(), d);
}

QObject *ObjectCache::get(const QString &name) const
{
    if (_dataList.contains(name)) {
        _dataList.value(name)->timer->restart();
        return _dataList.value(name)->obj;
    } else {
        return nullptr;
    }
}

bool ObjectCache::remove(const QString &name, const bool &deleteLater)
{
    if (!_dataList.contains(name))
        return false;

    if (deleteLater)
        _dataList.value(name)->obj->deleteLater();
    return _dataList.remove(name) > 0;
}

void ObjectCache::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)

    auto keys = _dataList.keys();
    foreach (QString key, keys) {
        auto d = _dataList.value(key);
        if (d->timer->hasExpired(d->timeout)) {
            d->obj->deleteLater();
            if (d->callback)
                d->callback(d->obj);
            _dataList.remove(key);
            delete d;
            qDebug() << key << "removed from cache";
        }
    }
}

ObjectCache::DataEntry::~DataEntry() {
    delete timer;
}
