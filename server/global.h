#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>


enum TransferType {
    Pull,
    Push
};

class Global {
public:
    Global();
    static void loadCss(const QString &cssFile);
};

#endif // GLOBAL_H
