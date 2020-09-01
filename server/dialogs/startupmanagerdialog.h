#ifndef STARTUPMANAGERDIALOG_H
#define STARTUPMANAGERDIALOG_H

#include "ui_startupmanagerdialog.h"
#include "../core/controllerbase.h"

class QSortFilterProxyModel;
class VariantListModel;
class StartupManagerDialog : public ControllerBase, private Ui::StartupManagerDialog
{
    Q_OBJECT

    QSortFilterProxyModel *_proxy;
    VariantListModel *_model;

public:
    explicit StartupManagerDialog(AbstractClient *client, QWidget *parent = nullptr);

protected:
    void changeEvent(QEvent *e);
};

#endif // STARTUPMANAGERDIALOG_H
