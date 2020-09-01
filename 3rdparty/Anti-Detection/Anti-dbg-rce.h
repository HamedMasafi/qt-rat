#include <stdio.h>
#include <Windows.h>

#pragma comment(lib,"ntdll.lib")

#pragma comment(linker,"/include:__tls_used") // This will cause the linker to create the TLS directory
#pragma section(".CRT$XLB",read) // Create a new section

extern "C" NTSTATUS NTAPI NtQueryInformationProcess(HANDLE hProcess, ULONG InfoClass, PVOID Buffer, ULONG Length, PULONG ReturnLength);

#define NtCurrentProcess() (HANDLE)-1

// The TLS callback is called before the process entry point executes, and is executed before the debugger breaks
// This allows you to perform anti-debugging checks before the debugger can do anything
// Therefore, TLS callback is a very powerful anti-debugging technique

void WINAPI TlsCallback(PVOID Module, DWORD Reason, PVOID Context)
{
	PBOOLEAN BeingDebugged = (PBOOLEAN)__readfsdword(0x30) + 2;
	HANDLE DebugPort = NULL;

	if (*BeingDebugged) // Read the PEB
	{
		MessageBox(NULL, "Debugger detected!", "TLS callback", MB_ICONSTOP);
	}

	else
	{
		MessageBox(NULL, "No debugger detected", "TLS callback", MB_ICONINFORMATION);
	}

	// Another check

	if (!NtQueryInformationProcess(
		NtCurrentProcess(),
		7, // ProcessDebugPort
		&DebugPort, // If debugger is present, it will be set to -1 | Otherwise, it is set to NULL
		sizeof(HANDLE),
		NULL))
	{
		if (DebugPort)
		{
			MessageBox(NULL, "Debugger detected!", "TLS callback", MB_ICONSTOP);
			ExitProcess(0);
		}

		else
		{
			MessageBox(NULL, "No debugger detected", "TLS callback", MB_ICONINFORMATION);
		}
	}
}

__declspec(allocate(".CRT$XLB")) PIMAGE_TLS_CALLBACK CallbackAddress[] = { TlsCallback,NULL }; // Put the TLS callback address into a null terminated array of the .CRT$XLB section

																							   // The entry point is executed after the TLS callback

int TLS_callback()
{
	//printf("Hello world");
	//getchar();

	return 0;
}