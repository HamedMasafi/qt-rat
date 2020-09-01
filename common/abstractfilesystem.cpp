#include <QTemporaryFile>
#include "abstractfilesystem.h"
#include <QDebug>
#include <QDir>

N_CLASS_IMPL(AbstractFileSystem)
{
    file = nullptr;
}

N_REMOTE_METHOD_IMPL(AbstractFileSystem, bool, openFile, QString, path, int, mode)
{
    _lastWrittenIndex = -1;
    file = new QFile(path);
    bool ok = file->open(static_cast<QIODevice::OpenMode>(mode));
    if (!ok) {
        qDebug() << "UNable to open file" << path <<"in"<< mode<<"mode";
    }
    return ok;
}

N_REMOTE_METHOD_IMPL(AbstractFileSystem, bool, createFile, QString, path)
{
    _lastWrittenIndex = -1;
    file = new QFile(path);
    return file->open(QIODevice::WriteOnly);
}

N_REMOTE_METHOD_IMPL(AbstractFileSystem, QString, createTempFile)
{
    _lastWrittenIndex = -1;
    file = new QTemporaryFile;
    if (!file->open(QIODevice::WriteOnly))
        return QString();
    return file->fileName();
}

N_REMOTE_METHOD_IMPL(AbstractFileSystem, closeFile)
{
    file->close();
}

N_REMOTE_METHOD_IMPL(AbstractFileSystem, write, QByteArray, data, int, chunkIndex)
{
//    if (_lastWrittenIndex + 1 == chunkIndex) {
        file->write(data);
//        _lastWrittenIndex++;
//    } else {
//        qDebug() << "The chunk" << chunkIndex << "is not in order";
//        _buffer.insert(chunkIndex, data);
//        flushOldData();
//    }
}

N_REMOTE_METHOD_IMPL(AbstractFileSystem, QByteArray, read, int, len)
{
    return file->read(len);
}

N_REMOTE_METHOD_IMPL(AbstractFileSystem, qint64, fileSize, QString, path)
{
    if (!QFile::exists(path))
        return -1;
    QFileInfo fi(path);
    return fi.size();
}

N_REMOTE_METHOD_IMPL(AbstractFileSystem, bool, copyFile, QString, source, QString, destination)
{
    auto b = QFile::copy(source, destination);
    if (!b)
        qDebug() << "copy from " << source << "to" << destination << "faild";
    return b;
}

bool AbstractFileSystem::copyRecursively(QString sourceFolder, QString destFolder)
{
    bool success = false;
    QDir sourceDir(sourceFolder);

    if(!sourceDir.exists())
        return false;

    QDir destDir(destFolder);
    if(!destDir.exists())
        destDir.mkdir(destFolder);

    QStringList files = sourceDir.entryList(QDir::Files);
    for(int i = 0; i< files.count(); i++) {
        QString srcName = sourceFolder + QDir::separator() + files[i];
        QString destName = destFolder + QDir::separator() + files[i];
        success = QFile::copy(srcName, destName);
        if(!success)
            return false;
    }

    files.clear();
    files = sourceDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    for(int i = 0; i< files.count(); i++)
    {
        QString srcName = sourceFolder + QDir::separator() + files[i];
        QString destName = destFolder + QDir::separator() + files[i];
        success = copyRecursively(srcName, destName);
        if(!success)
            return false;
    }

    return true;
}

N_REMOTE_METHOD_IMPL(AbstractFileSystem, bool, copyDir, QString, source, QString, destination)
{
    return copyRecursively(source, destination);
}
void AbstractFileSystem::flushOldData()
{
    if (_buffer.contains(_lastWrittenIndex + 1)) {
        file->write(_buffer.value(_lastWrittenIndex + 1));
        _buffer.remove(_lastWrittenIndex + 1);
        _lastWrittenIndex++;
        flushOldData();
    }
}

N_REMOTE_METHOD_IMPL(AbstractFileSystem, bool, moveFile, QString, source, QString, destination)
{
    if (!copyFileSlot(source, destination))
        return false;
    removeFile(source);
}
N_REMOTE_METHOD_IMPL(AbstractFileSystem, bool, moveDir, QString, source, QString, destination)
{
    if (!copyDirSlot(source, destination))
        return false;
    removeDir(source);
}

N_REMOTE_METHOD_IMPL(AbstractFileSystem, bool, removeFile, QString, source)
{
    return QFile::remove(source);
}

N_REMOTE_METHOD_IMPL(AbstractFileSystem, bool, removeDir, QString, source)
{
    return QDir(source).removeRecursively();
}

N_REMOTE_METHOD_IMPL(AbstractFileSystem, QVariant, dirEntries, QString, path)
{
    QVariantMap map;

    if (path.isEmpty()) {
        QStringList dirsList;
        QFileInfoList dirs = QDir::drives();
        foreach (QFileInfo di, dirs)
            dirsList.append(di.path());
        map["dirs"] = dirsList;
    } else {
        QDir d(path);
        map["files"] = d.entryList(QDir::Files);
        map["dirs"] = d.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    }

    return map;
}

N_REMOTE_METHOD_IMPL(AbstractFileSystem, QString, homePath)
{
    return QDir::homePath();
}
