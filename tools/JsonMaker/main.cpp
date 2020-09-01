#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include <QJsonArray>
#include <QProcess>
#include <QJsonObject>
#include <QJsonDocument>

#ifdef Q_OS_LINUX
#define PATH "/home/hamed/share/rat_files/repo/"
#define OUT "/home/hamed/share/rat_files/out/"
#endif
#ifdef Q_OS_WIN
#define PATH "C:/rat/client/"
#define OUT "C:/rat/client-out/"
#endif

void browseDir(const QString &path, QStringList &files)
{
    QDir d(path);
    auto dirs = d.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    foreach(auto dir, dirs)
        browseDir(dir.absoluteFilePath(), files);

    auto filesList = d.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    foreach (auto fi, filesList) {
        files.append(fi.absoluteFilePath());
    }
}

#ifdef Q_OS_LINUX
bool zipFile(const QString &platform, const QString &arch, const QString &filePath, const QString &outPath,
                QString &zipFileName, qint64 &fileSize)
{
    auto f = filePath;
    zipFileName = f
            .replace(PATH + platform + "/" + arch + "/", "")
            .replace("/", "_")
            .replace(".", "_") + ".zip";
    QProcess p;
    p.setProgram("zip");
    QFileInfo fi(filePath);
    p.setArguments(QStringList() << outPath + "/" + zipFileName << fi.fileName());
    p.setWorkingDirectory(QFileInfo(filePath).absolutePath());
    p.start();
    p.waitForFinished();
    if (p.exitCode()) {
        qDebug() << p.readAllStandardError();
        fileSize = 0;
        return false;
    }

    QFileInfo fis(outPath + "/" + zipFileName);
    fileSize = fis.size();

    return true;
}
#endif
#ifdef Q_OS_WIN
bool zipFile(const QString &platform, const QString &arch, const QString &filePath, const QString &outPath,
                QString &zipFileName, qint64 &fileSize)
{
    auto f = filePath;
    zipFileName = f
            .replace(PATH + platform + "/" + arch + "/", "")
            .replace("/", "_")
            .replace(".", "_") + ".zip";
    zipFileName = outPath + "/" + zipFileName;
    zipFileName = zipFileName.replace("/", "\\");

    QProcess p;
    p.setProgram("C:/Program Files/7-Zip/7z.exe");
    QFileInfo fi(filePath);
//    d archive.zip *.bak -r
    p.setArguments(QStringList() << "a" << "-mx9" << "-tzip" << zipFileName << fi.fileName());
    p.setWorkingDirectory(QFileInfo(filePath).absolutePath());
    p.start();
    p.waitForFinished();
    if (p.exitCode()) {
        qDebug() << p.readAllStandardError();
        fileSize = 0;
        return false;
    }

    QFileInfo fis(zipFileName);
    fileSize = fis.size();

    return true;
}
#endif

QJsonArray procDir(const QString &platform, const QString &arch, qint64 &totalSize)
{
    QString outPath = OUT + platform + "/" + arch;
    QDir out;
    out.mkpath(outPath);
    QStringList files;
    browseDir(PATH + platform + "/" + arch, files);

    totalSize = 0;
    QJsonArray arr;
    int c = 0;
    foreach (auto file, files) {
        QString zipFileName;
        qint64 fileSize;
        zipFile(platform, arch, file, outPath, zipFileName, fileSize);
        totalSize += fileSize;
        auto outFile = QString(file)
                .replace(PATH + platform + "/" + arch + "/", "");
        QJsonObject o{
            {"file", zipFileName},
            {"out", outFile},
            {"size", fileSize}
        };
        arr.append(o);
        qDebug() << (++c) << "/" << files.count();
    }
    return arr;

}
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QJsonArray arr;
    QDir path(PATH);
    auto platforms = path.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    qDebug() << platforms;
    foreach (auto platform, platforms) {
        QDir platformDir(PATH + platform);
        auto archs = platformDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        foreach (auto arch, archs) {
            qint64 size;
            auto a = procDir(platform, arch, size);
            QJsonObject o{
                {"platform", platform},
                {"arch", arch},
                {"total_size", size},
                {"files", a}
            };
            arr.append(o);
        }
    }

    QJsonDocument doc(arr);
    QFile jsonFile(QString(OUT) + "/data.json");
    jsonFile.open(QIODevice::WriteOnly);
    jsonFile.write(doc.toJson(QJsonDocument::Indented));
    jsonFile.close();

    qDebug() << "Finished";
    return a.exec();
}
