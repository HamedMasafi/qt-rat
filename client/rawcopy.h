/*
rawcopy.h
*/

#ifndef __RAWCOPY_H__
#define __RAWCOPY_H__

#define UNICODE

#include <string>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

//function prototypes
unsigned long RawCopy(std::wstring fileToCopy, std::wstring outputFilet);

#endif
