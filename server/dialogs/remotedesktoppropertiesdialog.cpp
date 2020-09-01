#include "remotedesktoppropertiesdialog.h"

#include <appsettings.h>

RemoteDesktopPropertiesDialog::RemoteDesktopPropertiesDialog(QWidget *parent) :
                                                                                QDialog(parent)
{
    setupUi(this);

    QList<QImage::Format> formats = QList<QImage::Format>()
                                    << QImage::Format_Mono
                                    << QImage::Format_RGB16
                                    << QImage::Format_RGB666
                                    << QImage::Format_RGB32
                                    << QImage::Format_ARGB32;
    spinBoxPort->setValue(RatAppSettings->remoteDesktopPort());
}

QSize RemoteDesktopPropertiesDialog::resulation() const
{
    if (radioButtonOriginalResulation->isChecked())
        return QSize(0, 0);
    return QSize(spinBox->value(), spinBox_2->value());
}

int RemoteDesktopPropertiesDialog::port() const
{
    return spinBoxPort->value();
}

QImage::Format RemoteDesktopPropertiesDialog::format() const
{
    switch (comboBoxFormat->currentIndex()) {
    case 0:
        return QImage::Format_Mono;
    case 1:
        return QImage::Format_RGB16;
    case 2:
        return QImage::Format_ARGB32;
    default:
        return QImage::Format_Invalid;
    }
}
