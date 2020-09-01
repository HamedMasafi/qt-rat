#include "mainwindow.h"
#include "registry.h"
#include "regkey.h"

#include <QSettings>
#include <QDebug>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);
    QStringList roots = QStringList()
        << "HKEY_CLASSES_ROOT"
        << "HKEY_CURRENT_USER"
        << "HKEY_LOCAL_MACHINE"
        << "HKEY_USERS"
        << "HKEY_CURRENT_CONFIG";

    QTreeWidgetItem *computer = new QTreeWidgetItem;
    computer->setText(0, "Computer");
    computer->setData(0, Type, Computer);
    treeWidget->addTopLevelItem(computer);
    foreach (auto r, roots) {
        auto i = new QTreeWidgetItem;
        i->setText(0, r);
        i->setData(0, Type, HKey);
        computer->addChild(i);
    }
}

void MainWindow::on_pushButton_clicked()
{
    Registry r;
    r.keys();
}

void MainWindow::on_pushButton_2_clicked()
{
    RegKey  key{ RegKey::CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion" };
    qDebug() << key.EnumSubKeys();

    RegKey  key2{ RegKey::CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run" };
    qDebug() << key2.EnumValues();

    qDebug() << key2.GetStringValue("RAT");
}

void MainWindow::on_pushButton_3_clicked()
{
    qDebug() << "OK";
    QSettings reg("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::Registry32Format);
    qDebug() << "Keys="<<reg.childKeys() << reg.childGroups();
    qDebug() << reg.value("ss");
    QString s;

}

void MainWindow::on_treeWidget_itemActivated(QTreeWidgetItem *item, int column)
{
    if (item->data(0, Type).toInt() == Computer)
        return;

    if (item->data(0, IsChecked).toBool())
        return;

    item->setData(0, IsChecked, true);
}
