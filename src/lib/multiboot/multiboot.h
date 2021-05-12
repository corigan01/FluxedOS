/*
 *       ______            __ __                 __
 *      / __/ /_ ____ __  / //_/__ _______  ___ / /
 *     / _// / // /\ \ / / ,< / -_) __/ _ \/ -_) / 
 *    /_/ /_/\_,_//_\_\ /_/|_|\__/_/ /_//_/\__/_/  
 *    
 *   copyright (c) 2021 Gavin Kellam (aka corigan01)
 *   
 *   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
 *   to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *   and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 *   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                              
 *  
 *   
 */

#ifndef KERNEL__H___MULTIBOOT
#define KERNEL__H___MULTIBOOT 

#include "../core/core.h"

#define MB_SEARCH_SIZE                             8192
#define MB_HEADER_ALIGN                            4
#define MB_MAGIC_HEADER_NUMBER                  0x1BADB002
#define MB_MAGIC_BOOTLOADER_NUMBER              0x2BADB002
#define MB_MOD_ALIGN                            0x00001000
#define MB_INFO_ALIGN                           0x00000004
#define MB_PAGE_ALIGN                           0x00000001
#define MB_MEM_INFO                             0x00000002
#define MB_VID_MODE                             0x00000004
#define MB_AOUT_KLUDGE                          0x00010000

struct multiboot_header
{
  uint32 magic;
  uint32 flags;
  uint32 checksum;
  uint32 header_addr;
  uint32 load_addr;
  uint32 load_end_addr;
  uint32 bss_end_addr;
  uint32 entry_addr;
  uint32 mode_type;
  uint32 width;
  uint32 height;
  uint32 depth;
};
struct mb_aout_SymbolTable
{
  uint32 tabsize;
  uint32 strsize;
  uint32 addr;
  uint32 reserved;
};
typedef struct mb_aout_SymbolTable mb_aout_SymbolTable_t;

struct mb_elf_HeaderTable
{
  uint32 num;
  uint32 size;
  uint32 addr;
  uint32 shndx;
};
typedef struct mb_elf_HeaderTable mb_elf_HeaderTable_t;

// this is provided from the real MultiBoot Header
struct multiboot_info
{
  uint32 flags;
  uint32 mem_lower;
  uint32 mem_upper;
  uint32 boot_device;
  uint32 cmdline;
  uint32 mods_count;
  uint32 mods_addr;
  union
  {
    mb_aout_SymbolTable_t aout_sym;
    mb_elf_HeaderTable_t elf_sec;
  } u;

  uint32 mmap_length;
  uint32 mmap_addr;
  uint32 drives_length;
  uint32 drives_addr;
  uint32 config_table;
  uint32 boot_loader_name;
  uint32 apm_table;
  uint32 vbe_control_info;
  uint32 vbe_mode_info;
  uint16 vbe_mode;
  uint16 vbe_interface_seg;
  uint16 vbe_interface_off;
  uint16 vbe_interface_len;
  uint64 framebuffer_addr;
  uint32 framebuffer_pitch;
  uint32 framebuffer_width;
  uint32 framebuffer_height;
  uint8 framebuffer_bpp;
#define MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED      0
#define MULTIBOOT_FRAMEBUFFER_TYPE_RGB          1
#define MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT     2
  uint8 framebuffer_type;
  union
  {
    struct
    {
      uint32 framebuffer_palette_addr;
      uint16 framebuffer_palette_num_colors;
    };
    struct
    {
      uint8 framebuffer_red_field_position;
      uint8 framebuffer_red_mask_size;
      uint8 framebuffer_green_field_position;
      uint8 framebuffer_green_mask_size;
      uint8 framebuffer_blue_field_position;
      uint8 framebuffer_blue_mask_size;
    };
  };
};
typedef struct multiboot_info multiboot_info_t;

struct multiboot_mmap_entry
{
  uint32 size;
  uint32 base_addr_low,base_addr_high;
	uint32 length_low,length_high;
  
#define MULTIBOOT_MEMORY_AVAILABLE              1
#define MULTIBOOT_MEMORY_RESERVED               2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE       3
#define MULTIBOOT_MEMORY_NVS                    4
#define MULTIBOOT_MEMORY_BADRAM                 5
  uint32 type;
} __attribute__((packed));
typedef struct multiboot_mmap_entry multiboot_memory_map_t;


#endif 