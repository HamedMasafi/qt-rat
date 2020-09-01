#ifndef CREATECLIENTDIALOG_H
#define CREATECLIENTDIALOG_H

#include "ui_createclientdialog.h"

class CreateClientDialog : public QDialog, private Ui::CreateClientDialog
{
    Q_OBJECT

    QString selectedPlatform() const;
public:
    explicit CreateClientDialog(QWidget *parent = nullptr);

protected:
    void changeEvent(QEvent *e);
private slots:
    void compressUpx();
    bool createWindowsDownloader();
    bool createClient();
    void on_pushButtonBuild_clicked();
    void on_toolButtonBrowseOutput_clicked();
};

#endif // CREATECLIENTDIALOG_H
