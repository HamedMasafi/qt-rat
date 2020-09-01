#ifndef REMOTEDESKTOPPROPERTIESDIALOG_H
#define REMOTEDESKTOPPROPERTIESDIALOG_H

#include "ui_remotedesktoppropertiesdialog.h"

class RemoteDesktopPropertiesDialog : public QDialog, private Ui::RemoteDesktopPropertiesDialog
{
    Q_OBJECT

public:
    explicit RemoteDesktopPropertiesDialog(QWidget *parent = nullptr);

    QSize resulation() const;
    int port() const;
    QImage::Format format() const;
};

#endif // REMOTEDESKTOPPROPERTIESDIALOG_H
