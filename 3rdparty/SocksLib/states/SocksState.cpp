#include "SocksState.h"

SocksState::SocksState(SocksConnection *parent) :
    QObject(parent)
{
    _parent = parent;
}

SocksState::~SocksState()
{
}

void SocksState::handleSetAsNewState()
{
    //default does nothing
}
