#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void changeEvent(QEvent *e);
private slots:
    void on_pushButtonDragMouse_clicked();
    void on_pushButtonRightClick_clicked();
    void on_pushButtonKeys_clicked();
};

#endif // MAINWINDOW_H
