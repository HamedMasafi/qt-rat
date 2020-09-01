#ifndef FILETRANSFERPROGRESSDIALOG_H
#define FILETRANSFERPROGRESSDIALOG_H

#include "ui_filetransferprogressdialog.h"
#include "../global.h"

class FileTransferProgressDialog : public QDialog, private Ui::FileTransferProgressDialog
{
    Q_OBJECT

    TransferType _type;
public:
    explicit FileTransferProgressDialog(QWidget *parent = nullptr);

    TransferType type() const;
    void setType(const TransferType &type);

protected:
    void changeEvent(QEvent *e);
};

#endif // FILETRANSFERPROGRESSDIALOG_H
