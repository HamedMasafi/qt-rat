#include "registryvalueeditordialog.h"

#include <QRegularExpressionValidator>

RegistryValueEditorDialog::RegistryValueEditorDialog(const ValueType &type, QWidget *parent) :
    QDialog(parent), _type(type)
{
    setupUi(this);

    switch (_type) {
    case String:
        setWindowTitle("Edit string");
        stackedWidget->setCurrentWidget(pageString);
        break;
    case Number:
        setWindowTitle("Edit number");
        stackedWidget->setCurrentWidget(pageXWord);
        break;
    case Hex:
        setWindowTitle("Edit hex value");
        stackedWidget->setCurrentWidget(pageHex);
        break;
    case MultiString:
        setWindowTitle("Edit multi string");
        stackedWidget->setCurrentWidget(pageMultiString);
        break;
    default:
        break;
    }

    hexEdit2->setHexCaps(true);
    hexEdit2->setBytesPerLine(8);
    hexEdit2->setOverwriteMode(false);

    _numberValidator = new QRegularExpressionValidator(this);
    lineEditNumber->setValidator(_numberValidator);
    on_radioButtonHex_toggled(true);
}

void RegistryValueEditorDialog::setName(const QString &name)
{
    lineEditName->setText(name);
}

void RegistryValueEditorDialog::setValue(const QVariant &value)
{
    switch (_type) {
    case String:
        lineEditStringValue->setText(value.toString());
        break;
    case Number:
        lineEditNumber->setText(value.toString());
        break;
    case Hex:
        hexEdit2->setData(value.toByteArray());
        break;
    case MultiString:
        textBrowserMultiString->setText(value.toString());
        break;
    default:
        break;
    }
}

QVariant RegistryValueEditorDialog::value() const
{
    switch (_type) {
    case String:
        return lineEditStringValue->text();
        break;
    case Number: {
       bool ok;
       qint64 n;
        if (radioButtonDec->isChecked())
            n = lineEditNumber->text().toLong(&ok, 10);
        else
            n = lineEditNumber->text().toLong(&ok, 16);
        if (!ok)
            return QVariant();
        return QVariant(n);
    }
    case Hex:
        return hexEdit2->data();

    case MultiString:
        return textBrowserMultiString->toPlainText();

    default:
        return QVariant();
    }
}

void RegistryValueEditorDialog::on_radioButtonHex_toggled(bool checked)
{
    if (!checked)
        return;
    bool ok;
    int tmp = lineEditNumber->text().toInt(&ok, 10);
    if (ok)
        lineEditNumber->setText(QString::number(tmp, 16));
    _numberValidator->setRegularExpression(QRegularExpression("[0-9a-fA-F]*"));
}

void RegistryValueEditorDialog::on_radioButtonDec_toggled(bool checked)
{
    if (!checked)
        return;
    bool ok;
    int tmp = lineEditNumber->text().toInt(&ok, 16);
    if (ok)
        lineEditNumber->setText(QString::number(tmp, 10));
    _numberValidator->setRegularExpression(QRegularExpression("[0-9]*"));
}
