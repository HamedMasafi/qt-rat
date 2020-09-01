#ifndef FILE_H
#define FILE_H

#include <SharedObject>

class QFile;
class AbstractFileSystem : public Neuron::SharedObject
{
    Q_OBJECT
    QFile *file;
    QMap<int, QByteArray> _buffer;
    int _lastWrittenIndex;

    void flushOldData();
    bool copyRecursively(QString sourceFolder, QString destFolder);

public:
    N_CLASS_DECL(AbstractFileSystem)

    N_REMOTE_METHOD_DECL(bool, openFile, QString, path, int, mode)
    N_REMOTE_METHOD_DECL(bool, createFile, QString, path)
    N_REMOTE_METHOD_DECL(QString, createTempFile)
    N_REMOTE_METHOD_DECL(closeFile)
    N_REMOTE_METHOD_DECL(write, QByteArray, data, int, chunkIndex)
    N_REMOTE_METHOD_DECL(QByteArray, read, int, len)
    N_REMOTE_METHOD_DECL(qint64, fileSize, QString, path)

    N_REMOTE_METHOD_DECL(bool, copyFile, QString, source, QString, destination)
    N_REMOTE_METHOD_DECL(bool, copyDir, QString, source, QString, destination)

    N_REMOTE_METHOD_DECL(bool, moveFile, QString, source, QString, destination)
    N_REMOTE_METHOD_DECL(bool, moveDir, QString, source, QString, destination)

    N_REMOTE_METHOD_DECL(bool, removeFile, QString, source)
    N_REMOTE_METHOD_DECL(bool, removeDir, QString, source)

    N_REMOTE_METHOD_DECL(QVariant, dirEntries, QString, path)

    N_REMOTE_METHOD_DECL(QString, homePath)

};

#endif // FILE_H
