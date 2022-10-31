#include "type.h"

typedef struct _IMAGE_FILE_HEADER 
{
    UINT16      Machine;
    UINT16      NumberOfSections;
    UINT32      TimeDateStamp;
    UINT32      PointerToSymbolTable;
    UINT32      NumberOfSymbols;
    UINT16      SizeOfOptionalHeader;
    UINT16      Characteristics;
}__attribute__((packed)) IMAGE_FILE_HEADER;

typedef struct _IMAGE_DATA_DIRECTORY 
{
    UINT32   VirtualAddress;
    UINT32   Size;
}__attribute__((packed)) IMAGE_DATA_DIRECTORY;//, *PIMAGE_DATA_DIRECTORY;

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES    16

typedef struct _IMAGE_OPTIONAL_HEADER64 
{
    UINT16          Magic;
    UINT8           MajorLinkerVersion;
    UINT8           MinorLinkerVersion;
    UINT32          SizeOfCode;
    UINT32          SizeOfInitializedData;
    UINT32          SizeOfUninitializedData;
    UINT32          AddressOfEntryPoint;
    UINT32          BaseOfCode;
    UINT64          ImageBase;
    UINT32          SectionAlignment;
    UINT32          FileAlignment;
    UINT16          MajorOperatingSystemVersion;
    UINT16          MinorOperatingSystemVersion;
    UINT16          MajorImageVersion;
    UINT16          MinorImageVersion;
    UINT16          MajorSubsystemVersion;
    UINT16          MinorSubsystemVersion;
    UINT32          Win32VersionValue;
    UINT32          SizeOfImage;
    UINT32          SizeOfHeaders;
    UINT32          CheckSum;
    UINT16          Subsystem;
    UINT16          DllCharacteristics;
    UINT64          SizeOfStackReserve;
    UINT64          SizeOfStackCommit;
    UINT64          SizeOfHeapReserve;
    UINT64          SizeOfHeapCommit;
    UINT32          LoaderFlags;
    UINT32          NumberOfRvaAndSizes;
    IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
}__attribute__((packed)) IMAGE_OPTIONAL_HEADER64;

typedef struct _IMAGE_NT_HEADERS64 {
    UINT32 Signature;
    IMAGE_FILE_HEADER FileHeader;
    IMAGE_OPTIONAL_HEADER64 OptionalHeader;
}__attribute__((packed)) IMAGE_NT_HEADERS64;