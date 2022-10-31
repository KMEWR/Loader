/**
 * author:      KMEXEC
 * mail:        kmeflexable@163.com
 */

#include "../include/functions.h"

EFI_STATUS EFIAPI UefiMain(IN EFI_HANDLE image_handle, IN EFI_SYSTEM_TABLE* system_table)
{
        UINT64  boot_info_address;
        UINT64  kernel_file_size;
        UINT64  kernel_image_size;
        UINT64  kernel_image_virtual_base;
        UINT64  kernel_file_load_physical_base_address;
        UINT64  kernel_image_physical_base_address;
        UINT64  kernel_entry_point_physical_address;
        UINT64  hardware_paging_structure_alloc_base;
        UINT64* boot_info_value = (UINT64*)0x5000;

        boot_info_address                       = PrepareBootInfo(&image_handle);
        kernel_file_load_physical_base_address  = LoadKernelFile(&image_handle, KERNEL_FILE_NAME, boot_info_address, &kernel_file_size);
        kernel_image_physical_base_address      = ExpandKernelFileToImage(kernel_file_load_physical_base_address, &kernel_image_size, kernel_file_size, &kernel_entry_point_physical_address, &kernel_image_virtual_base);
        hardware_paging_structure_alloc_base    = kernel_image_physical_base_address + kernel_image_size;
        
        Map(kernel_image_physical_base_address, kernel_image_virtual_base, kernel_image_size, hardware_paging_structure_alloc_base, (struct BootInfo*)boot_info_address);
        *boot_info_value = boot_info_address;

        ExecKernel(kernel_entry_point_physical_address);

        //program should never reach here
        Print(L"WHILE 1");
        while(1);

        return EFI_SUCCESS;
}