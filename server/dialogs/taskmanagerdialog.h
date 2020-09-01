#ifndef TASKMANAGERDIALOG_H
#define TASKMANAGERDIALOG_H

#include "ui_taskmanagerdialog.h"
#include "../core/controllerbase.h"

class QSortFilterProxyModel;
class VariantWrapperModel;
class TaskManagerDialog : public ControllerBase, private Ui::TaskManagerDialog
{
    Q_OBJECT
    QSortFilterProxyModel *_proxy;
    VariantWrapperModel *_model;

public:
    explicit TaskManagerDialog(AbstractClient *client, QWidget *parent = nullptr);

protected:
    void changeEvent(QEvent *e);
private slots:
    void on_lineEdit_textChanged(const QString &arg1);
    void on_pushButtonKill_clicked();
    void on_pushButtonRefresh_clicked();
};

#endif // TASKMANAGERDIALOG_H
