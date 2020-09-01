#ifndef DESKTOPLOCKDIALOG_H
#define DESKTOPLOCKDIALOG_H

#include "ui_desktoplockdialog.h"

class DesktopLockDialog : public QDialog, private Ui::DesktopLockDialog
{
    Q_OBJECT

public:
    explicit DesktopLockDialog(QWidget *parent = nullptr);

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
};

#endif // DESKTOPLOCKDIALOG_H
