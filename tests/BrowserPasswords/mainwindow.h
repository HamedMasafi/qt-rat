#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

class VariantListModel;
class VariantProxyModel;
class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT
    VariantListModel *_model;
    VariantProxyModel *_proxy;

public:
    explicit MainWindow(QWidget *parent = nullptr);
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_comboBox_activated(const QString &arg1);
    void on_lineEdit_textChanged(const QString &arg1);
};

#endif // MAINWINDOW_H
