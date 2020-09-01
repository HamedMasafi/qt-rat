#include "filetransferprogressdialog.h"

TransferType FileTransferProgressDialog::type() const
{
    return _type;
}

void FileTransferProgressDialog::setType(const TransferType &type)
{
    _type = type;
}

FileTransferProgressDialog::FileTransferProgressDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
}

void FileTransferProgressDialog::changeEvent(QEvent *e)
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
