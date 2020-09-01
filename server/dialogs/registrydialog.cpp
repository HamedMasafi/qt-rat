#include "registrydialog.h"
#include "filetransferdialog.h"

#include <QTreeWidgetItem>
#include "../common/abstractclient.h"
#include "../common/abstractfilesystem.h"
#include "../common/abstractregistry.h"
#include "../ui/registryvalueeditordialog.h"

#include <QDebug>
#include <QKeyEvent>
#include <QMenu>
#include <QMessageBox>
#include <QMetaMethod>
#include <QToolBar>

RegistryDialog::RegistryDialog(AbstractClient *client, QWidget *parent) :
    ControllerBase(client, parent)
{
    setupUi(this);

    keyMenu = new QMenu(this);

    valueMenu = new QMenu(this);

    _reg = new AbstractRegistry(this);
    _reg->setHub(client->hub());
    _reg->hub()->commit();

    initRoots();

    treeWidgetValues->installEventFilter(this);
}

void RegistryDialog::changeEvent(QEvent *e)
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

void RegistryDialog::on_treeWidgetRegistry_itemActivated(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)
    initNode(item);
}

void RegistryDialog::on_treeWidgetRegistry_customContextMenuRequested(const QPoint &pos)
{
    if (treeWidgetRegistry->currentIndex().isValid())
        keyMenu->popup(treeWidgetRegistry->mapToGlobal(pos));
}

void RegistryDialog::initFiles(const QString &path, const QVariantList &list)
{
    treeWidgetValues->clear();
    foreach (QVariant f, list) {
        auto map = f.toMap();

        auto item = new QTreeWidgetItem;
        auto name = map.value("name").toString();
        if (name.isEmpty())
            name = "(Default)";
        item->setText(0, name);
        item->setText(1, map.value("type").toString());
        item->setText(2, map.value("value").toString());
        treeWidgetValues->addTopLevelItem(item);
    }
}

void RegistryDialog::initNode(QTreeWidgetItem *item)
{
    QString parentPath = item->data(0, FullPath).toString();
    auto hkey = item->data(0, RootHiveKey).toInt();

    if (item->data(0, IsChecked).toBool()) {
        QVariant values = item->data(0, Values);
        if (values.type() == QVariant::List)
            initFiles(parentPath, values.toList());
        else {
            _reg->values(item->data(0, RootHiveKey).toInt(), item->data(0, FullPath).toString())
                    ->then([this, parentPath](QVariantList list){
                initFiles(parentPath, list);
            });
        }

        return;
    }


    QString oldText = item->text(0);
    item->setText(0, oldText + " (Loading...)");
qDebug() << "getting  " << parentPath;

    _reg->openKey(hkey, parentPath)->then([this, hkey, item, oldText, parentPath](QVariantMap v){
        this->initFiles("", v.value("values").toList());
        item->setData(0, Values, v.value("values"));
        item->setText(0, oldText);
        qDebug() << v;
        auto keys = v.value("keys").toList();
        foreach (QVariant k, keys) {
            auto i = new QTreeWidgetItem;
            i->setText(0, k.toString());
            i->setIcon(0, QIcon(":/icons/folder"));
            if (parentPath.isEmpty())
                i->setData(0, FullPath, k.toString());
            else
                i->setData(0, FullPath, parentPath + "\\" + k.toString());
            i->setData(0, RootHiveKey, hkey);
            item->addChild(i);
        }
        item->setExpanded(true);
    });

    item->setData(0, IsChecked, true);
}

void RegistryDialog::initRoots()
{
    QTreeWidgetItem *computer = new QTreeWidgetItem;
    computer->setText(0, "Computer");
    computer->setData(0, Type, Computer);
    treeWidgetRegistry->addTopLevelItem(computer);

    auto hiveKeysEnum = QMetaEnum::fromType<AbstractRegistry::HiveKeys>();
    for (int i = 0; i < hiveKeysEnum.keyCount(); ++i) {
        int v = hiveKeysEnum.value(i);
        auto item = new QTreeWidgetItem;
        item->setText(0, "HKEY_" + QString(hiveKeysEnum.valueToKey(v)));
        item->setData(0, Type, HiveKey);
        item->setData(0, RootHiveKey, v);
        item->setIcon(0, QIcon(":/icons/folder"));
        computer->addChild(item);
    }
    computer->setExpanded(true);
}

QString RegistryDialog::makePath(const QString &dir, const QString &file) const
{
    if (dir.isEmpty())
        return file;

    if (dir.endsWith("\\"))
        return dir + file;

    return dir + "\\" + file;
}

QString RegistryDialog::makePath(const QVariant &dir, const QString &file) const
{
    return makePath(dir.toString(), file);
}

QTreeWidgetItem *RegistryDialog::browseNodeForPath(const QString &path, QTreeWidgetItem *node)
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

QTreeWidgetItem *RegistryDialog::findPathNode(const QString &path)
{
    QString p(path);
//    if (!p.endsWith("/"))
//        p.append("/");

    for (int i = 0; i < treeWidgetRegistry->topLevelItemCount(); ++i) {
        auto t = browseNodeForPath(p, treeWidgetRegistry->topLevelItem(i));
        if (t)
            return t;
    }
    return nullptr;
}

void RegistryDialog::on_treeWidgetValues_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)
    auto typeName = item->text(1);
    auto type = RegistryValueEditorDialog::None;
    if (typeName == "REG_NONE") type = RegistryValueEditorDialog::String;
    if (typeName == "REG_SZ") type = RegistryValueEditorDialog::String;
    if (typeName == "REG_EXPAND_SZ") type = RegistryValueEditorDialog::String;
    if (typeName == "REG_BINARY") type = RegistryValueEditorDialog::Hex;
    if (typeName == "REG_DWORD") type = RegistryValueEditorDialog::Number;
    if (typeName == "REG_DWORD_LITTLE_ENDIAN") type = RegistryValueEditorDialog::Number;
    if (typeName == "REG_QWORD") type = RegistryValueEditorDialog::Number;
    if (typeName == "REG_QWORD_LITTLE_ENDIAN") type = RegistryValueEditorDialog::Number;
    if (typeName == "REG_DWORD_BIG_ENDIAN") type = RegistryValueEditorDialog::Number;
    if (typeName == "REG_LINK") type = RegistryValueEditorDialog::String;
    if (typeName == "REG_MULTI_SZ") type = RegistryValueEditorDialog::MultiString;

    if (type == RegistryValueEditorDialog::None) {
        return;
    }
    RegistryValueEditorDialog ed(type, this);
    ed.setName(item->text(0));
    ed.setValue(item->text(2));
    if (ed.exec() == QDialog::Accepted) {
        QVariant v = ed.value();
        auto hive = treeWidgetRegistry->currentItem()->data(0, RootHiveKey).toInt();
        auto path = treeWidgetRegistry->currentItem()->data(0, FullPath).toString();

        _reg->setValue(hive, path, item->text(0), v)->then([this, item, v](bool ok){
            if (ok) {
                item->setText(2, v.toString());
                treeWidgetRegistry->currentItem()->setData(0, Values, QVariant());
            } else {
                QMessageBox::warning(this, "Edit value", "Unable to save new value");
            }
        });
    }
}

bool RegistryDialog::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == treeWidgetValues && event->type() == QEvent::KeyPress) {
        auto e = reinterpret_cast<QKeyEvent*>(event);
        if (e->key() == Qt::Key_Delete) {
            auto currentItem = treeWidgetValues->currentItem();
            if (!currentItem)
                return true;
            auto hive = treeWidgetRegistry->currentItem()->data(0, RootHiveKey).toInt();
            auto path = treeWidgetRegistry->currentItem()->data(0, FullPath).toString();
            _reg->deleteValue(hive, path, currentItem->text(0))->then([this, currentItem](bool ok){
               if (ok) {
                   treeWidgetRegistry->currentItem()->setData(0, Values, QVariant());
                   delete currentItem;
               }
            });
        }
    }
    return QDialog::eventFilter(watched, event);
}
