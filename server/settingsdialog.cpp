#include "appsettings.h"
#include "settingsdialog.h"

#include <QMessageBox>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    spinBoxBroadcastPort->setValue(RatAppSettings->broadcastPort());
    spinBoxNeuronPort->setValue(RatAppSettings->ratPort());
    spinBoxRDPPort->setValue(RatAppSettings->remoteDesktopPort());
    spinBoxWebPort->setValue(RatAppSettings->installerPort());
    checkBoxBroadcast->setChecked(RatAppSettings->enableBroadcasting());

    if (!AppSettings::supportsInstaller()) {
        spinBoxWebPort->hide();
        labelWebPort->hide();
    }
}

void SettingsDialog::on_buttonBox_accepted()
{
    RatAppSettings->setBroadcastPort(spinBoxBroadcastPort->value());
    RatAppSettings->setRatPort(spinBoxNeuronPort->value());
    RatAppSettings->setRemoteDesktopPort(spinBoxRDPPort->value());
    RatAppSettings->setInstallerPort(spinBoxWebPort->value());
    RatAppSettings->setEnableBroadcasting(checkBoxBroadcast->isChecked());
    RatAppSettings->sync();

    QMessageBox::information(this, tr("Change settings"),
                             tr(" You must restart the server for changes to take effect."));
    accept();
}
