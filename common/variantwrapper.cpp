#include "variantwrapper.h"

#include <QVariantList>
#include <QDebug>

QStringList VariantWrapper::header() const
{
    QStringList h;
    foreach (QVariant c, _header)
        h.append(c.toString());
    return h;
}

void VariantWrapper::setHeader(const QStringList &header)
{
    _header.clear();
    foreach (QString c, header)
        _header.append(c);
}

QVariantMap VariantWrapper::data() const
{
    QVariantMap m;
    m.insert("data", _data);
    m.insert("header", _header);
    return m;
}

void VariantWrapper::setData(const QVariantMap &data)
{
    _data = data.value("data").toList();
    _header = data.value("header").toList();
}

void VariantWrapper::setData(const QVariant &data)
{
    if (data.type() == QVariant::Map)
        setData(data.toMap());
}

bool VariantWrapper::isValid() const
{
    return !_data.isEmpty() && !_header.isEmpty();
}

int VariantWrapper::rows() const
{
    if (!_data.size() || !_header.size())
        return 0;

    return _data.size() / _header.size();
}

int VariantWrapper::cols() const
{
    return _header.size();
}

QVariant VariantWrapper::data(int row, int col) const
{
    int index = (row * cols()) + col;
    if (index < _data.size() && index > 0)
        return _data.value(index);

    qDebug() << "invalid pos" << row << col << index << rows();
    return QVariant();
}

QVariant VariantWrapper::data(int row, const QString &key) const
{
    if (!_header.contains(key))
        return QVariant();
    auto c = _header.indexOf(key);
    return data(row, c);
}

VariantWrapper::VariantWrapper()
{

}

void VariantWrapper::addRow(const std::initializer_list<QString> &args)
{
    if (args.size() != _header.size()) {
        qDebug() << "Size of row is invalid";
        return;
    }

    QVariantList l;
    for (auto i = args.begin(); i != args.end(); ++i)
        l.append(*i);
    _data.append(l);
}

void VariantWrapper::addHeader(const QString &col)
{
    _header.append(col);
}
