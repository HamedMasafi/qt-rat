#pragma once
#include "utils.h"
//#define DEBUG
#define VMWARE
//#define VBOX

extern "C" int IsVirtualMachineExist();



BOOL DetectVMBaseOnPCItoBridgeSlots(DWORD dwMaxNumber)
{
	DWORD dwHardInfoSize = 0;
	DWORD i;
	DWORD dwPCISlot = 0;
	PHARDWAREINFO pHardInfo = NULL;

	HardwareInformation(pHardInfo, &dwHardInfoSize);
	pHardInfo = (PHARDWAREINFO)GlobalAlloc(GMEM_ZEROINIT, sizeof(HARDWAREINFO) * (dwHardInfoSize + 1));
	HardwareInformation(pHardInfo, &dwHardInfoSize);
	dwHardInfoSize += 1;

	for (i = 0; i <= dwHardInfoSize; i++)
	{
		if (strstr(pHardInfo[i].szHardwareName, "PCI-to-PCI bridge"))
			dwPCISlot++;
	}

#ifdef DEBUG
	
#endif


	if (dwPCISlot >= dwMaxNumber)
	{
		GlobalFree(pHardInfo);
		return TRUE;
	}

	GlobalFree(pHardInfo);
	return FALSE;
}

BOOL DetectVMBaseOnDiskInstanceID(CHAR *szDiskInstanceIDString)
{
	CHAR *szInstanceID = NULL;
	DWORD dwInstIDSize = 0;
	DiskInstanceID(szInstanceID, &dwInstIDSize);
	szInstanceID = (CHAR *)GlobalAlloc(GMEM_ZEROINIT, dwInstIDSize * 2);
	if (!DiskInstanceID(szInstanceID, &dwInstIDSize))
		return FALSE;

#ifdef DEBUG
	
#endif

	if (strstr(szInstanceID, szDiskInstanceIDString))
	{
		GlobalFree((HGLOBAL)szInstanceID);
		return TRUE;
	}
	GlobalFree(szInstanceID);
	return FALSE;
}

BOOL DetectVMWareBaseOnIDTValue()
{
	unsigned char	idtr[6];
	unsigned long	idt = 0;

	_asm sidt idtr
	idt = *((unsigned long *)&idtr[2]);

#ifdef DEBUG
	
#endif

	if ((idt >> 24) == 0xff)
		return TRUE;

	return FALSE;
}

BOOL DetectVMWareBaseOnLDTValue()
{
	unsigned char   ldtr[5] = "\xef\xbe\xad\xde";
	unsigned long   ldt = 0;

	_asm sldt ldtr
	ldt = *((unsigned long *)&ldtr[0]);

#ifdef DEBUG
	
#endif

	if (ldt != 0xdead0000)
		return TRUE;

	return FALSE;
}

BOOL DetectVMWareBaseOnGDTValue()
{
	unsigned char   gdtr[6];
	unsigned long   gdt = 0;

	_asm sgdt gdtr
	gdt = *((unsigned long *)&gdtr[2]);

#ifdef DEBUG
	
#endif

	if ((gdt >> 24) == 0xff)
		return TRUE;

	return FALSE;
}

BOOL DetectVMWareBaseOnSTRValue()
{
	unsigned char	mem[4] = { 0, 0, 0, 0 };

	__asm str mem;

#ifdef DEBUG
	
#endif

	if ((mem[0] == 0x00) &&
		(mem[1] == 0x40))
		return TRUE;

	return FALSE;
}

BOOL DetectVMWareBaseOnIOPortVMVersion(PDWORD pdwVesionNumber)
{
	unsigned int a, b;

	__try {
		__asm {

			// save register values on the stack
			push eax
				push ebx
				push ecx
				push edx

				// perform fingerprint
				mov eax, 'VMXh'	    // VMware magic value (0x564D5868)
				mov ecx, 0Ah		// special version cmd (0x0a)
				mov dx, 'VX'		// special VMware I/O port (0x5658)

				in eax, dx			// special I/O cmd

				mov a, ebx			// data 
				mov b, ecx			// data	(eax gets also modified but will not be evaluated)

									// restore register values from the stack
				pop edx
				pop ecx
				pop ebx
				pop eax
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {}

#ifdef DEBUG
	printf("\n [ a=%x ; b=%d ]\n\n", a, b);
#endif

	if (a == 'VMXh')
	{
		*pdwVesionNumber = b;
		return TRUE;
	}

	return FALSE;
}

BOOL DetectVMWareBaseOnIOPortVMMemorySize()
{
	unsigned int a = 0;

	__try {
		__asm {

			// save register values on the stack
			push eax
				push ebx
				push ecx
				push edx

				// perform fingerprint
				mov eax, 'VMXh'		// VMware magic value (0x564D5868)
				mov ecx, 14h		// get memory size command (0x14)
				mov dx, 'VX'		// special VMware I/O port (0x5658)

				in eax, dx			// special I/O cmd

				mov a, eax			// data 

									// restore register values from the stack
				pop edx
				pop ecx
				pop ebx
				pop eax
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {}

	if (a > 0)
		return TRUE;

	return FALSE;
}

BOOL DetectVMWareBaseOnSMSWInstruction()
{

	int rc = 0;
	unsigned int reax = 0;

	__asm
	{
		mov eax, 0xCCCCCCCC;  // This is the magic value
		smsw eax;
		mov DWORD PTR[reax], eax;
	}

#ifdef DEBUG
	printf("MSW: %2.2x%2.2x%2.2x%2.2x\n", (reax >> 24) & 0xFF, (reax >> 16) & 0xFF, (reax >> 8) & 0xFF, reax & 0xFF);
#endif

	// If the high order bits are still 0xCC, then we are in a VMWare session
	// with emulation turned off.
	if ((((reax >> 24) & 0xFF) == 0xcc) && (((reax >> 16) & 0xFF) == 0xcc))
		return TRUE;

	return FALSE;
}

BOOL DetectVMBaseOnServiceName(CHAR *szServiceName)
{
	LPENUM_SERVICE_STATUS_PROCESS enumser = NULL;
	DWORD dwServiceNumbers = 0;
	DWORD dwRequeirSize = 0;
	DWORD i;

	EnumerateServices(&enumser, &dwServiceNumbers, &dwRequeirSize);
	enumser = (LPENUM_SERVICE_STATUS_PROCESS)GlobalAlloc(GMEM_ZEROINIT, sizeof(ENUM_SERVICE_STATUS_PROCESS) * (dwRequeirSize + 1));
	dwRequeirSize += 1;

	if (EnumerateServices(&enumser, &dwServiceNumbers, &dwRequeirSize))
	{
		for (i = 0; i < dwServiceNumbers; i++)
		{
			if (strstr(enumser[i].lpServiceName, szServiceName))
				return TRUE;
		}
	}

	GlobalFree(enumser);
	return FALSE;
}

BOOL DetectVMBaseOnMacAddressRange()
{
	PMACINFO pMInfo = NULL;
	DWORD NicNum;
	DWORD i = 0;

	GetNICMacAddress(pMInfo, &NicNum);
	pMInfo = (PMACINFO)GlobalAlloc(GMEM_ZEROINIT, sizeof(MACINFO) * (NicNum + 1));

	if (!GetNICMacAddress(pMInfo, &NicNum))
		return FALSE;

	// You are not supposed to understand this line
	while (pMInfo[i].bUsed)
	{
		if ((pMInfo[i].MacAddress[0] == 0x00 &&
			pMInfo[i].MacAddress[1] == 0x05 &&
			pMInfo[i].MacAddress[2] == 0x69) ||

			(pMInfo[i].MacAddress[0] == 0x00 &&
				pMInfo[i].MacAddress[1] == 0x0C &&
				pMInfo[i].MacAddress[2] == 0x29) ||

			(pMInfo[i].MacAddress[0] == 0x00 &&
				pMInfo[i].MacAddress[1] == 0x50 &&
				pMInfo[i].MacAddress[2] == 0x56))
		{
			return TRUE;
		}

		i++;
	}

	return FALSE;
}

BOOL DetectVMBaseOnProcessName(CHAR * szProccessName)
{
	PPROCESSINFO pProcInfo = NULL;
	DWORD dwSize = 0;
	DWORD i = 0;

	GetProcessList(pProcInfo, &dwSize);

	pProcInfo = (PPROCESSINFO)GlobalAlloc(GMEM_ZEROINIT, sizeof(PROCESSINFO) * (dwSize + 1));
	dwSize *= 2;

	if (GetProcessList(pProcInfo, &dwSize))
	{
		while (pProcInfo[i].bUsed)
		{
			if (strstr(pProcInfo[i].szExeName, szProccessName))
			{
				GlobalFree(pProcInfo);
				return TRUE;
			}

			i++;
		}
	}

	GlobalFree(pProcInfo);
	return FALSE;
}

BOOL DetectVMBaseOnTimeDiffrence(DWORD dwMaxTime)
{
	DWORD dwIterTime = 0;
	FLOAT diff;

	if (!JunkIteration(50, &dwIterTime))
		return FALSE;

	diff = ((dwIterTime / 1000) % 60);

#ifdef DEBUG
	printf("Time Diffrence : %d\n", (DWORD)diff);
#endif

	if ((DWORD)diff > dwMaxTime)
		return TRUE;
	else
		return FALSE;
}

BOOL DetectVMBaseOnHardwareName(CHAR * szHardwareName)
{
	CHAR *szHardwareInfo;
	DWORD dwHardwareInfoSize = 0;

	EnumHardwareInfo(NULL, &dwHardwareInfoSize);

	szHardwareInfo = (CHAR *)malloc(dwHardwareInfoSize + 256);
	SecureZeroMemory(szHardwareInfo, (dwHardwareInfoSize + 256));
	EnumHardwareInfo(szHardwareInfo, &dwHardwareInfoSize);

	if (strstr(szHardwareInfo, szHardwareName))
	{
		free(szHardwareInfo);
		return TRUE;
	}

	free(szHardwareInfo);
	return FALSE;
}


BOOL DetectVMBaseOnDeviceObjectExistence(CHAR *szDeviceObjectName)
{
	if (CreateFile(szDeviceObjectName,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL) != INVALID_HANDLE_VALUE)
		return TRUE;
	return FALSE;
}


BOOL DetectVMBaseOnRegistryKeyExistence(CHAR *szRegKey)
{
	LONG lResult;
	HKEY hKey;
	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		szRegKey,// "HARDWARE\\ACPI\\DSDT\\VBOX__"
		0,
		KEY_READ,
		&hKey);

	if (lResult != ERROR_SUCCESS)
		return FALSE;

	return TRUE;
}

BOOL DetectVMBaseOnHarddiskModelDesciptions(CHAR *szHarddiskModelDesciptions)
{
	CHAR *szModelDesciption = GetFirstPhysicalDriveModelNames();
#ifdef DEBUG
	printf("Hard Disk Model Desciptions : %s\n", szModelDesciption);
#endif

	if (szModelDesciption != NULL)
		if (strstr(szModelDesciption, szHarddiskModelDesciptions))
			return TRUE;

	return FALSE;
}

BOOL DetectVMBaseOnBiosSerialString(CHAR *szBiosSerial)
{
	HRESULT hres;
	CHAR szBiosInfo[256];

	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres))
	{
		return FALSE;              // Program has failed.
	}

	hres = CoInitializeSecurity(NULL,
		-1,      // COM negotiates service                  
		NULL,    // Authentication services
		NULL,    // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,    // authentication
		RPC_C_IMP_LEVEL_IMPERSONATE,  // Impersonation
		NULL,             // Authentication info 
		EOAC_NONE,        // Additional capabilities
		NULL              // Reserved
		);


	if (FAILED(hres))
	{
		CoUninitialize();
		return FALSE;          // Program has failed.
	}

	if (FAILED(hres = RetrieveBIOSInfo(szBiosInfo)))
	{
		CoUninitialize();
		return FALSE;
	}
#ifdef DEBUG
	printf("Bios Serial String : %s\n", szBiosInfo);
#endif
	if (strstr(szBiosInfo, szBiosSerial))
		return TRUE;

	CoUninitialize();
	return FALSE;
}




BOOL check_vmware()
{
//	if (IsVirtualMachineExist() == false)
	//{
		//if (DetectVMBaseOnDiskInstanceID("VMWARE") == false)
		//{
			if (DetectVMBaseOnPCItoBridgeSlots(20) == false)
			{
				if (DetectVMWareBaseOnIDTValue() == false)
				{
					if (DetectVMWareBaseOnLDTValue() == false)
					{
						if (DetectVMWareBaseOnGDTValue() == false)
						{
							if (DetectVMWareBaseOnSTRValue() == false)
							{
								if (DetectVMWareBaseOnIOPortVMMemorySize() == false)
								{
									if (DetectVMWareBaseOnSMSWInstruction() == false)
									{
										if (DetectVMBaseOnServiceName("vmci") == false)
										{
											if (DetectVMBaseOnServiceName("vmdebug") == false)
											{
												if (DetectVMBaseOnServiceName("vmhgfs") == false)
												{
													if (DetectVMBaseOnServiceName("vmmouse") == false)
													{
														if (DetectVMBaseOnServiceName("vmscsi") == false)
														{
															if (DetectVMBaseOnServiceName("VMTools") == false)
															{
																if (DetectVMBaseOnServiceName("VMUpgradeHelper") == false)
																{
																	if (DetectVMBaseOnServiceName("VMware Physical Disk Helper Service") == false)
																	{
																		if (DetectVMBaseOnServiceName("vmxnet") == false)
																		{
																			if (DetectVMBaseOnServiceName("vmx_svga") == false)
																			{
																				if (DetectVMBaseOnMacAddressRange() == false)
																				{
																					if (DetectVMBaseOnProcessName("vmtoolsd") == false)
																					{
																						if (DetectVMBaseOnProcessName("VMwareUser") == false)
																						{
																							if (DetectVMBaseOnProcessName("VMwareTray") == false)
																							{
																								if (DetectVMBaseOnTimeDiffrence(5) == false)
																								{
																									if (DetectVMBaseOnBiosSerialString("VMware") == false)
																									{
																										//cout << "Vmware Not Found." << endl;
																										return false;
																									}
																								}
																							}
																						}
																					}
																				}
																			}
																		}
																	}
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		//}
	//}

	//cout << "Vmware Found" << endl;
	return true;
}

BOOL check_virtualbox()
{
	//if (DetectVMBaseOnDiskInstanceID("VBOX") == false)
	{
		if (DetectVMBaseOnHardwareName("VirtualBox Graphics")  == false)
		{
			if (DetectVMBaseOnHardwareName("VirtualBox Device") == false)
			{
				if (DetectVMBaseOnDeviceObjectExistence("\\\\.\\VBoxMiniRdrDN") == false)
				{
					if (DetectVMBaseOnRegistryKeyExistence("HARDWARE\\ACPI\\DSDT\\VBOX__") == false)
					{
						if (DetectVMBaseOnHarddiskModelDesciptions("VBOX") == false)
						{
							if (DetectVMBaseOnProcessName("VBoxTray") == false)
							{
								if (DetectVMBaseOnProcessName("VBoxService") == false)
								{
									if (DetectVMBaseOnServiceName("VBoxGuest") == false)
									{
										if (DetectVMBaseOnServiceName("VBoxMouse") == false)
										{
											if (DetectVMBaseOnServiceName("VBoxService") == false)
											{
												if (DetectVMBaseOnServiceName("VBoxSF") == false)
												{
													if (DetectVMBaseOnServiceName("VBoxVideo") == false)
													{
														if (DetectVMBaseOnTimeDiffrence(5) == false)
														{
															//cout << "Virtualbox Not Found." << endl;
															return false;
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	//cout << "Virtualbox Found." << endl;
	return true;
}


int IsRunningUnderVirtualMachine()
{
	if (check_vmware() || check_virtualbox())
	{
		//cout << "Virtual Machine Found." << endl;
		return true;
	}
	else
	{
		//cout << "Virtual Machine Not Found." << endl;
		return false;
	}


}



///////////////////////////////////// VIRTUALBOX STUFF BEGIN /////////////////////////////////////






//
//printf("[+] Detecting VMWare Base on I/O Port Get Version.\n");
//if (DetectVMWareBaseOnIOPortVMVersion(&dwVMWareVersion))
//switch (dwVMWareVersion)
//{
//case 1:
//	printf("[!] VMware Express Found!\n\n");
//	break;
//case 2:
//	printf("[!] VMware ESX Found!\n\n");
//	break;
//case 3:
//	printf("[!] VMware GSX Found!\n\n");
//	break;
//case 4:
//	printf("[!] VMware Workstation Found!\n\n");
//	break;
//default:
//	printf("[!] VMware Uknown Version Found!\n\n");
//}
//else
//printf("[!] VMware Not Found!\n\n");



