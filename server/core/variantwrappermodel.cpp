#include "variantwrappermodel.h"
#include "../common/variantwrapper.h"
#include <QDebug>

VariantWrapperModel::VariantWrapperModel(QObject *parent)
{

}

void VariantWrapperModel::setHeaderText(const QString &key, const QString &text)
{
    _headers.insert(key, text);
}

int VariantWrapperModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return _data.rows();
}

int VariantWrapperModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    if (!_data.isValid())
        return 0;

    return _data.cols();
}

QVariant VariantWrapperModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    return _data.data(index.row(), index.column());
}

QVariant VariantWrapperModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        if (section < 0 || section >= _data.header().count())
            return QVariant();
        auto key = _data.header().value(section);

        if (_headers.contains(key))
            return _headers.value(key);
        return key;
    }

    return section + 1;
}

void VariantWrapperModel::setVariant(const VariantWrapper &data)
{
//    if (_data.rows()) {
//        beginRemoveRows(QModelIndex(), 0, _data.rows() - 1);
//        endRemoveRows();
//    }

    _data = data;
    beginInsertColumns(QModelIndex(), 0, data.cols() - 1);
    endInsertColumns();
    beginInsertRows(QModelIndex(), 0, data.rows() - 1);
    endInsertRows();
}

QVariant VariantWrapperModel::data(int row, int col) const
{
    return _data.data(row, col);
}

QVariant VariantWrapperModel::data(int row, const QString &key) const
{
    return _data.data(row, key);
}
