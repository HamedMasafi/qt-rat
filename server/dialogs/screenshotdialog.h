#ifndef SCREENSHOTDIALOG_H
#define SCREENSHOTDIALOG_H

#include "ui_screenshotdialog.h"
#include "../core/controllerbase.h"

class ScreenShotDialog : public ControllerBase, private Ui::ScreenShotDialog
{
    Q_OBJECT
    QImage _image;

public:
    explicit ScreenShotDialog(AbstractClient *client, QWidget *parent = nullptr);

protected:
    void changeEvent(QEvent *e);
private slots:
    void on_pushButtonRefresh_clicked();
    void on_pushButtonSave_clicked();
};

#endif // SCREENSHOTDIALOG_H
