#include "screenshotdialog.h"
#include "../common/abstractclient.h"

#include <QFileDialog>

ScreenShotDialog::ScreenShotDialog(AbstractClient *client, QWidget *parent) :
    ControllerBase(client, parent)
{
    setupUi(this);
    on_pushButtonRefresh_clicked();
}

void ScreenShotDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

void ScreenShotDialog::on_pushButtonRefresh_clicked()
{
    if (!client())
        return;
    client()->screenshot()->then([this](QImage img){
        _image = img;
        pushButtonSave->setEnabled(true);
        label->setPixmap(QPixmap::fromImage(img));
    });
}

void ScreenShotDialog::on_pushButtonSave_clicked()
{
    QFileDialog d(this);
    d.setAcceptMode(QFileDialog::AcceptSave);
    d.setNameFilters(QStringList() << "*.png" << "*.jpg" << "*.bmp");
    d.setWindowTitle(tr("Save"));
    auto r = d.exec();

    if (r != QDialog::Accepted || !d.selectedFiles().count())
        return;

    auto fileName = d.selectedFiles().first();
    auto ext = d.selectedNameFilter().replace("*.", "");

    if (!fileName.endsWith("." + ext, Qt::CaseInsensitive))
        fileName.append("." + ext);

    _image.save(fileName, ext.toUpper().toLatin1().data());
}
