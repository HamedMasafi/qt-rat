#ifndef SOCKSPROXYDIALOG_H
#define SOCKSPROXYDIALOG_H

#include "ui_socksproxydialog.h"
#include "../core/controllerbase.h"

#include <proxyserver.h>

class SocksProxyDialog : public ControllerBase, private Ui::SocksProxyDialog
{
    Q_OBJECT
    void changeuiByStatus(bool status);
    static QPointer<ProxyServer> _proxyServer;

public:
    explicit SocksProxyDialog(AbstractClient *client, QWidget *parent = nullptr);
private slots:
    void testProxy();
    void setSystemProxy();
    void unsetSystemProxy();
    void on_pushButtonStart_clicked();
    void on_pushButtonStop_clicked();
    void on_pushButtonTest_clicked();
    void on_checkBoxSystemWideProxy_stateChanged(int arg1);
};

#endif // SOCKSPROXYDIALOG_H
