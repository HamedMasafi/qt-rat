#include "remotecli.h"

#include <QProcess>
#include <QDebug>

RemoteCLI::RemoteCLI() : AbstractRemoteCLI()
{

}

void RemoteCLI::reliveSlot()
{
    if (!p) {
        p = new QProcess(this);
        QString shell;
#ifdef Q_OS_WIN
        shell = "cmd.exe";
#else
        shell = qEnvironmentVariable("SHELL");
        if (shell.isEmpty())
            shell = "/bin/sh";
#endif
        p->setProgram(shell);
        connect(p, &QProcess::readyReadStandardOutput, [this](){
            auto buffer = p->readAllStandardOutput();
            qDebug() << "buffer="<<buffer;
            cout(buffer);
        });

        connect(p, &QProcess::readyReadStandardError, [this](){
            auto buffer = p->readAllStandardOutput();
            qDebug() << "buffer="<<buffer;
            cout(buffer);
        });
    }

    if (p->state() != QProcess::Running)
        p->start();
}

void RemoteCLI::runSlot(QString command)
{
    if (p->state() == QProcess::Running)
        p->write(command.toLocal8Bit() + "\n");
}
