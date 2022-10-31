#pragma once
#include "type.h"

#define IMAGE_SIZEOF_SHORT_NAME              8

typedef struct _IMAGE_SECTION_HEADER {
    UINT8    Name[IMAGE_SIZEOF_SHORT_NAME];
    union {
            UINT32   PhysicalAddress;
            UINT32   VirtualSize;
    } Misc;
    UINT32      VirtualAddress;
    UINT32      SizeOfRawData;
    UINT32      PointerToRawData;
    UINT32      PointerToRelocations;
    UINT32      PointerToLinenumbers;
    UINT16      NumberOfRelocations;
    UINT16      NumberOfLinenumbers;
    UINT32      Characteristics;
}__attribute__((packed)) IMAGE_SECTION_HEADER;