#ifndef ABSTRACTREMOTECLI_H
#define ABSTRACTREMOTECLI_H

#include <SharedObject>

class AbstractRemoteCLI : public Neuron::SharedObject
{
    Q_OBJECT

public:
    N_CLASS_DECL(AbstractRemoteCLI)

    N_REMOTE_METHOD_DECL(void, relive)
    N_REMOTE_METHOD_DECL(void, run, QString, command)
    N_REMOTE_METHOD_DECL(void, cout, QString, out)
    N_REMOTE_METHOD_DECL(void, cerr, QString, err)

signals:
    void outputRecived(QString out);
    void errorRecived(QString out);
};

#endif // ABSTRACTREMOTECLI_H
