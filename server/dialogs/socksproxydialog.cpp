#include "socksproxydialog.h"
#include "../common/abstractclient.h"
#include <QHostAddress>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <appsettings.h>

#include <QTimer>
#include <proxyserver.h>

QPointer<ProxyServer> SocksProxyDialog::_proxyServer = nullptr;

void SocksProxyDialog::changeuiByStatus(bool status)
{
    if (status)
        labelStatus->setText(tr("Running"));
    else
        labelStatus->setText(tr("Not running"));

    pushButtonStart->setEnabled(!status);
    pushButtonStop->setEnabled(status);
    pushButtonTest->setEnabled(status);
    widgetProxyOptions->setEnabled(!status);
}

SocksProxyDialog::SocksProxyDialog(AbstractClient *client, QWidget *parent) :
    ControllerBase(client, parent)
{
    setupUi(this);

//    labelAddress->setText(client->hub()->localAddress().toString());
    labelStatus->setText(tr("Getting status..."));
    pushButtonStart->setEnabled(false);
    spinBoxLocalPort->setValue(AppSettings::instance()->localProxyPort());
    spinBoxRemotePort->setValue(AppSettings::instance()->clientConnectionProxyPort());

    client->querySocksProxy()->then([this](QVariant v){
        QVariantMap map = v.toMap();
        bool ok = map.value("status").toBool();
        changeuiByStatus(ok);

        if (ok) {
            spinBoxRemotePort->setValue(map.value("port").toInt());
            doubleSpinBoxThrottle->setValue(map.value("throttle").toDouble());
        }
    });
}

void SocksProxyDialog::testProxy()
{
    pushButtonTest->setEnabled(false);
    QNetworkProxy proxy;
    proxy.setType(QNetworkProxy::Socks5Proxy);
    proxy.setPort(spinBoxLocalPort->value());
    proxy.setHostName("127.0.0.1");

    QNetworkAccessManager nam;
    nam.setProxy(proxy);
    QNetworkRequest req(QUrl("http://api.ipify.org"));

    QEventLoop loop;

    QNetworkReply *reply = nam.get(req);
    connect(&nam, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);
    connect(&nam, &QNetworkAccessManager::finished, reply, &QObject::deleteLater);

    bool finished = false;
    QTimer::singleShot(15000, [&loop, &finished](){
        finished = true;
        loop.quit();
    });
    loop.exec();

    if (finished) {
        QMessageBox::warning(this, "Proxy test", "Timeout");
    } else {
        if (reply->error() == QNetworkReply::NoError)
            QMessageBox::information(this, "Proxy test", "Proxy connected successfully, your ip is: " + reply->readAll());
        else
            QMessageBox::warning(this, "Proxy test", reply->errorString());
    }

    pushButtonTest->setEnabled(true);
}

void SocksProxyDialog::setSystemProxy()
{
#ifdef Q_OS_WINDOWS
    QSettings reg("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Internet Settings", QSettings::NativeFormat);
    reg.setValue("ProxyEnable", 1u);
    reg.setValue("ProxyServer", QString("127.0.0.1:%1").arg(spinBoxLocalPort->value()));
    reg.sync();
#endif
}

void SocksProxyDialog::unsetSystemProxy()
{
#ifdef Q_OS_WINDOWS
    QSettings reg("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Internet Settings", QSettings::NativeFormat);
    reg.remove("ProxyEnable");
    reg.remove("ProxyServer");
    reg.sync();
#endif
}

void SocksProxyDialog::on_pushButtonStart_clicked()
{
    if (!client())
        return;
    labelStatus->setText(tr("Starting..."));
    if (_proxyServer) {
        _proxyServer->stop();
        _proxyServer->deleteLater();
    }
    auto server = new ProxyServer(client(), this);
    server->setLocalPort(spinBoxLocalPort->value());
    server->setRemotePort(spinBoxRemotePort->value());
    bool ok = server->start();
    _proxyServer = server;
    if (!ok) {
        QMessageBox::warning(this, "Start proxy",
                             QString("Unable to start proxy on port %1").arg(spinBoxLocalPort->value()));
        return;
    }
    client()->startSocksProxy(spinBoxRemotePort->value(), doubleSpinBoxThrottle->value())
        ->then([this, server](bool ok){
        if (ok) {
            changeuiByStatus(true);


        } else {
            labelStatus->setText(tr("Error in starting socks proxy!"));
        }
    });
}

void SocksProxyDialog::on_pushButtonStop_clicked()
{
    if (!client())
        return;
    labelStatus->setText(tr("Stopping..."));
    client()->endSocksProxy()->then([this](bool ok){
        if (ok)
            changeuiByStatus(false);
        else
            labelStatus->setText(tr("Error in stopping socks proxy!"));
    });
}

void SocksProxyDialog::on_pushButtonTest_clicked()
{
    testProxy();
}

void SocksProxyDialog::on_checkBoxSystemWideProxy_stateChanged(int s)
{
    auto state = static_cast<Qt::CheckState>(s);
    if (state == Qt::Checked)
        setSystemProxy();
    else if (state == Qt::Unchecked)
        unsetSystemProxy();
}
