#ifndef CONTROLLERBASE_H
#define CONTROLLERBASE_H

#include <QDialog>

class AbstractClient;
class ControllerBase : public QDialog
{
    Q_OBJECT
    AbstractClient *_client;
    bool _busy;

public:
    ControllerBase(QWidget *parent = nullptr);
    ControllerBase(AbstractClient *client, QWidget *parent = nullptr);

    AbstractClient *client() const;
    void setClient(AbstractClient *client);

private slots:
    void client_destroyed(QObject * = nullptr);

    // QWidget interface
protected:
    void setBusy(const bool &busy);
    void closeEvent(QCloseEvent *event);
};

#endif // CONTROLLERBASE_H
