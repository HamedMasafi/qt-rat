#ifndef CLIENTBUILDWIZARD_H
#define CLIENTBUILDWIZARD_H

#include "ui_clientbuildwizard.h"

class ClientBuildWizard : public QWizard, private Ui::ClientBuildWizard
{
    Q_OBJECT
    QByteArray content;

public:
    explicit ClientBuildWizard(QWidget *parent = nullptr);

private:
    void build();
    void compressUpx();
    bool createWindowsDownloader();
    bool createClient();
    QString selectedPlatform() const;

    bool setOption(const QString &name, const QString &value);
    bool setOption(const QString &name, const int &value);
    bool setOption(const QString &name, const bool &value);

private slots:
    void on_labelStandardPaths_linkActivated(const QString &link);
    void on_lineEditInstallPath_textChanged(const QString &arg1);
    void on_toolButtonBrowseOutput_clicked();
    void on_ClientBuildWizard_accepted();

public:
    int nextId() const;

    // QWizard interface
public:
    bool validateCurrentPage();
};

#endif // CLIENTBUILDWIZARD_H
