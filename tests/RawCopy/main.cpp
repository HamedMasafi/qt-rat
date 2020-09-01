#include <QCoreApplication>
#include <QString>
#include "rawcopy.h"

int main(int argc, char *argv[])
{
    std::wstring src = L"C:\\Users\\Administrator\\AppData\\Local\\Google\\Chrome\\User Data\\Default\\Current Tabs";
    std::wstring dst = L"C:\\Users\\Administrator\\Desktop\\tabs.txt";
    QCoreApplication a(argc, argv);

    RawCopy(src, dst);

    return a.exec();
}
