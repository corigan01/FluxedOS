;/*
; *       ______            __ __                 __
; *      / __/ /_ ____ __  / //_/__ _______  ___ / /
; *     / _// / // /\ \ / / ,< / -_) __/ _ \/ -_) / 
; *    /_/ /_/\_,_//_\_\ /_/|_|\__/_/ /_//_/\__/_/  
; *    
; *   copyright (c) 2021 Gavin Kellam (aka corigan01)
; *   
; *   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
; *   to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
; *   and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
; *
; *   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
; *
; *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
; *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
; *   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                              
; *  
; *   
; */

# Declare constants for the multiboot header.
.set ALIGN,    1<<0             				# align loaded modules on page boundaries
.set MEMINFO,  1<<1             				# provide memory map
.set GFXMODE,  1<<2             				# set graphics mode
.set FLAGS,    ALIGN | MEMINFO | GFXMODE  		# this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       				# 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) 				# checksum of above, to prove we are multiboot

# Declare a multiboot header that marks the program as a kernel.
.section .multiboot.data, "aw"
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM
.long 0x00000000
.long 0x00000000
.long 0x00000000
.long 0x00000000
.long 0x00000000
.long 0x00000000
.long 0
.long 0
.long 32 										# this tells grub that the vid mode is compatable with the Kernel

# Allocate the initial stack.
.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

# Preallocate pages used for paging. Don't hard-code addresses and assume they
# are available, as the bootloader might have loaded its multiboot structures or
# modules there. This lets the bootloader know it must avoid the addresses.

.section .bss, "aw", @nobits
	.align 4096
boot_page_directory:
	.skip 4096
boot_page_table1:
	.skip 4096
TepKmalloc:
	.skip 1024 * 24 


# Further page tables may be required if the kernel grows beyond 3 MiB.

# The kernel entry point.
.section .multiboot.text, "a"
.global _start
.type _start, @function
_start:

	# Physical address of boot_page_table1.
	movl $(boot_page_table1 - 0xC0000000), %edi

	# First address to map is address 0.
	movl $0, %esi
	# Map 1015 pages for the kernel. (the kernel only uses 256 or so )
	movl $1015, %ecx

1:
	# Only map the kernel.
	cmpl $0, %esi
	jl 2f
	cmpl $(_kernel_end - 0xC0000000), %esi
	jge 3f

	# Map physical address as "present, writable". Note that this maps
	# .text and .rodata as writable. Mind security and map them as non-writable.
	movl %esi, %edx
	orl $0x003, %edx
	movl %edx, (%edi)

2:
	# Size of page is 4kb. 
	addl $4096, %esi

	# Size of entries in boot_page_table1 is 4 bytes.
	addl $4, %edi

	# Loop to the next entry if we haven't finished.
	loop 1b

3:
	movl %ebx, %eax
	orl $0x003, %eax

	# Map the (DUMB) Virtual Memory Kmalloc area. (4kb) 0xC03FF000 - (2 * 4kb) 
	movl $(TepKmalloc - 0xC0000000 			     + 0x003), boot_page_table1 - 0xC0000000 + 1016 * 4
	movl $(TepKmalloc - 0xC0000000 + (1024 * 4 ) + 0x003), boot_page_table1 - 0xC0000000 + 1017 * 4
	movl $(TepKmalloc - 0xC0000000 + (1024 * 8 ) + 0x003), boot_page_table1 - 0xC0000000 + 1018 * 4
	movl $(TepKmalloc - 0xC0000000 + (1024 * 12) + 0x003), boot_page_table1 - 0xC0000000 + 1019 * 4
	movl $(TepKmalloc - 0xC0000000 + (1024 * 16) + 0x003), boot_page_table1 - 0xC0000000 + 1020 * 4
	movl $(TepKmalloc - 0xC0000000 + (1024 * 20) + 0x003), boot_page_table1 - 0xC0000000 + 1021 * 4

	# Map VGA video memory to 0xC03FF000 - 4kb as "present, writable".
	# NO NEED, THIS IS IN PAGE 256 NOW
	# movl $(0x000B8000 | 0x003), boot_page_table1 - 0xC0000000 + 1022 * 4

    # Map the eax and ebx to virtal addresses (0xC03FF000).
	movl %eax, boot_page_table1 - 0xC0000000 + 1023 * 4

	# Push the new Vaddress into the Regsiters
	movl $(0xC03FF000 ), %ebx
	movl $(0xC03FF000 ), %eax
	subl $(1024 * 8), %eax


	# The page table is used at both page directory entry 0 (virtually from 0x0
	# to 0x3FFFFF) (thus identity mapping the kernel) and page directory entry
	# 768 (virtually from 0xC0000000 to 0xC03FFFFF) (thus mapping it in the
	# higher half). The kernel is identity mapped because enabling paging does
	# not change the next instruction, which continues to be physical. The CPU
	# would instead page fault if there was no identity mapping.

	# Map the page table to both virtual addresses 0x00000000 and 0xC0000000.

	# identity mapping is when you map the phys addr of the kernel to both
	# (+0) and you map it to (+0xC0000000). So we can continue as if nothing changed 
	movl $(boot_page_table1 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 0
	movl $(boot_page_table1 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 768 * 4

	# Set cr3 to the address of the boot_page_directory.
	movl $(boot_page_directory - 0xC0000000), %ecx
	movl %ecx, %cr3

	# Enable paging and the write-protect bit.
	movl %cr0, %ecx
	orl $0x80010000, %ecx
	movl %ecx, %cr0

	# Paging is enabled, but we are still in the lower-half
	# so we need to jump into (+0xC0000000), but its ok
	# because we have the phys addrs mapped there as well. 

	# Jump to higher half with an absolute jump. 
	lea 4f, %ecx
	jmp *%ecx

.section .text
4:
	# At this point, paging is fully set up and enabled. 
	# We are also in higher half, but we left behind the
	# one to one map, so lets remove it. 

	# Unmap the identity mapping as it is now unnecessary. 
	movl $0, boot_page_directory + 0

	# Reload crc3 to force a TLB flush so the changes to take effect.
	movl %cr3, %ecx
	movl %ecx, %cr3

	# Set up the stack.
	mov $stack_top, %esp

	push $(boot_page_directory)
	push %eax
    push %ebx

	# Enter the high-level kernel.
	call kernel_entry

	# call cxa is a C++ exception-handling function.
	subl $4, %esp 
	movl $0x0, %esp
	
	call __cxa_finalize
	
	addl $4, %esp

	# Infinite loop if the system has nothing more to do.
	cli
1:	hlt
	jmp 1b
