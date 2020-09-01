#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

class AudioHub;
class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

    AudioHub *_server;
    AudioHub *_client;

public:
    explicit MainWindow(QWidget *parent = nullptr);
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
};

#endif // MAINWINDOW_H
