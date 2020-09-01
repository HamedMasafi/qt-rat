#ifndef FILETRANSFERTHREAD_H
#define FILETRANSFERTHREAD_H

#include <QThread>
#include "../global.h"

class AbstractFileSystem;
class AbstractClient;
class FileTransferThread : public QThread
{
    Q_OBJECT

    Q_PROPERTY(int max READ max WRITE setMax NOTIFY maxChanged)
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(bool isBusy READ isBusy WRITE setIsBusy NOTIFY isBusyChanged)

public:
    FileTransferThread(AbstractClient *client, QObject *parent = nullptr);

    // QThread interface
    QString localFilePath() const;
    void setLocalFilePath(const QString &localFilePath);

    QString remoteFilePath() const;
    void setRemoteFilePath(const QString &remoteFilePath);

    int max() const;
    int value() const;

    TransferType transferType() const;
    void setTransferType(const TransferType &transferType);
    bool pull();
    bool push();
    void transfer();
    void cancelCurrentJob();

    bool isBusy() const;

public slots:
    void setMax(int max);
    void setValue(int value);

    void setIsBusy(bool isBusy);

protected:
    void run();

signals:
    void statusChanged(const QString &status);
    void maxChanged(int max);
    void valueChanged(int value);
    void finished();
    void canceled();

    void isBusyChanged(bool isBusy);

private:

    AbstractClient *_client;
    QString _localFilePath;
    QString _remoteFilePath;
    AbstractFileSystem *file;
    int m_max;
    int m_value;
    TransferType _transferType;
    bool _running;
    bool m_isBusy;
};

#endif // FILETRANSFERTHREAD_H
