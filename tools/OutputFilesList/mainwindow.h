#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void browseDir(const QString &path, QStringList &files);
private slots:
    void on_pushButtonSave_clicked();
};

#endif // MAINWINDOW_H
