#ifndef ANTIDEBUG_H
#define ANTIDEBUG_H


#include <windows.h>
#include "Misc.h"

bool CheckForDebugger_CRDP(void);
bool CheckForDebugger_NQIP(void);
bool CheckForDebugger_PEB(void);
bool CheckForDebugger_EH(void);

bool MonitorDebugRegisters(void);

#endif