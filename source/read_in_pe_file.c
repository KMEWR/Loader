/**
 * author:      KMEXEC
 * mail:        kmeflexable@163.com
 */

#include "../include/functions.h"

UINT64 LoadKernelFile(EFI_HANDLE* image_handle, CHAR16* file_name, UINT64 boot_info_base_address, UINT64* kernel_file_size)
{
        EFI_FILE_HANDLE root_file_system, file_handle;
        EFI_FILE_IO_INTERFACE* volume;
        EFI_LOADED_IMAGE* load_image;

        gBS->HandleProtocol(*image_handle, &gEfiLoadedImageProtocolGuid, (VOID*)&load_image);
        gBS->HandleProtocol(load_image->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (VOID*)&volume);
        
        //search the driver
        volume->OpenVolume(volume, &root_file_system);
        if(EFI_ERROR(root_file_system->Open(root_file_system, &file_handle, file_name, EFI_FILE_MODE_READ, 0)))
        {
                Print(L"KERNEL FILE NOT FOUND.\n");
		return EFI_SUCCESS;
        }
        
        EFI_FILE_INFO* file_info;
        UINTN buffer_size = sizeof(EFI_FILE_INFO) + sizeof(CHAR16) * 100;
        EFI_PHYSICAL_ADDRESS load_base = boot_info_base_address + sizeof(struct BootInfo);

        //alloc memory
        gBS->AllocatePool(EfiRuntimeServicesData,buffer_size, (VOID**)&file_info);
        file_handle->GetInfo(file_handle, &gEfiFileInfoGuid, &buffer_size, file_info);
        gBS->AllocatePages(AllocateAddress, EfiLoaderData, (file_info->FileSize + 0x1000 - 1) / 0x1000, &load_base);
        
        //read in file
        buffer_size = file_info->FileSize;
        *kernel_file_size = file_info->FileSize;
        file_handle->Read(file_handle, &buffer_size, (VOID*)load_base);

        gBS->CloseProtocol(load_image->DeviceHandle,&gEfiSimpleFileSystemProtocolGuid,*image_handle,NULL);
        return load_base;
}

UINT64 ExpandKernelFileToImage(UINT64 kernel_file_load_base_address, UINT64* image_size_buffer, UINT64 file_size, UINT64* kernel_entry, UINT64* image_base_buffer)
{
        IMAGE_DOS_HEADER* dosh                          = (IMAGE_DOS_HEADER*)kernel_file_load_base_address;
        UINT64 byte_seek                                = kernel_file_load_base_address + dosh->e_lfanew;
        IMAGE_NT_HEADERS64* nt64h                       = (IMAGE_NT_HEADERS64*)byte_seek;
        UINT32 optional_header_size                     = nt64h->FileHeader.SizeOfOptionalHeader;
        UINT32 section_number                           = nt64h->FileHeader.NumberOfSections;
        IMAGE_SECTION_HEADER* sections_info_array       = (IMAGE_SECTION_HEADER*)((UINT64)nt64h + sizeof(IMAGE_FILE_HEADER) + sizeof(nt64h->Signature) + optional_header_size);
        UINT64 image_load_base                          = ((kernel_file_load_base_address + file_size) & 0xFFFFFFFFF000) + 0x1000;

        for(UINT32 seek_section_info = 0 ; seek_section_info < section_number ; ++seek_section_info)
        {
                gBS->CopyMem(
                        (void*)(image_load_base + sections_info_array[seek_section_info].VirtualAddress), 
                        (void*)(kernel_file_load_base_address + sections_info_array[seek_section_info].PointerToRawData), 
                        sections_info_array[seek_section_info].Misc.VirtualSize);
        }

        *image_size_buffer      = nt64h->OptionalHeader.SizeOfImage;
        *kernel_entry           = nt64h->OptionalHeader.ImageBase + nt64h->OptionalHeader.AddressOfEntryPoint;
        *image_base_buffer      = nt64h->OptionalHeader.ImageBase;

        return image_load_base;
}