#include "webcamviewdialog.h"

#include "../common/abstractclient.h"

#include <QTime>
#include <qevent.h>

WebcamViewDialog::WebcamViewDialog(AbstractClient *client, QWidget *parent) :
    ControllerBase(client, parent), closing(false), capturing(false)
{
    setupUi(this);

    client->cameras()->then([this](QStringList cameras) {
        foreach (auto c, cameras)
            comboBoxDevice->addItem(c);
    });
}

void WebcamViewDialog::startCapturing()
{
    client()->cameraFrame()->then([this](QImage img){
        if (img.isNull()) {
            labelStatus->setText(tr("Invalid frame"));
        } else {
            labelStatus->setText(tr("Last frame: ") + QTime::currentTime().toString());
            label->setPixmap(QPixmap::fromImage(img));
        }

        if (!closing)
            metaObject()->invokeMethod(this, "startCapturing", Qt::QueuedConnection);
        else
            client()->stopCamera()->then([this]() {
                close();
            });
    });
}

void WebcamViewDialog::closeEvent(QCloseEvent *event)
{
    if (!closing)
        event->ignore();

    closing = true;

//    client()->stopCamera();
}

void WebcamViewDialog::changeEvent(QEvent *e)
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

void WebcamViewDialog::on_pushButtonStart_clicked()
{
    if (capturing) {
        comboBoxDevice->setEnabled(true);
        pushButtonStart->setText(tr("Start"));
        return;
    }
    comboBoxDevice->setEnabled(false);
    client()->startCamera(comboBoxDevice->currentText().toLatin1())->then([this](bool ok){
        if (ok) {
            startCapturing();
            capturing = true;
            pushButtonStart->setText(tr("End"));
        } else {
            label->setText(tr("Unable to connect to webcam"));
            closing = true;
        }
    });
}
