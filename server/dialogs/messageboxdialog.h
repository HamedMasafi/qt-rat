#ifndef MESSAGEBOXDIALOG_H
#define MESSAGEBOXDIALOG_H

#include "ui_messageboxdialog.h"
#include "../core/controllerbase.h"

class MessageBoxDialog : public ControllerBase, private Ui::MessageBoxDialog
{
    Q_OBJECT

public:
    explicit MessageBoxDialog(AbstractClient *client, QWidget *parent = nullptr);

protected:
    void changeEvent(QEvent *e);
private slots:
    void on_pushButton_clicked();
};

#endif // MESSAGEBOXDIALOG_H
