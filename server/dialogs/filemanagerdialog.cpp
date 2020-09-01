#include "filemanagerdialog.h"
#include "filetransferdialog.h"

#include <QTreeWidgetItem>
#include "../common/abstractclient.h"
#include "../common/abstractfilesystem.h"
#include <QDebug>
#include <QMenu>
#include <QMessageBox>
#include <QToolBar>

FileManagerDialog::FileManagerDialog(AbstractClient *client, QWidget *parent) :
    ControllerBase(client, parent), _lastAction(None)
{
    setupUi(this);
    exts = QStringList() << "aac" <<   "avi" <<     "c" <<    "csv" <<  "doc" <<   "dxf" <<  "flv" <<  "hpp" <<   "iso" <<   "js" <<    "mid" <<  "mpg" <<  "odt" <<  "ott" <<    "php" <<  "psd" <<  "rar" <<  "sass" <<  "tga" <<   "txt" <<  "xlsx" <<  "zip" <<
           "aiff" <<  "_blank" <<  "cpp" <<  "dat" <<  "dotx" <<  "eps" <<  "gif" <<  "html" <<  "java" <<  "key" <<   "mp3" <<  "odf" <<  "otp" <<  "_page" <<  "png" <<  "py" <<   "rb" <<   "scss" <<  "tgz" <<   "wav" <<  "xml" <<
           "ai" <<    "bmp" <<     "css" <<  "dmg" <<  "dwg" <<   "exe" <<  "h" <<    "ics" <<   "jpg" <<   "less" <<  "mp4" <<  "ods" <<  "ots" <<  "pdf" <<    "ppt" <<  "qt" <<   "rtf" <<  "sql" <<   "tiff" <<  "xls" <<  "yml";


    fileMenu = new QMenu(this);
    fileMenu->addAction(actionCutFile);
    fileMenu->addAction(actionCopyFile);
    fileMenu->addSeparator();
    fileMenu->addAction(actionDeleteFile);
    fileMenu->addSeparator();
    fileMenu->addAction(actionDownload);

    dirMenu = new QMenu(this);
    dirMenu->addAction(actionCutDir);
    dirMenu->addAction(actionCopyDir);
    dirMenu->addAction(actionPaste);
    dirMenu->addSeparator();
    dirMenu->addAction(actionDeleteDir);
    dirMenu->addSeparator();
    dirMenu->addAction(actionUpload);

    /*
    auto toolbar = new QToolBar(tr("Main toolbar"), this);
    toolbar->addAction(actionBack);
    toolbar->addAction(actionForward);
    toolbar->addAction(actionUp);
    toolbar->addSeparator();
    toolbar->addAction(actionHome);
    toolbar->addSeparator();
    toolbar->addAction(actionRefresh);
    widgetToolbarContainer->layout()->addWidget(toolbar);
*/
    _fs = new AbstractFileSystem(this);
    _fs->setHub(client->hub());

    initRoots();
    enableActions();
}

void FileManagerDialog::changeEvent(QEvent *e)
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

void FileManagerDialog::on_treeWidgetClientFiles_itemActivated(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)
    initNode(item);
    lineEditPath->setText(item->data(0, FullPath).toString());
    _forwardPaths.clear();
    _backPaths.push(lineEditPath->text());
}

void FileManagerDialog::on_listWidget_customContextMenuRequested(const QPoint &pos)
{
    if (listWidget->currentRow() >= 0)
        fileMenu->popup(listWidget->mapToGlobal(pos));
}

void FileManagerDialog::on_treeWidgetClientFiles_customContextMenuRequested(const QPoint &pos)
{
    if (treeWidgetClientFiles->currentIndex().isValid())
        dirMenu->popup(treeWidgetClientFiles->mapToGlobal(pos));
}

void FileManagerDialog::on_actionCutFile_triggered()
{
    if (!listWidget->currentIndex().isValid())
        return;

    _clipboardPath = treeWidgetClientFiles->currentItem()->data(0, FullPath).toString();
    _clipboardFile = listWidget->currentItem()->text();
    _lastAction = CutFule;
}

void FileManagerDialog::on_actionCopyFile_triggered()
{
    if (!listWidget->currentIndex().isValid())
        return;

    _clipboardPath = treeWidgetClientFiles->currentItem()->data(0, FullPath).toString();
    _clipboardFile = listWidget->currentItem()->text();
    _lastAction = CopyFile;
}

void FileManagerDialog::on_actionCutDir_triggered()
{
    if (!treeWidgetClientFiles->currentIndex().isValid())
        return;

    _clipboardPath = treeWidgetClientFiles->currentItem()->data(0, FullPath).toString();
    _lastAction = CutDir;
}

void FileManagerDialog::on_actionCopyDir_triggered()
{
    if (!treeWidgetClientFiles->currentIndex().isValid())
        return;

    _clipboardPath = treeWidgetClientFiles->currentItem()->data(0, FullPath).toString();
    _lastAction = CopyDir;
}

void FileManagerDialog::on_actionDeleteFile_triggered()
{
    if (!treeWidgetClientFiles->currentIndex().isValid())
        return;

    _fs->removeFile(listWidget->currentItem()->data(FullPath).toString());
}

void FileManagerDialog::on_actionDeleteDir_triggered()
{
    if (!treeWidgetClientFiles->currentIndex().isValid())
        return;

    _fs->removeDir(treeWidgetClientFiles->currentItem()->data(0, FullPath).toString());
}

void FileManagerDialog::on_actionPaste_triggered()
{
    if (!_clipboardPath.endsWith("/"))
        _clipboardPath.append("/");

    auto la = _lastAction;
    auto cb = [this, la](bool ok) {
        QString action;
        switch (la) {
        case None:
            break;
        case CutFule:
            action = "Cut file";
            break;
        case CopyFile:
            action = "Copy file";
            break;
        case CutDir:
            action = "Cut dir";
            break;
        case CopyDir:
            action = "Copy dir";
            break;
        }
        QString text;
        if (ok)
            text = QString(tr("%1 finished successfully")).arg(action);
        else
            text = QString(tr("%1 finished with error")).arg(action);
        QMessageBox::information(this, action, text);
    };
    switch (_lastAction) {
    case None:
        break;
    case CutFule:
        _fs->moveFile(makePath(_clipboardPath, _clipboardFile),
                      makePath(treeWidgetClientFiles->currentItem()->data(0, FullPath), _clipboardFile))
            ->then(cb);
        break;
    case CopyFile:
        _fs->copyFile(makePath(_clipboardPath, _clipboardFile),
                      makePath(treeWidgetClientFiles->currentItem()->data(0, FullPath), _clipboardFile))
            ->then(cb);
        break;
    case CutDir:
        _fs->moveDir(_clipboardPath,
                     treeWidgetClientFiles->currentItem()->data(0, FullPath).toString())
            ->then(cb);
        break;
    case CopyDir:
        _fs->copyDir(_clipboardPath,
                     treeWidgetClientFiles->currentItem()->data(0, FullPath).toString())
            ->then(cb);
        break;
    }
    _lastAction = None;
}

void FileManagerDialog::initFiles(const QString &path, const QVariantList &list)
{
    QString p(path);
    if (!p.endsWith("/"))
        p.append("/");

    listWidget->clear();
    foreach (QVariant f, list) {
        auto fn = f.toString();
        QString ext;
        if (fn.contains("."))
            ext = fn.mid(fn.lastIndexOf(".") + 1);

        auto item = new QListWidgetItem;
        item->setText(fn);

        if (exts.contains(ext))
            item->setIcon(QIcon(":/icons/exts/" + ext + ".png"));
        else
            item->setIcon(QIcon(":/icons/exts/_blank.png"));

        item->setData(FullPath, p + fn);
        listWidget->addItem(item);
    }
}

void FileManagerDialog::initNode(QTreeWidgetItem *item)
{
    QString parentPath = item->data(0, FullPath).toString();
    if (!parentPath.endsWith("/"))
        parentPath.append("/");

    if (item->data(0, IsChecked).toBool()) {
        initFiles(parentPath, item->data(0, Files).toList());

        return;
    }


    QString oldText = item->text(0);
    item->setText(0, oldText + " (Loading...)");

    _fs->dirEntries(parentPath)->then([this, item, oldText, parentPath](QVariant v){
        item->setText(0, oldText);
        auto map = v.toMap();
        auto dirs = map.value("dirs").toList();
        auto files = map.value("files").toList();
        item->setData(0, Files, files);

        initFiles(parentPath, files);

        foreach (QVariant dir, dirs) {
            auto i = new QTreeWidgetItem;
            i->setText(0, dir.toString());
            i->setIcon(0, QIcon(":/icons/folder"));
            i->setData(0, FullPath, parentPath + dir.toString());
            item->addChild(i);
        }
    });

    item->setData(0, IsChecked, true);
}

void FileManagerDialog::initRoots()
{
    _fs->hub()->commit();
    _fs->dirEntries("")->then([this](QVariant v){
        auto map = v.toMap();
        auto dirs = map.value("dirs").toList();

        foreach (QVariant dir, dirs) {
            auto i = new QTreeWidgetItem;
            i->setText(0, dir.toString());
            i->setIcon(0, QIcon(":/icons/folder"));
            i->setData(0, FullPath, dir.toString());
            treeWidgetClientFiles->addTopLevelItem(i);
        }
    });
}

QString FileManagerDialog::makePath(const QString &dir, const QString &file) const
{
    if (dir.endsWith("/"))
        return dir + file;

    return dir + "/" + file;
}

QString FileManagerDialog::makePath(const QVariant &dir, const QString &file) const
{
    return makePath(dir.toString(), file);
}

QTreeWidgetItem *FileManagerDialog::browseNodeForPath(const QString &path, QTreeWidgetItem *node)
{
    qDebug() << node->data(0, FullPath).toString() <<"=="<< path;
    if (node->data(0, FullPath).toString() == path)
        return node;

    for (int i = 0; i < node->childCount(); i++) {
        auto t = browseNodeForPath(path, node->child(i));
        if (t)
            return t;
    }
    return nullptr;
}

QTreeWidgetItem *FileManagerDialog::findPathNode(const QString &path)
{
    QString p(path);
//    if (!p.endsWith("/"))
//        p.append("/");

    for (int i = 0; i < treeWidgetClientFiles->topLevelItemCount(); ++i) {
        auto t = browseNodeForPath(p, treeWidgetClientFiles->topLevelItem(i));
        if (t)
            return t;
    }
    return nullptr;
}

void FileManagerDialog::enableActions()
{
    actionBack->setEnabled(_backPaths.size());
    actionForward->setEnabled(_forwardPaths.size());
    actionUp->setEnabled(treeWidgetClientFiles->currentItem() && treeWidgetClientFiles->currentItem()->parent());
}

void FileManagerDialog::goToPath(const QString &path)
{
    auto node = findPathNode(path);
    if (!node)
        return;
    treeWidgetClientFiles->setCurrentItem(node);
    on_treeWidgetClientFiles_itemActivated(node, 0);
}

void FileManagerDialog::on_actionHome_triggered()
{
    _fs->homePath()->then([this](QString home){
        goToPath(home);
    });
}

void FileManagerDialog::on_actionBack_triggered()
{
    if (!_backPaths.size())
        return;
    auto p = _backPaths.pop();
    _forwardPaths.push(p);
    goToPath(p);
    enableActions();
}

void FileManagerDialog::on_actionForward_triggered()
{
    if (!_forwardPaths.size())
        return;
    auto p = _forwardPaths.pop();
    _backPaths.push(p);
    goToPath(p);
    enableActions();
}

void FileManagerDialog::on_actionUp_triggered()
{
    auto item = treeWidgetClientFiles->currentItem();
    if (!item && !item->parent())
        return;
    treeWidgetClientFiles->setCurrentItem(item->parent());
}

void FileManagerDialog::on_actionRefresh_triggered()
{
    auto item = treeWidgetClientFiles->currentItem();
    if (!item)
        return;

    item->setData(0, IsChecked, false);
    initNode(item);
}

void FileManagerDialog::on_actionDownload_triggered()
{
    FileTransferDialog d(client(), this);
    d.setRemoteFilePath(listWidget->currentItem()->data(FullPath).toString());
    d.setLocalFilePath(QString());
    d.setEnableRemoteSelection(false);
    d.setEnableLocalSelection(true);
    d.setType(Pull);
    d.exec();
}

void FileManagerDialog::on_actionUpload_triggered()
{
    FileTransferDialog d(client(), this);
    d.setRemotePath(treeWidgetClientFiles->currentItem()->data(0, FullPath).toString());
    d.setEnableLocalSelection(true);
    d.setEnableRemoteSelection(false);
    d.setType(Push);
    d.exec();
}
