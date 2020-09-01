#ifndef BROWSEROPENTABSDIALOG_H
#define BROWSEROPENTABSDIALOG_H

#include "ui_browseropentabsdialog.h"
#include "../core/controllerbase.h"

class AbstractClient;
class BrowserOpenTabsDialog : public ControllerBase, private Ui::BrowserOpenTabsDialog
{
    Q_OBJECT

public:
    explicit BrowserOpenTabsDialog(AbstractClient *client, QWidget *parent = nullptr);

protected:
    void changeEvent(QEvent *e);
private slots:
    void on_pushButton_clicked();
};

#endif // BROWSEROPENTABSDIALOG_H
