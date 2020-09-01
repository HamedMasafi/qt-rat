#include "systeminfodialog.h"
#include "../common/abstractclient.h"
#include <QDebug>
#include <QSortFilterProxyModel>
#include "../core/variantlistmodel.h"

SystemInfoDialog::SystemInfoDialog(AbstractClient *client, QWidget *parent) :
    ControllerBase(client, parent)
{
    setupUi(this);

    _model = new VariantListModel(this);
    _model->addHeader("name", tr("Name"));
    _model->addHeader("value", tr("Value"));

    _proxy = new QSortFilterProxyModel(this);
    _proxy->setFilterKeyColumn(0);

    _proxy->setSourceModel(_model);
    tableView->setModel(_proxy);

    client->systemInformations()->then([this](QVariant value){
        _model->setData(value.toList());
    });
}

void SystemInfoDialog::changeEvent(QEvent *e)
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

void SystemInfoDialog::on_lineEditFilter_textChanged(const QString &s)
{
    _proxy->setFilterFixedString(s);
}
