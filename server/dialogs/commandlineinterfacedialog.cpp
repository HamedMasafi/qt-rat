#include "commandlineinterfacedialog.h"

#include "../common/abstractclient.h"
#include "../common/abstractremotecli.h"
#include <QDebug>
#include <QHostAddress>

CommandLineInterfaceDialog::CommandLineInterfaceDialog(AbstractClient *client, QWidget *parent)
    : ControllerBase(client, parent)
    , _path("/")
{
    setupUi(this);
//    console->setCmdColor(Qt::white);
//    console->setErrColor(Qt::red);
    console->setTextColor(Qt::white);

//    console->reset("Connected to " + client->hub()->localAddress().toString());
    console->setPrompt("/> ");

    _cli = new AbstractRemoteCLI(this);
    _cli->setHub(client->hub());
//    client->hub()->attachSharedObject(cli);
    connect(_cli, &AbstractRemoteCLI::outputRecived, [this](QString o){
        console->printOutput(o);
    });
    connect(_cli, &AbstractRemoteCLI::errorRecived, [this](QString o){
        console->printError(o);
    });
    _cli->relive();
}

CommandLineInterfaceDialog::CommandLineInterfaceDialog(AbstractClient *client,
                                                       QString command,
                                                       QWidget *parent)
    : ControllerBase(client, parent)
    , _path("/")
{
    setupUi(this);
//    console->setCmdColor(Qt::white);
//    console->setErrColor(Qt::red);
    console->setTextColor(Qt::white);

//    console->reset("Connected to " + client->hub()->localAddress().toString());
    console->setPrompt("/> ");

    on_console_execCommand(command);
}

void CommandLineInterfaceDialog::changeEvent(QEvent *e)
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

void CommandLineInterfaceDialog::on_console_execCommand(const QString &command)
{
    qDebug() << "going to exesute command: " << command;
    if (!client())
        return;
    _cli->run(command);
//    client()->execute(command, _path)->then([this, command](QVariant o) {
//        auto map = o.toMap();

//        if (map.contains("path")) {
//            if (map.value("ok").toBool()) {
//                _path = map.value("path").toString();
//                console->printCommandExecutionResults("path changed!",
//                                                      QConsole::ResultType::Complete);
//                console->setPrompt(QString("$:%1> %2").arg(_path, command));
//            } else {
//                console->printCommandExecutionResults(
//                "Unable to find path " + map.value("path").toString(),
//                    QConsole::ResultType::Error);
//            }
//        }

//        if (map.contains("code")) {
//            auto code = map.value("code").toInt();
//            if (code == -2)
//                console->printCommandExecutionResults(tr("Process cannot be started"),
//                                                      QConsole::ResultType::Error);

//            if (code == -1)
//                console->printCommandExecutionResults(tr("Process crashes"),
//                                                      QConsole::ResultType::Error);

//            if (code > 0)
//                console->printCommandExecutionResults("Process exited with code: " + QString::number(code),
//                                                      QConsole::ResultType::Error);

//            if (map.contains("out") && !map.value("out").toString().isEmpty())
//                console->printCommandExecutionResults(map.value("out").toString());
//            if (map.contains("err") && !map.value("err").toString().isEmpty())
//                console->printCommandExecutionResults(map.value("err").toString(),
//                                                      QConsole::ResultType::Error);

//        }
////        console->displayPrompt();
    //    });
}

void CommandLineInterfaceDialog::on_console_onCommand(const QString &command)
{
    if (!client())
        return;
    _cli->run(command);
}
