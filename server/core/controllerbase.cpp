#include "controllerbase.h"

#include "../common/abstractclient.h"
#include <QCloseEvent>
#include <QMessageBox>
#include <QObject>

ControllerBase::ControllerBase(QWidget *parent) : QDialog(parent), _busy(false)
{

}

ControllerBase::ControllerBase(AbstractClient *client, QWidget *parent) :
    QDialog(parent), _client(client), _busy(false)
{
    connect(client, &QObject::destroyed, this, &ControllerBase::client_destroyed);
}

AbstractClient *ControllerBase::client() const
{
    return _client;
}

void ControllerBase::setClient(AbstractClient *client)
{
    _client = client;
}

void ControllerBase::client_destroyed(QObject *)
{
    _client = nullptr;
    QMessageBox::warning(this, "rat",
                         "Client has disconnected.");
    _busy = false;
    reject();
}

void ControllerBase::setBusy(const bool &busy)
{
    _busy = busy;
}

void ControllerBase::closeEvent(QCloseEvent *event)
{
    if (_busy) {
        if (!windowTitle().endsWith("(busy)"))
            setWindowTitle(windowTitle() + " (busy)");
        event->ignore();
    }
}
