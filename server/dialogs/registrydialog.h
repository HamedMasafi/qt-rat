#ifndef REGISTRYDIALOG_H
#define REGISTRYDIALOG_H

#include "ui_registrydialog.h"
#include "../core/controllerbase.h"

class AbstractRegistry;
class RegistryDialog : public ControllerBase, private Ui::RegistryDialog
{
    Q_OBJECT
    QMenu *valueMenu;
    QMenu *keyMenu;
    AbstractRegistry *_reg;


    enum Type {
        Computer,
        HiveKey,
        Key
    };

    enum Roles {
        IsChecked = Qt::UserRole,
        FullPath,
        Values,
        Type,
        RootHiveKey
    };

public:
    explicit RegistryDialog(AbstractClient *client, QWidget *parent = nullptr);

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

private slots:
    void on_treeWidgetRegistry_itemActivated(QTreeWidgetItem *item, int column);
    void on_treeWidgetRegistry_customContextMenuRequested(const QPoint &pos);

    void on_treeWidgetValues_itemDoubleClicked(QTreeWidgetItem *item, int column);

    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event);
};

#endif // REGISTRYDIALOG_H
