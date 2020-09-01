#include "startupmanagerdialog.h"
#include "../core/variantlistmodel.h"
#include "../common/abstractclient.h"

#include <QSortFilterProxyModel>
#include <QStandardItemModel>

StartupManagerDialog::StartupManagerDialog(AbstractClient *client, QWidget *parent) :
    ControllerBase(client, parent)
{
    setupUi(this);

    _model = new VariantListModel(this);
    _model->addHeader("path", tr("Path"));
    _model->addHeader("exec", tr("Command"));

    _proxy = new QSortFilterProxyModel(this);
    _proxy->setFilterKeyColumn(0);
    _proxy->setSourceModel(_model);
    tableView->setModel(_proxy);
    QPointer<StartupManagerDialog> me(this);
    client->startupItems()->then([me](QVariant v){
        if (!me)
            return;
        auto list = v.toList();
        me->_model->setData(list);
    });
}

void StartupManagerDialog::changeEvent(QEvent *e)
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
