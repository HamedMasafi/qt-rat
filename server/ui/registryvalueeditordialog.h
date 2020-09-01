#ifndef REGISTRYVALUEEDITORDIALOG_H
#define REGISTRYVALUEEDITORDIALOG_H

#include "ui_registryvalueeditordialog.h"

class QRegularExpressionValidator;
class RegistryValueEditorDialog : public QDialog, private Ui::RegistryValueEditorDialog
{
    Q_OBJECT
    QRegularExpressionValidator *_numberValidator;

public:
    enum ValueType {
        None,
        String,
        Number,
        Hex,
        MultiString
    };

    explicit RegistryValueEditorDialog(const ValueType &type, QWidget *parent = nullptr);

    void setName(const QString &name);
    void setValue(const QVariant &value);
    QVariant value() const;

private slots:
    void on_radioButtonHex_toggled(bool checked);
    void on_radioButtonDec_toggled(bool checked);

private:
    ValueType _type;
};

#endif // REGISTRYVALUEEDITORDIALOG_H
