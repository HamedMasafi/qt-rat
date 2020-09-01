#ifndef DOWNLOADANDEXECDIALOG_H
#define DOWNLOADANDEXECDIALOG_H

#include "ui_downloadandexecdialog.h"
#include "../core/controllerbase.h"

class DownloadAndExecDialog : public ControllerBase, private Ui::DownloadAndExecDialog
{
    Q_OBJECT

public:
    explicit DownloadAndExecDialog(AbstractClient *client, QWidget *parent = nullptr);
protected:
    void changeEvent(QEvent *e);
private slots:
    void on_pushButton_clicked();
};

#endif // DOWNLOADANDEXECDIALOG_H
