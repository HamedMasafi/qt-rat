#ifndef FILE_H
#define FILE_H

#include <SharedObject>

class QFile;
class File : public Neuron::SharedObject
{
    Q_OBJECT
    QFile *file;
    QMap<int, QByteArray> _buffer;
    int _lastWrittenIndex;

    void flushOldData();

public:
    N_CLASS_DECL(File)

    N_REMOTE_METHOD_DECL(bool, createFile, QString, path)
    N_REMOTE_METHOD_DECL(QString, createTempFile)
    N_REMOTE_METHOD_DECL(closeFile)
    N_REMOTE_METHOD_DECL(write, QByteArray, data, int, chunkIndex)
};

#endif // FILE_H
