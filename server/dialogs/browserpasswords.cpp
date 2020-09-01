#include "browserpasswords.h"
#include "../common/abstractclient.h"
#include "../core/variantlistmodel.h"

BrowserPasswords::BrowserPasswords(AbstractClient *client, QWidget *parent) :
    ControllerBase(client, parent)
{
    setupUi(this);

    _model = new VariantListModel(this);
    _model->addHeader("browser", tr("Browser"));
    _model->addHeader("url", tr("Url"));
    _model->addHeader("username", tr("Username"));
    _model->addHeader("password", tr("Password"));

    _proxy = new VariantProxyModel(this);
    _proxy->setSourceModel(_model);
    tableView->setModel(_proxy);

    QPointer<BrowserPasswords> me(this);
    client->passwords()->then([this, me](QVariantList passList){
        if (!me)
            return;
        me->_model->setData(passList);
        setBusy(false);
    });
}

void BrowserPasswords::on_lineEditFilter_textChanged(const QString &s)
{
    if (s.isEmpty())
        _proxy->setFilter("username", QVariant());
    else
        _proxy->setFilter("username", s);
}

void BrowserPasswords::on_comboBoxBrowser_activated(const QString &b)
{
    if (b.isEmpty())
        _proxy->setFilter("browser", QVariant());
    else
        _proxy->setFilter("browser", b);
}
