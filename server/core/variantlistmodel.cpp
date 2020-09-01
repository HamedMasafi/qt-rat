#include "variantlistmodel.h"
#include <QDebug>

void VariantListModel::setData(const QVariantList &data)
{
    if (_data.count()) {
        beginRemoveRows(QModelIndex(), 0, _data.count() - 1);
        _data.clear();
        endRemoveRows();
    }
    beginInsertRows(QModelIndex(), 0, data.count() - 1);
    _data = data;
    endInsertRows();
}

VariantListModel::VariantListModel(QObject *parent) : QAbstractTableModel(parent)
{

}

void VariantListModel::addHeader(const QString &key, const QString &text)
{
    beginInsertColumns(QModelIndex(), _headers.count(), _headers.count());
    _headers.insert(key, text);
    _headerNames.append(key);
    endInsertColumns();
}

int VariantListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return _data.count();
}

int VariantListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return _headers.count();
}

QVariant VariantListModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (index.column() < 0 || index.row() < 0)
        return QVariant();

    if (index.column() >= _headers.count() || index.row() >= _data.count())
        return QVariant();

    if (_data.at(index.row()).type() != QVariant::Map)
        return QVariant();

    auto map = _data.at(index.row()).toMap();
    return map.value(_headerNames.at(index.column()));
}

QVariant VariantListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        if (section < 0 || section >= _headers.count())
            return QVariant();
        return _headers.value(_headerNames.at(section));
    }

    return section + 1;
}

QVariantMap VariantListModel::map(const int &index) const
{
    if (index < 0 || index >= _data.count())
        return QVariantMap();
    return _data.at(index).toMap();
}

QVariantMap VariantListModel::map(const QModelIndex &index) const
{
    if (!index.isValid())
        return QVariantMap();
    return map(index.row());
}

VariantProxyModel::VariantProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{}

void VariantProxyModel::setFilter(const QString &col, const QVariant &filter)
{
    if (filter == QVariant())
        _filter.remove(col);
    else
        _filter.insert(col, filter);
    invalidate();
}

void VariantProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    auto o = qobject_cast<VariantListModel*>(sourceModel);
    if (!o) {
        qWarning() << "VariantProxyModel::setSourceModel accept only parametere of type VariantListModel";
        return;
    }
    QSortFilterProxyModel::setSourceModel(sourceModel);
    _sourceModel = o;
}

bool VariantProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent);

    if (!_sourceModel)
        return false;

    auto map = _sourceModel->map(source_row);
    for (auto i = _filter.begin(); i != _filter.end(); ++i) {
        if (!map.value(i.key()).toString().contains(i.value().toString()))
            return false;
    }
    return true;
}
