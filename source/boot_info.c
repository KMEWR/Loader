/**
 * author:      KMEXEC
 * mail:        kmeflexable@163.com
 */

#include "../include/functions.h"

static UINT64 PrepareMemoryInfo()
{
        UINTN mem_map_size = 0;
	EFI_MEMORY_DESCRIPTOR* mem_map = 0;
	UINTN mem_key = 0;
	UINTN descriptor_size = 0;
	UINT32 des_version = 0;

	gBS->GetMemoryMap(&mem_map_size,mem_map,&mem_key,&descriptor_size, &des_version);
	mem_map_size += descriptor_size * 5;
	gBS->AllocatePool(EfiRuntimeServicesData,mem_map_size,(VOID**)&mem_map);
	gBS->SetMem((void*)mem_map,mem_map_size,0);
	gBS->GetMemoryMap(&mem_map_size,mem_map,&mem_key,&descriptor_size, &des_version);

        EFI_MEMORY_DESCRIPTOR* seek_map;
        
        UINT64 biggest_base;
        UINT64 last;
        for(UINT32 i = 0 ; i < mem_map_size / descriptor_size ; ++i)
        {
                seek_map = (EFI_MEMORY_DESCRIPTOR*) ((CHAR8*)mem_map + i * descriptor_size);
                if(seek_map->NumberOfPages == 0)
                {
                        continue;
                }
                if((i >= 1) && (seek_map->NumberOfPages > last))
                {
                        biggest_base = seek_map->PhysicalStart;
                }
                last = seek_map->NumberOfPages;
        }

        struct MemoryList* target_info_struct = (struct MemoryList*)biggest_base;
        UINT32 seek_info = 0, typex;
        for(UINT32 i = 0 ; i < mem_map_size / descriptor_size ; ++i)
        {
                seek_map = (EFI_MEMORY_DESCRIPTOR*) ((CHAR8*)mem_map + i * descriptor_size);
                if(seek_map->NumberOfPages == 0)
                {
                        continue;
                }
                switch(seek_map->Type)
                {
                        case EfiReservedMemoryType:
			case EfiMemoryMappedIO:
			case EfiMemoryMappedIOPortSpace:
			case EfiPalCode:
			{
                                typex = RESERVED;
                                break;
                        }
			case EfiUnusableMemory:
			{
                                typex = USABLE;
                        }
			case EfiACPIReclaimMemory:
			{
                                typex = ACPIRECLAIM;
                                break;
                        }
			case EfiLoaderCode:
			case EfiLoaderData:
			case EfiBootServicesCode:
			case EfiBootServicesData:
			case EfiRuntimeServicesCode:
			case EfiRuntimeServicesData:
			case EfiConventionalMemory:
			case EfiPersistentMemory:
			{
                                typex = USABLE;
                                break;
                        }
			case EfiACPIMemoryNVS:
			{
                                typex = ACPINVS;
                                break;
                        }
			default:
			{
                                break;
                        }
                }
                if((seek_info >= 1) && (seek_map->PhysicalStart == target_info_struct->mem_list[seek_info - 1].physical_end_address) && (typex == target_info_struct->mem_list[seek_info - 1].type))
                {
                        target_info_struct->mem_list[seek_info - 1].physical_end_address += (seek_map->NumberOfPages << 12);
                        target_info_struct->mem_list[seek_info - 1].size += (seek_map->NumberOfPages << 12);
                }
                else
                {
                        ++(target_info_struct->memory_list_valid_count);
                        target_info_struct->mem_list[seek_info].physical_base_address = seek_map->PhysicalStart;
                        target_info_struct->mem_list[seek_info].physical_end_address = seek_map->PhysicalStart + (seek_map->NumberOfPages << 12);
                        target_info_struct->mem_list[seek_info].size = (seek_map->NumberOfPages << 12);
                        target_info_struct->mem_list[seek_info].type = typex;
                        if((seek_info >= 1) && (target_info_struct->mem_list[seek_info].physical_base_address > target_info_struct->mem_list[seek_info - 1].physical_base_address) && (typex == USABLE))
                        {
                                biggest_base = target_info_struct->mem_list[seek_info].physical_base_address;
                        }
                        ++seek_info;
                }
        }

        gBS->FreePool(mem_map);

        return biggest_base;
}

static void PrepareGraphicInfo(struct GraphicInfo* target_info_struct, EFI_HANDLE* image_handle)
{
        EFI_GRAPHICS_OUTPUT_PROTOCOL* graphics_output = 0;
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* ginfo = 0;
	UINTN info_size = 0;

        gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid,NULL,(VOID **)&graphics_output);

        for(UINT32 i = 0;i < graphics_output->Mode->MaxMode;i++)
	{
		graphics_output->QueryMode(graphics_output,i,&info_size,&ginfo);
                #ifdef QEMU
                if((ginfo->HorizontalResolution == 1440) && (ginfo->VerticalResolution == 900))
		{
                        graphics_output->SetMode(graphics_output, i);
                        gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid,NULL,(VOID **)&graphics_output);
                        target_info_struct->fream_buffer_physical_address = graphics_output->Mode->FrameBufferBase;
                        target_info_struct->fream_buffer_size = graphics_output->Mode->FrameBufferSize;
                        target_info_struct->horizen_x = graphics_output->Mode->Info->HorizontalResolution;
                        target_info_struct->horizen_y = graphics_output->Mode->Info->VerticalResolution;
                        target_info_struct->pixel_per_scan_line = graphics_output->Mode->Info->PixelsPerScanLine;
                        gBS->FreePool(ginfo);
                        break;
                }
                #else
                gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid,NULL,(VOID **)&graphics_output);
                target_info_struct->fream_buffer_physical_address = graphics_output->Mode->FrameBufferBase;
                target_info_struct->fream_buffer_size = graphics_output->Mode->FrameBufferSize;
                target_info_struct->horizen_x = graphics_output->Mode->Info->HorizontalResolution;
                target_info_struct->horizen_y = graphics_output->Mode->Info->VerticalResolution;
                target_info_struct->pixel_per_scan_line = graphics_output->Mode->Info->PixelsPerScanLine;
                break;
                #endif
	}
        gBS->CloseProtocol(graphics_output,&gEfiGraphicsOutputProtocolGuid,*image_handle,NULL);
}

UINT64 PrepareBootInfo(EFI_HANDLE *image_handle)
{
        UINT64 boot_info_base_address_retval = PrepareMemoryInfo();
        struct BootInfo* boot_info_base_address = (struct BootInfo*)boot_info_base_address_retval;
        PrepareGraphicInfo(&boot_info_base_address->graphic_info, image_handle);

        return boot_info_base_address_retval;
}