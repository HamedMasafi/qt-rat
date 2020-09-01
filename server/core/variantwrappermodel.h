#ifndef VARIANTWRAPPERMODEL_H
#define VARIANTWRAPPERMODEL_H

#include <QAbstractTableModel>
#include "../common/variantwrapper.h"

class VariantWrapperModel : public QAbstractTableModel
{
    Q_OBJECT
    VariantWrapper _data;
    QMap<QString, QString> _headers;

public:
    VariantWrapperModel(QObject *parent = nullptr);

    void setHeaderText(const QString &key, const QString &text);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void setVariant(const VariantWrapper &data);
    QVariant data(int row, int col) const;
    QVariant data(int row, const QString &key) const;
};

#endif // VARIANTWRAPPERMODEL_H
