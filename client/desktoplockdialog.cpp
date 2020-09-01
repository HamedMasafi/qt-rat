#include "desktoplockdialog.h"

#include <QCloseEvent>

DesktopLockDialog::DesktopLockDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    setWindowFlag(Qt::FramelessWindowHint);
    setWindowFlag(Qt::WindowStaysOnTopHint);
}

void DesktopLockDialog::changeEvent(QEvent *e)
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

void DesktopLockDialog::closeEvent(QCloseEvent *event)
{
    event->ignore();
}
