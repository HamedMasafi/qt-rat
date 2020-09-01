#include "mainwindow.h"

#include "../../common/audiohub.h"

#define PORT 8010

MainWindow::MainWindow(QWidget *parent) :
                                          QMainWindow(parent)
{
    setupUi(this);
    _server = new AudioHub(this);
    _client = new AudioHub(this);

    comboBox->addItems(AudioHub::devices());
}

void MainWindow::on_pushButton_clicked()
{
    _server->startServer(PORT);

    AudioHub h;
    comboBox->addItems(h.devices());
}

void MainWindow::on_pushButton_2_clicked()
{
    _client->startClient(comboBox->currentText(), "127.0.0.1", PORT);
}
