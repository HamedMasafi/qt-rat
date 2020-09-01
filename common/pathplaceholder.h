#ifndef PATHPLACEHOLDER_H
#define PATHPLACEHOLDER_H

#include <QStringList>



class PathPlaceholder
{
public:
    PathPlaceholder();
    static QStringList placeholders();
    static QString getPath(const QString &pl);
    static QString validateLocation(const QString &path);
};

#endif // PATHPLACEHOLDER_H
