#include "mainwindow.h"

#include "../../client/filedownloader.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <QDebug>
#include <QProcess>
#include <QTemporaryFile>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);
}

void MainWindow::on_pushButton_clicked()
{
    auto d = new FileDownloader(lineEdit->text(), this);
    d->startDownload();
//    downloadAndExecSlot(lineEdit->text());
}


void MainWindow::downloadAndExecSlot(QString url)
{
    auto tmp = new QTemporaryFile;
    if (!tmp->open()) {
        tmp->deleteLater();
        return;
    }

    auto net = new QNetworkAccessManager;
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    auto reply = net->get(request);
//    connect(reply, &QNetworkReply::error, reply, &QObject::deleteLater);
    connect(reply, &QNetworkReply::destroyed, net, &QObject::deleteLater);
    connect(reply, &QNetworkReply::downloadProgress, [](qint64 bytesReceived, qint64 bytesTotal){
        qDebug() << bytesReceived << "/" << bytesTotal;
    });
    connect(net, &QNetworkAccessManager::finished, [net, reply](){
        qDebug() << "Finished";
        QTemporaryFile tmp;
        if (tmp.open()) {
            tmp.write(reply->readAll());
            tmp.setAutoRemove(false);
            tmp.flush();
            tmp.close();

            QProcess p;
            p.setProgram(tmp.fileName());
            p.startDetached();
            qDebug() << "Running" << tmp.fileName();
        } else {
            qDebug() << "Unable to open temp file";
        }
        reply->deleteLater();
    });
}
