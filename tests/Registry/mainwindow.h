#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

    enum Type {
        Computer,
        HKey,
        Key
    };

    enum Roles {
        IsChecked = Qt::UserRole,
        FullPath,
        Type
    };
public:
    explicit MainWindow(QWidget *parent = nullptr);
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_treeWidget_itemActivated(QTreeWidgetItem *item, int column);
};

#endif // MAINWINDOW_H
