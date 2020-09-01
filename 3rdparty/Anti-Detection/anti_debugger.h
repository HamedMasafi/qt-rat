#pragma once
#include "Misc.h"
//#include "Anti-dbg-rce.h"
extern "C" int IsDebugged();


BOOL CheckForDebugger_CRDP(void)
{
	BOOL Present = FALSE;
	CheckRemoteDebuggerPresent(GetCurrentProcess(), &Present);
	return Present;
}

bool CheckForDebugger_NQIP(void)
{
	HMODULE hModNTDLL;
	FARPROC _NQIP;

	hModNTDLL = LoadLibrary(XorString("ntdll.dll"));
	_NQIP = GetProcAddress(hModNTDLL, XorString("NtQueryInformationProcess"));

	PVOID ProcessInfo;
	DWORD *ad_ = (DWORD*)&ProcessInfo;

	__asm
	{
		push NULL
		push 4
			push ad_
			push 0x07		
			push - 1			
			call _NQIP
	}

	if (ProcessInfo != 0)
	{
		return true;
	}
	return false;
}

bool CheckForDebugger_PEB(void)
{
	TIB* tib;
	__asm
	{
		push eax
		mov eax, fs:[18h]
			mov tib, eax
			pop eax
	}

	if (tib->Peb->BeingDebugged == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool MonitorDebugRegisters(void)
{
	CONTEXT* ct = 0;
	GetThreadContext(GetCurrentProcess(), ct);
	if (ct->Dr0 != 0 || ct->Dr1 != 0 || ct->Dr2 != 0 || ct->Dr3 != 0 || ct->Dr6 != 0 || ct->Dr7 != 0)
	{
		return true;
	}
	return false;
}

int set = 0;
bool done = false;

void EHCheck()
{
	__try
	{
		__asm
		{
			int 3h
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		set = 1;
	}
	done = true;
}

bool CheckForDebugger_EH(void)
{
	HANDLE EHThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)EHCheck, 0, 0, 0);
	if (done == true)
	{
		if (set != 1)
		{
			return true;
		}
	}
	return false;
}


int IsRunningUnderdebugger()
{
	
	//Antidebugg_TLS();
	if (IsDebugged() == FALSE)
	{
		if (CheckForDebugger_CRDP() == FALSE)
		{
			if (CheckForDebugger_NQIP() == FALSE)
			{
				if (CheckForDebugger_PEB() == FALSE)
				{
					//if (MonitorDebugRegisters() == FALSE)
					{
						if (CheckForDebugger_EH() == FALSE)
						{
							
							return FALSE;
						}
					}
				}
			}
		}
	}

	
	return TRUE;
}


