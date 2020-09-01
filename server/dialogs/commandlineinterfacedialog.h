#ifndef COMMANDLINEINTERFACEDIALOG_H
#define COMMANDLINEINTERFACEDIALOG_H

#include "ui_commandlineinterfacedialog.h"
#include "../core/controllerbase.h"

class AbstractRemoteCLI;
class CommandLineInterfaceDialog : public ControllerBase, private Ui::CommandLineInterfaceDialog
{
    Q_OBJECT
    QString _path;
    AbstractRemoteCLI *_cli;

public:
    explicit CommandLineInterfaceDialog(AbstractClient *client, QWidget *parent = nullptr);
    explicit CommandLineInterfaceDialog(AbstractClient *client, QString command, QWidget *parent = nullptr);

protected:
    void changeEvent(QEvent *e);
private slots:
    void on_console_execCommand(const QString &command);
    void on_console_onCommand(const QString &command);
};

#endif // COMMANDLINEINTERFACEDIALOG_H
