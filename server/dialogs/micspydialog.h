#ifndef MICSPYDIALOG_H
#define MICSPYDIALOG_H

#include "ui_micspydialog.h"
#include "../core/controllerbase.h"

class AbstractClient;
class MicSpyDialog : public ControllerBase, private Ui::MicSpyDialog
{
    Q_OBJECT

    void enableUi(bool ok);
public:
    explicit MicSpyDialog(AbstractClient *client, QWidget *parent = nullptr);
private slots:
    void on_pushButtonStart_clicked();
    void on_pushButtonStop_clicked();
};

#endif // MICSPYDIALOG_H
