/**
 * author:      KMEXEC
 * mail:        kmeflexable@163.com
 */

#include "../include/functions.h"

UINT64 GetCR3Value()
{
        UINT64 val;
        asm volatile("movq %%cr3, %%rax" : "=a"(val));
        return val;
}

UINT64 GetCR0Value()
{
        UINT64 cr0v;
        asm volatile("movq %%cr0, %%rax" : "=a"(cr0v));
        return cr0v;
}

void DisableCR0WP()
{
        UINT64 cr0v;
        asm volatile("movq %%cr0, %%rax" : "=a"(cr0v));
        if(cr0v & (1 << 16))
        {
                cr0v -= 0x10000;
                asm volatile("movq %0, %%cr0"::"r"(cr0v):"memory");
        }
}

void EnableCR0WP()
{
        UINT64 cr0v;
        asm volatile("movq %%cr0, %%rax" : "=a"(cr0v));
        if(cr0v & (1 << 16))
        {
                return;
        }
        else
        {
                cr0v += 0x10000;
                asm volatile("movq %0, %%cr0"::"r"(cr0v):"memory");
        }
}

void WriteCR3(UINT64 val)
{
        UINT64 t = val;
        asm volatile("movq %0, %%cr3"::"r"(t):"memory");
}

void ReloadCR3()
{
        UINT64 a = GetCR3Value();
        WriteCR3(a);
}

#define MEM_MEMMAP_PML4_SEEK(addr)                  ((((UINT64)(addr)) & 0xFF8000000000) >> 39)
#define MEM_MEMMAP_PDPT_SEEK(addr)                  ((((UINT64)(addr)) & 0x7FC0000000) >> 30)
#define MEM_MEMMAP_PDT_SEEK(addr)                   ((((UINT64)(addr)) & 0x3FE00000) >> 21)
#define MEM_MEMMAP_PT_SEEK(addr)                    ((((UINT64)(addr)) & 0x1FF000) >> 12)

UINT64 SearchPhysicalAddress(UINT64 virtual_address, UINT64 cr3v)
{
        //UINT64 pmle, pdpte, pde, pte;
        UINT64 pte;
        UINT64* pml; UINT64* pdpt; UINT64* pd; UINT64* pt;
        UINT32 pml_offset      = MEM_MEMMAP_PML4_SEEK(virtual_address);
        UINT32 pdpt_offset     = MEM_MEMMAP_PDPT_SEEK(virtual_address);
        UINT32 pd_offset       = MEM_MEMMAP_PDT_SEEK(virtual_address);
        UINT32 pt_offset       = MEM_MEMMAP_PT_SEEK(virtual_address);
        pml     = (UINT64*)cr3v;
        pdpt    = (UINT64*)(pml[pml_offset] & 0x7FFFFFFFFFFF000);
        pd      = (UINT64*)(pdpt[pdpt_offset] & 0x7FFFFFFFFFFF000);
        pt      = (UINT64*)(pd[pd_offset] & 0x7FFFFFFFFFFF000);
        pte     = pt[pt_offset];

        return (pte & 0x7FFFFFFFFFFF000);
}

UINT64 table_alloc_usable;

UINT64 AllocTable(UINT32 request_table_level_count)
{
        UINT64 ret_val = table_alloc_usable;
        table_alloc_usable += (request_table_level_count * 0x1000);
        return ret_val;
}

void MapPages(UINT64 pstart_address, UINT64 vstart_address, UINT64 page_count, UINT64 crv)
{
        //check every page
        UINT64 p, v;

        UINT32 pml_offset;
        UINT32 pdpt_offset;
        UINT32 pdt_offset;
        UINT32 pt_offset;

        UINT64* pml;
        UINT64* pdpt;
        UINT64* pdt;
        UINT64* pt; 

        UINT64 pmle;
        UINT64 pdpte;
        UINT64 pdte;
        UINT64 pte;

        UINT64 temp_address;

        DisableCR0WP();
        for(UINT32 i = 0 ; i < page_count ; ++i)
        {
                //judge which struct is not exist
                p = pstart_address + (0x1000 * i);
                v = vstart_address + (0x1000 * i);
                pml_offset      = MEM_MEMMAP_PML4_SEEK(v);
                pdpt_offset     = MEM_MEMMAP_PDPT_SEEK(v);
                pdt_offset      = MEM_MEMMAP_PDT_SEEK(v);
                pt_offset       = MEM_MEMMAP_PT_SEEK(v);

                pml     = (UINT64*)crv;
                pdpt    = (UINT64*)((pml[pml_offset]) & 0xFFFFFFFFFFFF000);
                pdt     = (UINT64*)((pdpt[pdpt_offset]) & 0xFFFFFFFFFFFF000);
                pt      = (UINT64*)((pdt[pdt_offset]) & 0xFFFFFFFFFFFF000);

                //construct map structure
                if(pml[pml_offset] == 0)                        //alloc pdpt,pdt,pt
                {
                        temp_address = AllocTable(3);           //pdpt,pdt,pt

                        pmle    = temp_address + 3;             //pdpt
                        pdpte   = temp_address + 0x1000 + 3;    //pdt
                        pdte    = temp_address + 0x2000 + 3;    //pt
                        pte     = p + 3;
                        
                        pdpt    = (UINT64*)temp_address;
                        pdt     = (UINT64*)(temp_address + 0x1000);
                        pt      = (UINT64*)(temp_address + 0x2000);

                        pml[pml_offset]         = pmle;
                        pdpt[pdpt_offset]       = pdpte;
                        pdt[pdt_offset]         = pdte;
                        pt[pt_offset]           = pte;

                        continue;
                }
                else if(pdpt[pdpt_offset] == 0)                 //alloc pdt,pt
                {
                        temp_address = AllocTable(2);           //pdt,pt

                        pdpte   = temp_address + 3;             //pdt
                        pdte    = temp_address + 0x1000 + 3;    //pt
                        pte     = p + 3;

                        pdt     = (UINT64*)(temp_address);
                        pt      = (UINT64*)(temp_address + 0x1000);

                        pdpt[pdpt_offset]       = pdpte;
                        pdt[pdt_offset]         = pdte;
                        pt[pt_offset]           = pte;

                        continue;
                }
                else if(pdt[pdt_offset] == 0)                   //alloc pt
                {
                        temp_address = AllocTable(1);
                        
                        pdte    = temp_address + 3;
                        pte     = p + 3;

                        pt = (UINT64*)(temp_address);

                        pdt[pdt_offset]         = pdte;
                        pt[pt_offset]           = pte;

                        continue;
                }
                else //pt[pt_offset] == 0
                {
                        pte = p + 3;

                        pt[pt_offset] = pte;

                        continue;
                }
        }
        ReloadCR3();
        EnableCR0WP();
}

void MapKernel(UINT64 image_base_paddr, UINT64 image_base_vaddr, UINT64 image_size)
{
        UINT64 page_count = image_size / 0x1000;
        MapPages(image_base_paddr, image_base_vaddr, page_count, GetCR3Value());
}

void Map(UINT64 kernel_image_base_paddr, UINT64 kernel_image_base_vaddr, UINT64 image_size, UINT64 alloc_head, struct BootInfo* boot_info)
{
        //init alloc structure
        table_alloc_usable = alloc_head;

        //first, map kernel image
        UINT64 page_count = image_size / 0x1000;
        MapPages(kernel_image_base_paddr, kernel_image_base_vaddr, page_count, GetCR3Value());

        //second, map fream buffer
        page_count = ((boot_info->graphic_info.fream_buffer_size & 0xFFFFFFFFFFFF000) / 0x1000) + 1;
        MapPages(boot_info->graphic_info.fream_buffer_physical_address, boot_info->graphic_info.fream_buffer_physical_address + kernel_image_base_vaddr, page_count, GetCR3Value());

        //third, fill up the map info
        boot_info->graphic_info.fream_buffer_virtual_address                            = boot_info->graphic_info.fream_buffer_physical_address + kernel_image_base_vaddr;
        boot_info->memory_info.system_map_info.kernel_image_physical_base_address       = kernel_image_base_paddr;
        boot_info->memory_info.system_map_info.kernel_image_virtual_base_address        = kernel_image_base_vaddr;
        boot_info->memory_info.system_map_info.kernel_image_size                        = image_size;
        boot_info->memory_info.system_map_info.kernel_new_map_base_address              = alloc_head;
        boot_info->memory_info.system_map_info.kernel_new_map_end_address               = table_alloc_usable - 0x1000;
        boot_info->memory_info.system_map_info.cr3_value                                = GetCR3Value();
}