#include "messageboxdialog.h"
#include "../common/abstractclient.h"

MessageBoxDialog::MessageBoxDialog(AbstractClient *client, QWidget *parent) :
    ControllerBase(client, parent)
{
    setupUi(this);
}

void MessageBoxDialog::changeEvent(QEvent *e)
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

void MessageBoxDialog::on_pushButton_clicked()
{
    if (!client())
        return;
    client()->messageBox(lineEditTitle->text(), textEditBody->toPlainText(), 0);
    accept();
}
