#include "mainwindow.h"
#include "browserhelper.h"
#include <QDebug>
#include "../../server/core/variantlistmodel.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);

    _model = new VariantListModel(this);
    _model->addHeader("browser", tr("Browser"));
    _model->addHeader("url", tr("Url"));
    _model->addHeader("username", tr("Username"));
    _model->addHeader("password", tr("Password"));

    _proxy = new VariantProxyModel(this);
    _proxy->setSourceModel(_model);
    tableView->setModel(_proxy);
}

void MainWindow::on_pushButton_clicked()
{
    auto m = BrowserHelper::passwords();
    if (m.size())
        _model->setData(m);
}

void MainWindow::on_pushButton_2_clicked()
{
    listWidget->addItems(BrowserHelper::tabs());
}

void MainWindow::on_comboBox_activated(const QString &b)
{
    if (b.isEmpty())
        _proxy->setFilter("browser", QVariant());
    else
        _proxy->setFilter("browser", b);
}

void MainWindow::on_lineEdit_textChanged(const QString &s)
{
    if (s.isEmpty())
        _proxy->setFilter("username", QVariant());
    else
        _proxy->setFilter("username", s);
}
