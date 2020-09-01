#include "taskmanager.h"

#include <QProcess>

N_CLASS_IMPL(TaskManager) {}

N_REMOTE_METHOD_IMPL(TaskManager, QVariant, tasks)
{
    QVariantList list;
#ifdef Q_OS_LINUX
    auto process = new QProcess;
    process->setProgram("ps");
    process->setArguments(QStringList() << "axo"
                                        << "pid,%cpu,comm");
    process->start();
    process->waitForFinished();

    QString psOutput = process->readAllStandardOutput();
    QStringList processList = psOutput.split('\n');
    processList.removeFirst();
    processList.removeLast();


    foreach (QString p, processList) {
        QVariantMap map;
        QStringList parts = p.split(" ");
        if (parts.count() != 3)
            continue;
        map.insert("pid", parts.at(0));
        map.insert("cpu", parts.at(1));
        map.insert("cmd", parts.at(2));

        list.append(map);
    }
#endif
    return list;
}
