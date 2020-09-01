#include "visitwebsitedialog.h"
#include "../common/abstractclient.h"

VisitWebsiteDialog::VisitWebsiteDialog(AbstractClient *client, QWidget *parent) :
    ControllerBase(client, parent)
{
    setupUi(this);
}

void VisitWebsiteDialog::changeEvent(QEvent *e)
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

void VisitWebsiteDialog::on_pushButton_clicked()
{
    if (client())
        client()->visitWebsite(lineEdit->text(), 0);
}
