#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <SharedObject>

class TaskManager : public Neuron::SharedObject
{
public:
    N_CLASS_DECL(TaskManager)

    N_REMOTE_METHOD_DECL(QVariant, tasks)
};

#endif // TASKMANAGER_H
