#ifndef VARIANTWRAPPER_H
#define VARIANTWRAPPER_H

#include <QStringList>
#include <QVariantList>
#include <initializer_list>
class VariantWrapper
{
    QVariantList _header;
    QVariantList _data;

public:
    VariantWrapper();

    void addRow(const std::initializer_list<QString> &args);
    void addHeader(const QString &col);
    QStringList header() const;
    void setHeader(const QStringList &header);

    QVariantMap data() const;
    void setData(const QVariantMap &data);
    void setData(const QVariant &data);

    bool isValid() const;
    int rows() const;
    int cols() const;
    QVariant data(int row, int col) const;
    QVariant data(int row, const QString &key) const;
};

#endif // VARIANTWRAPPER_H
