#pragma once 
#include "type.h"

typedef struct _IMAGE_EXPORT_DIRECTORY {
    UINT32   Characteristics;
    UINT32   TimeDateStamp;
    UINT16    MajorVersion;
    UINT16    MinorVersion;
    UINT32   Name;
    UINT32   Base;
    UINT32   NumberOfFunctions;
    UINT32   NumberOfNames;
    UINT32   AddressOfFunctions;     // RVA from base of image
    UINT32   AddressOfNames;         // RVA from base of image
    UINT32   AddressOfNameOrdinals;  // RVA from base of image
}__attribute__((packed)) IMAGE_EXPORT_DIRECTORY;