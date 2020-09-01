#include "filetransferdialog.h"

#include "../common/abstractclient.h"
#include "../common/abstractfilesystem.h"
#include "../core/filetransferthread.h"
#include "filemanagerdialog.h"

#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QApplication>
#include <QStyle>
#include <QTemporaryFile>
#include <QThread>

QString FileTransferDialog::localFilePath() const
{
    return lineEditLocalPath->text();
}

void FileTransferDialog::setLocalFilePath(const QString &localFilePath)
{
    lineEditLocalPath->setText(localFilePath);
//    if (localFilePath.isNull()) {
//        lineEditLocalPath->setEnabled(true);
//        toolButtonLocalBrowse->show();
//    } else {
//        lineEditLocalPath->setEnabled(false);
//        toolButtonLocalBrowse->hide();
//    }
}

QString FileTransferDialog::remoteFilePath() const
{
    return lineEditRemotePath->text();
}

void FileTransferDialog::setRemoteFilePath(const QString &remoteFilePath)
{
    lineEditRemotePath->setText(remoteFilePath);
//    if (remoteFilePath.isNull()) {
//        lineEditRemotePath->setEnabled(true);
//        toolButtonRemoteBrowse->show();
//    } else {
//        lineEditRemotePath->setEnabled(false);
//        toolButtonRemoteBrowse->hide();
//    }
}

bool FileTransferDialog::remoteLocalSelection() const
{
    return _remoteLocalSelection;
}

void FileTransferDialog::setEnableRemoteSelection(bool enableRemoteSelection)
{
    lineEditRemotePath->setEnabled(enableRemoteSelection);
    toolButtonRemoteBrowse->setVisible(false);//TODO: enableRemoteSelection);
    _remoteLocalSelection = enableRemoteSelection;
}

bool FileTransferDialog::enableLocalSelection() const
{
    return _enableLocalSelection;
}

void FileTransferDialog::setEnableLocalSelection(bool enableLocalSelection)
{
    lineEditLocalPath->setEnabled(enableLocalSelection);
    toolButtonLocalBrowse->setVisible(enableLocalSelection);
    _enableLocalSelection = enableLocalSelection;
}

//void FileTransferDialog::pull(const QString &localPath, const QString &remotePath)
//{
//    setLocalFilePath(localPath);
//    setRemoteFilePath(remotePath);
//    setType(Pull);
//}

//void FileTransferDialog::push(const QString &localPath, const QString &remotePath)
//{
//    setLocalFilePath(localPath);
//    setRemoteFilePath(remotePath);
//    setType(Push);
//}

TransferType FileTransferDialog::type() const
{
    return _type;
}

void FileTransferDialog::setType(const TransferType &type)
{
    _type = type;
}

FileTransferDialog::FileTransferDialog(AbstractClient *client, QWidget *parent) :
    ControllerBase(client, parent), file(nullptr)
{
    setupUi(this);

    pushButtonClose->setIcon(qApp->style()->standardIcon(QStyle::SP_DockWidgetCloseButton));
    pushButtonStart->setIcon(qApp->style()->standardIcon(QStyle::SP_MediaPlay));

    _thread = new FileTransferThread(client, this);
    connect(_thread, &FileTransferThread::maxChanged, progressBar, &QProgressBar::setMaximum);
    connect(_thread, &FileTransferThread::valueChanged, progressBar, &QProgressBar::setValue);
    connect(_thread, &FileTransferThread::statusChanged, labelStatus, &QLabel::setText);
}

void FileTransferDialog::setRemotePath(const QString &remotePath)
{
    if (remotePath.endsWith("/"))
        _remotePath = remotePath;
    else
        _remotePath = remotePath + "/";
}

QString FileTransferDialog::remotePath() const
{
    return _remotePath;
}

FileTransferDialog::FileTransferDialog(AbstractClient *client,
                                       QString localFile,
                                       QString remoteFile,
                                       QWidget *parent) :
                                                          ControllerBase(client, parent), file(nullptr)
{
    setupUi(this);

    if (localFile.isNull()) {
        auto name = remoteFile.split('/').last();
        lineEditLocalPath->setText(QDir::tempPath() + "/" + name);
    } else {
        lineEditLocalPath->setText(localFile);
        lineEditLocalPath->setEnabled(false);
        toolButtonLocalBrowse->hide();
    }

    if (!remoteFile.isNull()) {
        lineEditRemotePath->setText(remoteFile);
        lineEditRemotePath->setEnabled(false);
        toolButtonRemoteBrowse->hide();
    }

    _thread = new FileTransferThread(client, this);
    connect(_thread, &FileTransferThread::maxChanged, progressBar, &QProgressBar::setMaximum);
    connect(_thread, &FileTransferThread::valueChanged, progressBar, &QProgressBar::setValue);
    connect(_thread, &FileTransferThread::statusChanged, labelStatus, &QLabel::setText);
    connect(_thread, &FileTransferThread::isBusyChanged, pushButtonStart, &QAbstractButton::setDisabled);
}

void FileTransferDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

void FileTransferDialog::on_pushButtonStart_clicked()
{
    if (!client())
        return;

    _thread->setTransferType(_type);
    _thread->setRemoteFilePath(remoteFilePath());
    _thread->setLocalFilePath(localFilePath());
    _thread->transfer();
}

void FileTransferDialog::on_pushButtonClose_clicked()
{
    reject();
    close();
}

void FileTransferDialog::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    if (file) {
        client()->hub()->detachSharedObject(file);
        file->deleteLater();
    }
}

void FileTransferDialog::on_toolButtonRemoteBrowse_clicked()
{
    FileManagerDialog d(client(), this);
    if (d.exec() == QDialog::Accepted) {
        //TODO; make FileManagerDialog path selectable
//        lineEditRemotePath->setText(d.path());
    }
}

void FileTransferDialog::on_toolButtonLocalBrowse_clicked()
{
    QString selectedFile;
    if (_type == Push)
        selectedFile = QFileDialog::getOpenFileName(this, tr("Select file"));
    else
        selectedFile = QFileDialog::getSaveFileName(this, tr("Select file"));

    if (selectedFile != QString()) {
        lineEditLocalPath->setText(selectedFile);
        if (!_remotePath.isNull())
            lineEditRemotePath->setText(_remotePath + selectedFile.split('/').last());
    }
}
