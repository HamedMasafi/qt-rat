#ifndef NATIVEUTILS_H
#define NATIVEUTILS_H

#include <QString>

class NativeUtils
{
public:
    NativeUtils();

    static void createTempFile(QString &fileName);
};

#endif // NATIVEUTILS_H
