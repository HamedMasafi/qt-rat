#include "browseropentabsdialog.h"
#include "../common/abstractclient.h"

#include <QDesktopServices>
#include <QUrl>

BrowserOpenTabsDialog::BrowserOpenTabsDialog(AbstractClient *client, QWidget *parent) :
    ControllerBase(client, parent)
{
    setupUi(this);

    client->grabUrls()->then([this](QVariant v){
        if (v.type() != QVariant::List)
            return;

        auto list = v.toList();
        foreach (auto i, list) {
            listWidget->addItem(i.toString());
        }
    });
}

void BrowserOpenTabsDialog::changeEvent(QEvent *e)
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

void BrowserOpenTabsDialog::on_pushButton_clicked()
{
    if (listWidget->currentIndex().isValid())
        QDesktopServices::openUrl(QUrl(listWidget->currentItem()->text()));
}
