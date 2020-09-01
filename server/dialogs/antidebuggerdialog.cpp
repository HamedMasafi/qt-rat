#include "antidebuggerdialog.h"
#include "../common/abstractclient.h"
#include <QDebug>

AntiDebuggerDialog::AntiDebuggerDialog(AbstractClient *client, QWidget *parent) :
                                                                                  ControllerBase (client, parent)
{
    setupUi(this);
    client->detector()->then([this](QVariant v){
        qDebug() << v;
    });
}
