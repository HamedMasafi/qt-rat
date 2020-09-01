#ifndef FILEMANAGERDIALOG_H
#define FILEMANAGERDIALOG_H

#include "ui_filemanagerdialog.h"
#include "../core/controllerbase.h"
#include <QStack>

class AbstractFileSystem;
class FileManagerDialog : public ControllerBase, private Ui::FileManagerDialog
{
    Q_OBJECT
    QStringList exts;
    QMenu *fileMenu;
    QMenu *dirMenu;
    QString _clipboardPath;
    QString _clipboardFile;
    AbstractFileSystem *_fs;
    QStack<QString> _backPaths;
    QStack<QString> _forwardPaths;

    enum ActionType {
        None,
        CutFule,
        CopyFile,
        CutDir,
        CopyDir,
    };
    ActionType _lastAction;


    enum Roles {
        IsChecked = Qt::UserRole,
        FullPath,
        Files
    };

public:
    explicit FileManagerDialog(AbstractClient *client, QWidget *parent = nullptr);

protected:
    void changeEvent(QEvent *e);

private:
    void initFiles(const QString &path, const QVariantList &list);
    void initNode(QTreeWidgetItem *item);
    void initRoots();
    QString makePath(const QString &dir, const QString &file) const;
    QString makePath(const QVariant &dir, const QString &file) const;

    QTreeWidgetItem *browseNodeForPath(const QString &path, QTreeWidgetItem *node);
    QTreeWidgetItem *findPathNode(const QString &path);

    void enableActions();
    void goToPath(const QString &path);

private slots:
    void on_treeWidgetClientFiles_itemActivated(QTreeWidgetItem *item, int column);
    void on_listWidget_customContextMenuRequested(const QPoint &pos);
    void on_treeWidgetClientFiles_customContextMenuRequested(const QPoint &pos);
    void on_actionCutFile_triggered();
    void on_actionCopyFile_triggered();
    void on_actionCutDir_triggered();
    void on_actionCopyDir_triggered();
    void on_actionDeleteFile_triggered();
    void on_actionDeleteDir_triggered();
    void on_actionPaste_triggered();
    void on_actionHome_triggered();
    void on_actionBack_triggered();
    void on_actionForward_triggered();
    void on_actionUp_triggered();
    void on_actionRefresh_triggered();
    void on_actionDownload_triggered();
    void on_actionUpload_triggered();
};

#endif // FILEMANAGERDIALOG_H
