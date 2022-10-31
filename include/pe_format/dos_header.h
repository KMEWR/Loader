#pragma once
#include "type.h"

typedef struct _IMAGE_DOS_HEADER // DOS .EXE header
{      
    UINT16   e_magic;                     // Magic number
    UINT16   e_cblp;                      // Bytes on last page of file
    UINT16   e_cp;                        // Pages in file
    UINT16   e_crlc;                      // Relocations
    UINT16   e_cparhdr;                   // Size of header in paragraphs
    UINT16   e_minalloc;                  // Minimum extra paragraphs needed
    UINT16   e_maxalloc;                  // Maximum extra paragraphs needed
    UINT16   e_ss;                        // Initial (relative) SS value
    UINT16   e_sp;                        // Initial SP value
    UINT16   e_csum;                      // Checksum
    UINT16   e_ip;                        // Initial IP value
    UINT16   e_cs;                        // Initial (relative) CS value
    UINT16   e_lfarlc;                    // File address of relocation table
    UINT16   e_ovno;                      // Overlay number
    UINT16   e_res[4];                    // Reserved UINT16s
    UINT16   e_oemid;                     // OEM identifier (for e_oeminfo)
    UINT16   e_oeminfo;                   // OEM information; e_oemid specific
    UINT16   e_res2[10];                  // Reserved UINT16s
    UINT32   e_lfanew;                    // File address of new exe header
}__attribute__((packed)) IMAGE_DOS_HEADER;//, *PIMAGE_DOS_HEADER;