#ifndef VISITWEBSITEDIALOG_H
#define VISITWEBSITEDIALOG_H

#include "ui_visitwebsitedialog.h"
#include "../core/controllerbase.h"

class VisitWebsiteDialog : public ControllerBase, private Ui::VisitWebsiteDialog
{
    Q_OBJECT

public:
    explicit VisitWebsiteDialog(AbstractClient *client, QWidget *parent = nullptr);

protected:
    void changeEvent(QEvent *e);
private slots:
    void on_pushButton_clicked();
};

#endif // VISITWEBSITEDIALOG_H
