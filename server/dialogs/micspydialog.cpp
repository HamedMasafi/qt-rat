#include "micspydialog.h"

#include <QAudioFormat>
#include <QMessageBox>
#include "../common/audiohub.h"
#include "../common/abstractclient.h"
#include "appsettings.h"

MicSpyDialog::MicSpyDialog(AbstractClient *client, QWidget *parent) :
                                              ControllerBase(client, parent)
{
    setupUi(this);

    spinBoxPort->setValue(RatAppSettings->micSpyPort());
    client->micList()->then([this](QStringList list){
        comboBoxInputSource->addItems(list);
    });
}

void MicSpyDialog::on_pushButtonStart_clicked()
{
    if (comboBoxInputSource->currentIndex() == -1) {
        QMessageBox::warning(this, windowTitle(), "Please select a source");
        return;
    }
    auto hub = new AudioHub(AudioHub::Tcp, this);
    hub->startServer(spinBoxPort->value());
    setBusy(true);
    client()->startMicSpy(comboBoxInputSource->currentText(), spinBoxPort->value())->then([hub, this](bool ok){
        if (ok)
            enableUi(true);
        else
            hub->stopServer();
        setBusy(false);
    });
}

void MicSpyDialog::on_pushButtonStop_clicked()
{
    setBusy(true);
    client()->stopMicSpy()->then([this](bool ok){
        setBusy(false);
        if (ok)
            enableUi(false);
    });
}

void MicSpyDialog::enableUi(bool running)
{
    pushButtonStart->setEnabled(!running);
    pushButtonStop->setEnabled(running);
    labelStatus->setText(running ? tr("Running") : tr("Not running"));
    spinBoxPort->setEnabled(!running);
}
