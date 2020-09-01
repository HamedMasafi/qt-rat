#include "abstractremotecli.h"

N_CLASS_IMPL(AbstractRemoteCLI)
{}

N_REMOTE_METHOD_IMPL(AbstractRemoteCLI, void, relive)
{
}

N_REMOTE_METHOD_IMPL(AbstractRemoteCLI, void, run, QString, command)
{
    Q_UNUSED(command)
}

N_REMOTE_METHOD_IMPL(AbstractRemoteCLI, void, cout, QString, out)
{
    emit outputRecived(out);
}

N_REMOTE_METHOD_IMPL(AbstractRemoteCLI, void, cerr, QString, err)
{
    emit errorRecived(err);
}
