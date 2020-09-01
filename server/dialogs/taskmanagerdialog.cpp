#include "taskmanagerdialog.h"
#include "../core/variantlistmodel.h"
#include "../common/abstractclient.h"
#include "../common/variantwrapper.h"
#include "core/variantwrappermodel.h"

#include <QSortFilterProxyModel>
#include <QStandardItemModel>

TaskManagerDialog::TaskManagerDialog(AbstractClient *client, QWidget *parent) :
                                                                                ControllerBase(client, parent)
{
    setupUi(this);
    _proxy = new QSortFilterProxyModel(this);
    _proxy->setFilterKeyColumn(0);

    _model = new VariantWrapperModel(this);
    _model->setHeaderText("name", tr("Name"));
    _model->setHeaderText("usage", tr("Memory usage"));
    _model->setHeaderText("pid", tr("PID"));
    _model->setHeaderText("user", tr("User"));
    _model->setHeaderText("section", tr("Type"));
    _model->setHeaderText("status", tr("Status"));
    _model->setHeaderText("title", tr("Window title"));
    _model->setHeaderText("cpu", tr("CPU"));
    _model->setHeaderText("cmd", tr("Command"));


    _proxy->setSourceModel(_model);
    tableView->setModel(_proxy);

    on_pushButtonRefresh_clicked();
}

void TaskManagerDialog::changeEvent(QEvent *e)
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

void TaskManagerDialog::on_lineEdit_textChanged(const QString &s)
{
    _proxy->setFilterFixedString(s);
}

void TaskManagerDialog::on_pushButtonKill_clicked()
{
    bool ok;
    auto idx = tableView->currentIndex();
    auto pid = _model->data(idx.row(), "pid").toInt(&ok);

    if (ok)
        client()->killProcess(pid);
}

void TaskManagerDialog::on_pushButtonRefresh_clicked()
{
    setBusy(true);
    QPointer<TaskManagerDialog> me(this);
    client()->tasks()->then([this, me](QVariant p){
        if (!me)
            return;

        VariantWrapper w;
        w.setData(p);
        me->_model->setVariant(w);
        setBusy(false);
    });
}
