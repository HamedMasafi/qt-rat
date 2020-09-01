#ifndef FILETRANSFERDIALOG_H
#define FILETRANSFERDIALOG_H

#include "ui_filetransferdialog.h"
#include "../core/controllerbase.h"
#include "../global.h"

class AbstractFileSystem;
class FileTransferThread;
class FileTransferDialog : public ControllerBase, private Ui::FileTransferDialog
{
    Q_OBJECT

    FileTransferThread *_thread;
    AbstractFileSystem *file;

    bool _enableLocalSelection;
    bool _remoteLocalSelection;
    TransferType _type;

    QString _remotePath;

public:
    explicit FileTransferDialog(AbstractClient *client, QWidget *parent = nullptr);
    explicit FileTransferDialog(AbstractClient *client,
                                QString localFile,
                                QString remoteFile,
                                QWidget *parent = nullptr);

    void setRemotePath(const QString &remotePath);
    void setType(const TransferType &type);
    void setLocalFilePath(const QString &localFilePath);
    void setRemoteFilePath(const QString &remoteFilePath);
    void setEnableRemoteSelection(bool enableRemoteSelection);
    void setEnableLocalSelection(bool enableLocalSelection);

    TransferType type() const;
    QString localFilePath() const;
    QString remoteFilePath() const;
    bool remoteLocalSelection() const;
    bool enableLocalSelection() const;
    QString remotePath() const;

//    void pull(const QString &localPath, const QString &remotePath);
//    void push(const QString &localPath, const QString &remotePath);




protected:
    void changeEvent(QEvent *e);

private slots:
    void on_pushButtonStart_clicked();
    void on_toolButtonLocalBrowse_clicked();
    void on_toolButtonRemoteBrowse_clicked();
    void on_pushButtonClose_clicked();

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // FILETRANSFERDIALOG_H
