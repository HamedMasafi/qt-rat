#include "mainwindow.h"

#include "../../client/messagebox.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);
}

void MainWindow::on_pushButtonShow_clicked()
{
    MessageBox::show();
}
