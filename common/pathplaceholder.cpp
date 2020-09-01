#include "pathplaceholder.h"

#include <QRegularExpression>
#include <QStandardPaths>

PathPlaceholder::PathPlaceholder()
{

}

QStringList PathPlaceholder::placeholders()
{
 return QStringList()
         << "Desktop"
         << "Documents"
         << "Fonts"
         << "Applications"
         << "Music"
         << "Movies"
         << "Pictures"
         << "Temp"
         << "Home"
         << "Data"
         << "Cache"
         << "GenericCache"
         << "GenericData"
         << "Runtime"
         << "Config"
         << "Download"
         << "GenericConfig"
         << "AppData"
         << "AppLocalData"
         << "AppConfig";
}

QString PathPlaceholder::getPath(const QString &pl)
{
    if (pl == "Desktop") return QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    if (pl == "Documents") return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    if (pl == "Fonts") return QStandardPaths::writableLocation(QStandardPaths::FontsLocation);
    if (pl == "Applications") return QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
    if (pl == "Music") return QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
    if (pl == "Movies") return QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
    if (pl == "Pictures") return QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    if (pl == "Temp") return QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    if (pl == "Home") return QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    if (pl == "Data") return QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    if (pl == "Cache") return QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    if (pl == "GenericCache") return QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation);
    if (pl == "GenericData") return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    if (pl == "Runtime") return QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation);
    if (pl == "Config") return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    if (pl == "Download") return QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    if (pl == "GenericConfig") return QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
    if (pl == "AppData") return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (pl == "AppLocalData") return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    if (pl == "AppConfig") return QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    return QString();
}

QString PathPlaceholder::validateLocation(const QString &path)
{
    QString ret(path);
    QRegularExpression r("\\$(\\w+)\\$");
    auto i = r.globalMatch(path);
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        ret = ret.replace(match.captured(), getPath(match.captured(1)));
    }
    return ret;
}


