#ifndef REMOTECLI_H
#define REMOTECLI_H

#include "../common/abstractremotecli.h"

class QProcess;
class RemoteCLI : public AbstractRemoteCLI
{
    Q_OBJECT
    QProcess *p;

public:
    RemoteCLI();

    void reliveSlot() override;
    void runSlot(QString command) override;
};

#endif // REMOTECLI_H
