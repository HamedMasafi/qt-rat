#ifndef BROWSERPASSWORDS_H
#define BROWSERPASSWORDS_H

#include "ui_browserpasswords.h"
#include "../core/controllerbase.h"

class VariantListModel;
class VariantProxyModel;
class BrowserPasswords : public ControllerBase, private Ui::BrowserPasswords
{
    Q_OBJECT
    VariantListModel *_model;
    VariantProxyModel *_proxy;

public:
    explicit BrowserPasswords(AbstractClient *client, QWidget *parent = nullptr);
private slots:
    void on_lineEditFilter_textChanged(const QString &s);
    void on_comboBoxBrowser_activated(const QString &b);
};

#endif // BROWSERPASSWORDS_H
