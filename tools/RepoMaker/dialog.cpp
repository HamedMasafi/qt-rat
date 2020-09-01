#include "dialog.h"

#include <QDebug>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMessageBox>
#include <QLibraryInfo>
#include <QJSEngine>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    QDir d(PATH);
    d.cdUp();
    d.cdUp();
    binPath = d.path() + "/output";
    d.cd("installer");
    outputPath = d.path();
    updatePath();

    qDebug() << "qt dir=" << QLibraryInfo::location(QLibraryInfo::PrefixPath);
}

bool Dialog::checkFiles() const
{
    QString server = QString("%1/%2-%3/server")
            .arg(binPath, comboBoxPlatform->currentText(), comboBoxArch->currentText());
    QString builder = QString("%1/builder/%2/client")
            .arg(binPath, comboBoxPlatform->currentText());
    if (!QFile::exists(server))
        return false;
    if (!QFile::exists(builder))
        return false;
    return true;
}

void Dialog::updatePath()
{
    lineEditPath->setText(outputPath
                          + "/" + comboBoxPlatform->currentText().toLower()
                          + "-" + comboBoxArch->currentText());
}

void Dialog::on_comboBoxPlatform_activated(const QString &)
{
    updatePath();
}

void Dialog::on_comboBoxArch_activated(const QString &)
{
    updatePath();
}

void Dialog::copyDir(const QString &from, const QString &to)
{
    QDir t(to);
    QDir f(from);
    if (!f.exists())
        return;

    if (!t.exists())
        t.mkdir(to);

    auto files = f.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    foreach (auto f, files)
        if (!QFile::copy(f.absoluteFilePath(), to + "/" + f.fileName()))
            qDebug() << "Unable copy from " <<
                        f.absoluteFilePath()
                     << " to " << to + "/" + f.fileName();


    auto subdirs = f.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    foreach (auto subdir, subdirs)
        copyDir(from + "/" + subdir, to + "/" + subdir);
}

void Dialog::on_pushButtonBuild_clicked()
{
    if (!checkFiles())
        return;
}
