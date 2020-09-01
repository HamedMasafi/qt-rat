#ifndef SYSTEMINFODIALOG_H
#define SYSTEMINFODIALOG_H

#include "ui_systeminfodialog.h"
#include "../core/controllerbase.h"

class VariantListModel;
class QSortFilterProxyModel;
class SystemInfoDialog : public ControllerBase, private Ui::SystemInfoDialog
{
    Q_OBJECT
    VariantListModel *_model;
    QSortFilterProxyModel *_proxy;

public:
    explicit SystemInfoDialog(AbstractClient *client, QWidget *parent = nullptr);

protected:
    void changeEvent(QEvent *e);
private slots:
    void on_lineEditFilter_textChanged(const QString &s);
};

#endif // SYSTEMINFODIALOG_H
