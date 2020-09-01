#include "mainwindow.h"
#include "window.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);
}

void MainWindow::on_pushButton_clicked()
{
    Window w;
    w.show();
}
