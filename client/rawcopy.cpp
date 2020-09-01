/*
rawcopy.cpp
*/

#define UNICODE

#include "rawcopy.h"
#include <Windows.h>
#include <winioctl.h>
#include <string>
#include <iostream>

using std::wstring;
using std::wcout;


// NTFS file record header
typedef struct {
    ULONG Type;			// Magic number 'FILE'
    USHORT UsaOffset;	// Offset to the update sequence
    USHORT UsaCount;	// Size in words of Update Sequence Number & Array (S)
    ULONGLONG Lsn;		// $LogFile Sequence Number (LSN)
} NTFS_RECORD_HEADER, *PNTFS_RECORD_HEADER;

// MFT File record header
typedef struct {
    NTFS_RECORD_HEADER RecHdr;
    USHORT SequenceNumber;		// Sequence number
    USHORT LinkCount;			// Hard link count
    USHORT AttributeOffset;		// Offset to the first Attribute
    USHORT Flags;				// Flags
    ULONG BytesInUse;			// Real size of te FILE record
    ULONG BytesAllocated;		// Allocated size of the FILE record
    ULONGLONG BaseFileRecord;	// Reference to the base FILE record
    USHORT NextAttributeNumber;	// Next Attribute ID
    USHORT Pading;				// Align to 4 byte boundary (XP)
    ULONG MFTRecordNumber;		// Number of this MFT Record (XP)
    USHORT UpdateSeqNum;
} FILE_RECORD_HEADER, *PFILE_RECORD_HEADER;

// MFT Attribute type enum
typedef enum {
    AttributeStandardInformation = 0x10,
    AttributeAttributeList = 0x20,
    AttributeFileName = 0x30,
    AttributeObjectId = 0x40,
    AttributeSecurityDescriptor = 0x50,
    AttributeVolumeName = 0x60,
    AttributeVolumeInformation = 0x70,
    AttributeData = 0x80,
    AttributeIndexRoot = 0x90,
    AttributeIndexAllocation = 0xA0,
    AttributeBitmap = 0xB0,
    AttributeReparsePoint = 0xC0,
    AttributeEAInformation = 0xD0,
    AttributeEA = 0xE0,
    AttributePropertySet = 0xF0,
    AttributreLoggedUtilityStream = 0x100
} ATTRIBUTE_TYPE, *PATTRIBUTE_TYPE;

// MFT Attribute struct
typedef struct {
    ATTRIBUTE_TYPE AttributeType;
    ULONG Length;			// Length of the attribute
    BOOLEAN Nonresident;		// boolean indication of a nonresident file
    UCHAR NameLength;		// lenght of the file name
    USHORT NameOffset;		// starting position of the filename
    USHORT Flags;			// flags
    USHORT AttributeNumber;	// attribute number
} ATTRIBUTE, *PATTRIBUTE;

// MFT resident file struct
typedef struct {
    ATTRIBUTE Attribute;
    ULONG ValueLength;	// length of the attribute value
    USHORT ValueOffset;	// location of the attribute
    UCHAR Flags;		// flags
} RESIDENT_ATTRIBUTE, *PRESIDENT_ATTRIBUTE;

//takes a file and uses direct io access to copy it to a specificed location
unsigned long RawCopy(wstring src, wstring dst) {
    unsigned long result = ERROR_SUCCESS;	// result of the the operation
    //get a handle to the file in question
    HANDLE hFile = CreateFile(src.c_str(), FILE_READ_ATTRIBUTES, (FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE), NULL, OPEN_EXISTING, 0, NULL);
    //as long as the handle is good
    if(hFile != INVALID_HANDLE_VALUE) {
        wstring szDrive = src.substr(0, 2);	//get just the drive i.e. C:
        DWORD dwSectorPerCluster;			//holds the number of secotrs per cluster i.e. 8
        DWORD dwBytesPerSector;				//holds the size of a sector i.e. 512
        //Get the disk free space will return the number of secotrs and the size of sectors
        if(GetDiskFreeSpace(szDrive.c_str(), &dwSectorPerCluster, &dwBytesPerSector, NULL, NULL)) {
            LARGE_INTEGER liFileSize;																				//holds the size of the file we want to copy
            DWORD dwRead, dwWritten;																				//how many bytes were read, and written from disk
            DWORD dwCluserSize = (dwBytesPerSector * dwSectorPerCluster);											//determine the size of a cluster
            liFileSize.LowPart = GetFileSize(hFile, (LPDWORD)&liFileSize.HighPart);									//determine the size of the file in question
            DWORD dwClusters = (liFileSize.QuadPart / dwCluserSize);												//determine the number of clusters that the file is spread accross
            DWORD dwPointsSize = sizeof(RETRIEVAL_POINTERS_BUFFER) + (dwClusters * (sizeof(LARGE_INTEGER) * 2));	//determine the size of retreivla pointers buffer needed
            PRETRIEVAL_POINTERS_BUFFER pPoints = (PRETRIEVAL_POINTERS_BUFFER) new BYTE[dwPointsSize];				//allocate memory
            STARTING_VCN_INPUT_BUFFER vcnStart = {0};																//initialzie the starting vcn input buffer (set to 0 for we want the first location every time)

            //call to device io control to get the retreival pointers which is the sector(s) on disk where the file is located
            if(DeviceIoControl(hFile, FSCTL_GET_RETRIEVAL_POINTERS, &vcnStart, sizeof(vcnStart), pPoints, dwPointsSize, &dwWritten, NULL)) {
                szDrive = L"\\\\.\\" + szDrive;	//adjust the drive location with then UNC path

                //get a handle to the drive location
                HANDLE hDrive = CreateFile(szDrive.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
                //as long as the handle is good
                if(hDrive != INVALID_HANDLE_VALUE) {
                    //get a handle to the copy to file to create it
                    HANDLE hDst = CreateFile(dst.c_str(), GENERIC_WRITE, 0, NULL, CREATE_NEW, 0, NULL);

                    //as long as the handle is good
                    if(hDst != INVALID_HANDLE_VALUE) {
                        //set the copy to file pointer to the size of the the file we want to copy
                        if(SetFilePointer(hDst, liFileSize.LowPart, &liFileSize.HighPart, FILE_BEGIN) != INVALID_SET_FILE_POINTER) {
                            //mark that location as the end of the file
                            if(SetEndOfFile(hDst)) {
                                LPBYTE lpCluser = new BYTE[dwCluserSize];		//allocated memory for each cluster
                                LARGE_INTEGER vcnPrev = pPoints->StartingVcn;	//get the location of the cluster on disk

                                //loop over each cluster location
                                for(DWORD dwExtent = 0; dwExtent < pPoints->ExtentCount; dwExtent++) {
                                    LARGE_INTEGER liSrcPos;																		//the source position on disk
                                    LARGE_INTEGER liDstPos;																		//the destination postion on disk
                                    DWORD dwLength = (DWORD)(pPoints->Extents[dwExtent].NextVcn.QuadPart - vcnPrev.QuadPart);	//get the length of each cluster

                                    liSrcPos.QuadPart = (pPoints->Extents[dwExtent].Lcn.QuadPart * dwCluserSize);				//get the source position on disk
                                    liDstPos.QuadPart = (vcnPrev.QuadPart * dwCluserSize);										//get the destination position on disk

                                    //loop over the clusters
                                    for(DWORD dwCluser = 0; dwCluser < dwLength; dwCluser++) {
                                        //set the file pointer of the drive to the location of the source files clusters
                                        if(SetFilePointer(hDrive, liSrcPos.LowPart, &liSrcPos.HighPart, FILE_BEGIN) != INVALID_SET_FILE_POINTER) {
                                            //read its contents per cluster size
                                            if(ReadFile(hDrive, lpCluser, dwCluserSize, &dwRead, NULL)) {
                                                //set the file pointer of the destination file
                                                if(SetFilePointer(hDst, liDstPos.LowPart, &liDstPos.HighPart, FILE_BEGIN) != INVALID_SET_FILE_POINTER) {
                                                    //write the contents of the file to the new location
                                                    if(WriteFile(hDst, lpCluser, dwRead, &dwWritten, NULL)) {
                                                        liSrcPos.QuadPart += dwCluserSize;	//move the source position to the next cluster
                                                        liDstPos.QuadPart += dwCluserSize;	//move the destination position to the next cluster
                                                    }
                                                    else {
                                                        //if the write file fails end the loop and report error
                                                        result = GetLastError();
                                                        break;
                                                    }
                                                }
                                                else {
                                                    //if the set file pointer failes end the loop and report error
                                                    result = GetLastError();
                                                    break;
                                                }
                                            }
                                            else {
                                                //if the read file files end the loop and report error
                                                result = GetLastError();
                                                break;
                                            }
                                        }
                                        else {
                                            //if the set file pointer fails report the error
                                            result = GetLastError();
                                        }
                                    }
                                    //go to the next cluster on disk
                                    vcnPrev = pPoints->Extents[dwExtent].NextVcn;
                                }
                                //clean up memory
                                delete[] lpCluser;
                            }
                            else {
                                //if setting the end of the file fails report the error
                                result = GetLastError();
                            }
                        }
                        else {
                            //if the set file pointer fails report the error
                            result = GetLastError();
                        }
                        //close the handle to the destination file
                        CloseHandle(hDst);
                    }
                    //close the handle to the drive
                    CloseHandle(hDrive);
                }
                //if handle to the drive is bad report error
                else {
                    result = GetLastError();
                }
            }
            //if the device io control fails then the file is a resident file i.e. in the MFT itself
            else {
                result = GetLastError();
                //check to see if the file is located in the mft
                if(result == ERROR_HANDLE_EOF) {
                    szDrive = L"\\\\.\\" + szDrive;	//get unc path to the drive i.e. \\\\.\\C:

                    //get a handle to the drive
                    HANDLE hDrive = CreateFile(szDrive.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
                    //as long as the handle is good
                    if(hDrive != INVALID_HANDLE_VALUE) {
                        NTFS_VOLUME_DATA_BUFFER ntfsVolData; //struct to hold the ntfs volume data

                        //get the ntfs volume data
                        if(DeviceIoControl(hDrive, FSCTL_GET_NTFS_VOLUME_DATA, NULL, 0, &ntfsVolData, sizeof(ntfsVolData), &dwWritten, NULL)) {
                            BY_HANDLE_FILE_INFORMATION bhFileInfo; //struct to hold file information

                            //get the source file informaiton
                            if(GetFileInformationByHandle(hFile, &bhFileInfo)) {
                                unsigned long dwOutputSize = (sizeof(NTFS_FILE_RECORD_OUTPUT_BUFFER) + ntfsVolData.BytesPerFileRecordSegment);			//determine the size of the file
                                NTFS_FILE_RECORD_INPUT_BUFFER mftRecordInput = {{bhFileInfo.nFileIndexLow, (LONG)bhFileInfo.nFileIndexHigh}};					//set the location of the file record based on the file information
                                PNTFS_FILE_RECORD_OUTPUT_BUFFER pmftRecordOutput = (PNTFS_FILE_RECORD_OUTPUT_BUFFER) new unsigned char[dwOutputSize];	//allocate space for output buffer

                                //get the ntfs file record for the file in question
                                if(DeviceIoControl(hDrive, FSCTL_GET_NTFS_FILE_RECORD, &mftRecordInput, sizeof(mftRecordInput), pmftRecordOutput, dwOutputSize, &dwWritten, NULL)) {
                                    PFILE_RECORD_HEADER pFileRecord = (PFILE_RECORD_HEADER)pmftRecordOutput->FileRecordBuffer;	//get the record header for the file
                                    unsigned char *lpData = NULL;																//buffer that will hold the file data
                                    unsigned long dwSize = 0;																	//the size of the data
                                    PATTRIBUTE pAttribute = (PATTRIBUTE)((LPBYTE)pFileRecord + pFileRecord->AttributeOffset);	//get the file attributes

                                    //loop over the attributes as long as there are bytes
                                    while(reinterpret_cast<unsigned char*>(pAttribute) < reinterpret_cast<unsigned char*>(pFileRecord + pFileRecord->BytesInUse)) {
                                        //only care about the data and no other attributes
                                        if(pAttribute->AttributeType == AttributeData) {
                                            PRESIDENT_ATTRIBUTE pResAttrib = (PRESIDENT_ATTRIBUTE)pAttribute;					//convert the attribute to the residient
                                            lpData = reinterpret_cast<unsigned char*>(pResAttrib) + pResAttrib->ValueOffset;	//get the data and length
                                            dwSize = pResAttrib->ValueLength;													//get the size of the data
                                            break;
                                        }
                                        pAttribute = (PATTRIBUTE)(reinterpret_cast<unsigned char*>(pAttribute) + pAttribute->Length);	//continue to the next attribute
                                    }
                                    //as long as there was data collected
                                    if(lpData != NULL) {
                                        //get a handle to the destination file
                                        HANDLE hDst = CreateFile(dst.c_str(), GENERIC_WRITE, 0, NULL, CREATE_NEW, 0, NULL);
                                        //as long as the file handle was good
                                        if(hDst != INVALID_HANDLE_VALUE) {
                                            //write the data that was extracted to file
                                            if(!WriteFile(hDst, lpData, dwSize, &dwWritten, NULL)) {
                                                //if the write failed for any reason return error
                                                result = GetLastError();
                                            }
                                            //close the handle to the destination file
                                            CloseHandle(hDst);
                                        }
                                    }
                                }
                                //if unable to extract the ntfs info from MFT return error
                                else {
                                    result = GetLastError();
                                }
                                //free memory
                                delete[] pmftRecordOutput;
                            }
                            //if unable to a the file informaiton return error
                            else {
                                result = GetLastError();
                            }
                        }
                        //if unable to get NFTS data return error
                        else {
                            result = GetLastError();
                        }
                        //close the handle to the drive
                        CloseHandle(hDrive);
                    }
                    //if the drive handle was bad
                    else {
                        result = GetLastError();
                    }
                }
            }
            //free memory
            delete[] pPoints;
        }
        //if unable to get basic disk information
        else {
            result = GetLastError();
        }
        //close handle to the source file
        CloseHandle(hFile);
    }
    //Failed to open source file handle
    else {
        result = GetLastError();
    }
    //return result of the operation
    return result;
}

//Main just to start the raw copy
//int wmain(int argc, wchar_t** argv) {
//    if(argc != 3) {
//        wcout << L"usage: rawcopy.exe COPY_FROM COPY_TO\n\n" <<
//            L"RawCopy: Copies any file from the hard drive\n\n" <<
//            L"required arguments:\n" <<
//            L"  COPY_FROM     The path to the file to copy from\n" <<
//            L"  COPY_TO       The path to the file to copy to\n";
//        return -1;
//    }

//    unsigned long result = RawCopy(wstring(argv[1]), wstring(argv[2]));
//    if(result != ERROR_SUCCESS) {
//        wcout << L"Error: " << result << "\n";
//    }
//    else {
//        wcout << L"Success\n";
//    }

//    return 0;
//}
