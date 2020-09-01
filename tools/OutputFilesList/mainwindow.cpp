#include "mainwindow.h"

#include <QDir>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
                                          QMainWindow(parent)
{
    setupUi(this);

    QDir d(PATH);
    d.cdUp();
    d.cdUp();
    d.cd("output");
    lineEditPath->setText(d.path());

    auto platforms = d.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    QStringList files;
    foreach (auto platform, platforms) {
        QDir platformDir(platform.path());
        auto archs = platformDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
        foreach (auto arch, archs) {
            browseDir(platform.path(), files);
        }
    }

    auto p = d.path();
    foreach(QString f, files)
        textBrowserFiles->append(f.replace(p, ""));
}

void MainWindow::browseDir(const QString &path, QStringList &files)
{
    QDir d(path);
    auto dirs = d.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    foreach(auto dir, dirs) {
        qDebug() << dir;
        browseDir(dir.absoluteFilePath(), files);
    }

    auto filesList = d.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    foreach (auto fi, filesList) {
        files.append(fi.absoluteFilePath());
    }
}

void MainWindow::on_pushButtonSave_clicked()
{
    QFile f(lineEditPath->text() + "/files.txt");
    if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        f.write(textBrowserFiles->toPlainText().toLocal8Bit());
        f.close();
    }
}
