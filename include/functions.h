#pragma once

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/SimpleFileSystem.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Guid/FileInfo.h>

#include "pe_format/dos_header.h"
#include "pe_format/nt_header.h"
#include "pe_format/section_header.h"

#define KERNEL_PHYSICAL_BASE_ADDRESS (0x800000ULL)
#define KERNEL_FILE_NAME L"system.exe"

enum MEMORY_TYPE{USABLE, UNUSABLE, ACPINVS, ACPIRECLAIM, RESERVED};

#define QEMU
//#define RELEASE

#define MEM_DES_MAX_COUNT 64

struct MemoryDescriptor
{
        UINT64 physical_base_address;
        UINT64 physical_end_address;
        UINT64 size;
        enum MEMORY_TYPE type;
};

struct MapInfo
{
        UINT64 cr3_value;
        UINT64 kernel_image_physical_base_address;
        UINT64 kernel_image_virtual_base_address;
        UINT64 kernel_image_size;
        UINT64 kernel_new_map_base_address;
        UINT64 kernel_new_map_end_address;
};

struct MemoryList
{
        UINT32 memory_list_valid_count;
        struct MapInfo system_map_info;
        struct MemoryDescriptor mem_list[MEM_DES_MAX_COUNT];
};

struct GraphicInfo
{
        UINT32 horizen_x, horizen_y;
        UINT32 pixel_per_scan_line;
        UINT64 fream_buffer_physical_address;
        UINT64 fream_buffer_virtual_address;
        UINT64 fream_buffer_size;
};

struct BootInfo
{
        struct MemoryList memory_info;
        struct GraphicInfo graphic_info;
};

UINT64 GetCR3Value();
UINT64 SearchPhysicalAddress(UINT64 virtual_address, UINT64 cr3v);
void MapPages(UINT64 pstart_address, UINT64 vstart_address, UINT64 page_count, UINT64 crv);

//return value is the physical base address of boot info structure
UINT64 PrepareBootInfo(EFI_HANDLE* image_handle);

//read kernel file data to boot info's base address + size of boot info structure
//return value is kernel file's physical address
UINT64 LoadKernelFile(EFI_HANDLE* image_handle, CHAR16* file_name, UINT64 boot_info_base_address, UINT64* kernel_file_size);

//parse kernel file and load sections to kernel file base address + size of file, after this, the hardware memory structure(cr3) is determined
//return value is image's physical address
UINT64 ExpandKernelFileToImage(UINT64 kernel_file_load_base_address, UINT64* image_size_buffer, UINT64 file_size, UINT64* kernel_entry, UINT64* image_base_buffer);

//void LoadSection(UINT64 physical_base_address, IMAGE_NT_HEADERS64* nt64_header);

//map the kernel file and the fream buffer, then modify the boot info to tell the kernel that where it self is and where's the fream buffer and the new memory structure
//retur val is the physical address of kernel entry point
void Map(UINT64 kernel_image_base_paddr, UINT64 kernel_image_base_vaddr, UINT64 image_size, UINT64 alloc_head, struct BootInfo* boot_info);

//the kernel will start here
void ExecKernel(UINT64 entry_point);