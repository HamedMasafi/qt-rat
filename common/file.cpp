#include <QTemporaryFile>
#include "file.h"
#include <QDebug>

N_CLASS_IMPL(File)
{
    file = nullptr;
}

N_REMOTE_METHOD_IMPL(File, bool, createFile, QString, path)
{
    _lastWrittenIndex = -1;
    file = new QFile(path);
    return file->open(QIODevice::WriteOnly);
}

N_REMOTE_METHOD_IMPL(File, QString, createTempFile)
{
    _lastWrittenIndex = -1;
    file = new QTemporaryFile;
    if (!file->open(QIODevice::WriteOnly))
        return QString();
    return file->fileName();
}

N_REMOTE_METHOD_IMPL(File, closeFile)
{
    file->close();
}

N_REMOTE_METHOD_IMPL(File, write, QByteArray, data, int, chunkIndex)
{
    if (_lastWrittenIndex + 1 == chunkIndex) {
        file->write(data);
        _lastWrittenIndex++;
    } else {
        qDebug() << "The chunk" << chunkIndex << "is not in order";
        _buffer.insert(chunkIndex, data);
        flushOldData();
    }
}

void File::flushOldData()
{
    if (_buffer.contains(_lastWrittenIndex + 1)) {
        file->write(_buffer.value(_lastWrittenIndex + 1));
        _buffer.remove(_lastWrittenIndex + 1);
        _lastWrittenIndex++;
        flushOldData();
    }
}
