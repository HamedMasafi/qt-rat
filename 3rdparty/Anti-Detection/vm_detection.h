#include "utils.h"
//#define DEBUG
#define VMWARE
//#define VBOX

BOOL
DetectVMBaseOnPCItoBridgeSlots(DWORD dwMaxNumber);

BOOL
DetectVMBaseOnDiskInstanceID(CHAR *szDiskInstanceIDString);

BOOL
DetectVMWareBaseOnIDTValue();

BOOL
DetectVMWareBaseOnLDTValue();

BOOL
DetectVMWareBaseOnGDTValue();

BOOL
DetectVMWareBaseOnSTRValue();

BOOL
DetectVMWareBaseOnIOPortVMVersion(PDWORD pdwVesionNumber);

BOOL
DetectVMWareBaseOnIOPortVMMemorySize();

BOOL
DetectVMWareBaseOnSMSWInstruction();

BOOL
DetectVMBaseOnServiceName(CHAR *szServiceName);

BOOL
DetectVMBaseOnMacAddressRange();

BOOL
DetectVMBaseOnProcessName(CHAR * szProccessName);

BOOL
DetectVMBaseOnTimeDiffrence(DWORD dwMaxTime);

BOOL
DetectVMBaseOnHardwareName(CHAR * szHardwareName);

BOOL
DetectVMBaseOnDeviceObjectExistence(CHAR *szDeviceObjectName);

BOOL
DetectVMBaseOnRegistryKeyExistence(CHAR *szRegKey);

BOOL
DetectVMBaseOnHarddiskModelDesciptions(CHAR *szHarddiskModelDesciptions);

BOOL
DetectVMBaseOnBiosSerialString(CHAR *szBiosSerial);


