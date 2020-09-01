#ifndef VARIANTLISTMODEL_H
#define VARIANTLISTMODEL_H

#include <QSortFilterProxyModel>
#include <QStandardItemModel>

class VariantListModel : public QAbstractTableModel
{
    Q_OBJECT
    QVariantList _data;
    QStringList _headerNames;
    QMap<QString, QString> _headers;

public:
    VariantListModel(QObject *parent = nullptr);

public:
    void addHeader(const QString &key, const QString &text);
    void setData(const QVariantList &data);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariantMap map(const int &index) const;
    QVariantMap map(const QModelIndex &index) const;
};

class VariantProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    QVariantMap _filter;
    VariantListModel *_sourceModel;

public:
    VariantProxyModel(QObject *parent = nullptr);
    void setFilter(const QString &col, const QVariant &filter);
    void setSourceModel(QAbstractItemModel *sourceModel);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
};

#endif // VARIANTLISTMODEL_H
