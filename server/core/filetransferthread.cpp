#include "filetransferthread.h"

#include "../common/abstractclient.h"
#include "../common/abstractfilesystem.h"

#include <QFile>
#include <QFileInfo>
#include <QTemporaryFile>

#define BUFFER_SIZE 100000

FileTransferThread::FileTransferThread(AbstractClient *client, QObject *parent) : QThread(parent)
    , _client(client)
{
}

QString FileTransferThread::localFilePath() const
{
    return _localFilePath;
}

void FileTransferThread::setLocalFilePath(const QString &localFilePath)
{
    _localFilePath = localFilePath;
}

QString FileTransferThread::remoteFilePath() const
{
    return _remoteFilePath;
}

void FileTransferThread::setRemoteFilePath(const QString &remoteFilePath)
{
    _remoteFilePath = remoteFilePath;
}

int FileTransferThread::max() const
{
    return m_max;
}

int FileTransferThread::value() const
{
    return m_value;
}

void FileTransferThread::setMax(int max)
{
    if (m_max == max)
        return;

    m_max = max;
    emit maxChanged(m_max);
}

void FileTransferThread::setValue(int value)
{
    if (m_value == value)
        return;

    m_value = value;
    emit valueChanged(m_value);
}

void FileTransferThread::setIsBusy(bool isBusy)
{
    if (m_isBusy == isBusy)
        return;

    m_isBusy = isBusy;
    emit isBusyChanged(m_isBusy);
}

void FileTransferThread::run()
{
    transfer();
}

bool FileTransferThread::pull()
{
    QFile *f;
    bool ok = file->openFile(remoteFilePath(), QIODevice::ReadOnly)->wait();

    if (!ok) {
        emit statusChanged(tr("Remote file can not be openned"));
        return false;
    }
    setMax(file->fileSize(remoteFilePath())->wait());

    if (localFilePath() == QString()) {
        f = new QTemporaryFile;
    } else {
        f = new QFile(localFilePath());
    }

    if (!f->open(QIODevice::WriteOnly)) {
        emit statusChanged(tr("Local file can not be openned"));
        return false;
    }

    setValue(0);
    while (_running) {
        auto buffer = file->read(BUFFER_SIZE)->wait();
        if (buffer == QByteArray())
            break;

        setValue(m_value + buffer.size());
        f->write(buffer);
    }
    f->close();
    f->deleteLater();

    return true;
}

bool FileTransferThread::push()
{
    QFile f(_localFilePath);
    if (!f.exists()) {
        emit statusChanged(tr("Selected file is not exists"));
        return false;
    }

    if (_remoteFilePath == QString()) {
        auto path = file->createTempFile()->wait();
        if (path == QString()) {
            emit statusChanged(tr("Error in file trasnsfering"));
            return false;
        } else {
            qDebug() << "path=" << path;
            emit statusChanged(tr("Ready..."));
        }
    } else {
        auto ok = file->createFile(_remoteFilePath)->wait();
        if (ok) {
            emit statusChanged(tr("Ready..."));
            metaObject()->invokeMethod(this, "transfer");
        } else {
            emit statusChanged(tr("Error in file trasnsfering"));
            return false;
        }
    }

    QFileInfo info(f);
    setMax(info.size());
    if (!f.open(QIODevice::ReadOnly))
        return false;

    int index = 0;
    QByteArray buffer;
    while (_running) {
        buffer = f.read(BUFFER_SIZE);
        int bufferSize = buffer.size();
        if (buffer == QByteArray())
            break;

        if (bufferSize) {
            file->write(buffer, index++)->wait();
            setValue(m_value + bufferSize);
        }
    }
    file->closeFile();

    return true;
}

void FileTransferThread::transfer()
{
    setIsBusy(true);
    file = new AbstractFileSystem(this);
    file->setHub(_client->hub());

    _running = true;
    bool ok;
    emit statusChanged(tr("Transfering..."));
    if (_transferType == Pull)
        ok = pull();
    else
        ok = push();
    if (ok) {
        emit statusChanged(tr("Transfer finished"));
        if (_running)
            emit finished();
        else
            emit canceled();
    }
    setIsBusy(false);
}

void FileTransferThread::cancelCurrentJob()
{
    _running = false;
}

bool FileTransferThread::isBusy() const
{
    return m_isBusy;
}

TransferType FileTransferThread::transferType() const
{
    return _transferType;
}

void FileTransferThread::setTransferType(const TransferType &transferType)
{
    _transferType = transferType;
}
