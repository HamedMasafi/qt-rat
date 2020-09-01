#include "downloadandexecdialog.h"
#include "../common/abstractclient.h"

#include <QMessageBox>

DownloadAndExecDialog::DownloadAndExecDialog(AbstractClient *client, QWidget *parent) :
    ControllerBase(client, parent)
{
    setupUi(this);
}

void DownloadAndExecDialog::changeEvent(QEvent *e)
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

void DownloadAndExecDialog::on_pushButton_clicked()
{
    if (!client())
        return;

    client()->downloadAndExec(lineEdit->text());
    QMessageBox::information(this, windowTitle(),
                             "Starting download on client...");
    close();
}
