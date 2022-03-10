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

#include "ext2.hpp"

using namespace System;

void System::fs::ext2::test_node(System::fs::fs_node_t node) {
    kout << "Testing node..." << endl;

    superblock_t *sb = new superblock_t;

    u32 BeginingOfPartition = node.partition->lba;
    u32 BeginingOfSuperblock = BeginingOfPartition + 2;

    u8* SuperblockBuffer = (u8*)Memory::kmalloc(1024);

    u32 SuperblockSize = 1024;
    u32 SectorSize = 512;

    for (u32 i = BeginingOfSuperblock; i < (BeginingOfSuperblock + (SuperblockSize / SectorSize)); i ++) {
        Disk::read_sector(*node.partition->disk, i);

        for (int e = 0; e < 512; e ++) {
            SuperblockBuffer[e + ((i - BeginingOfSuperblock) * 512)] = node.partition->disk->read_write_buffer[e];
        }
    }


    for (int i = 0; i < 84; i ++) {
        ((u8*)sb)[i] = SuperblockBuffer[i];
    }

    kout << "Superblock info:" << endl;
    kout << "\tInodes\t\t: " << sb->total_inods << endl;
    kout << "\tBlocks\t\t: " << sb->total_bocks << endl;
    kout << "\tReserved\t: " << sb->reserved_blocks << endl;
    kout << "\tunallocBlocks\t: " << sb->total_unalloc_blocks << endl;
    kout << "\tunallocInodes\t: " << sb->total_unalloc_inoodes << endl;
    kout << "\tSuperblock\t: " << sb->block_of_super_block << endl;
    kout << "\tShift Block\t: " << sb->block_size_shift << endl;
    kout << "\tShift Inode\t: " << sb->total_unalloc_inoodes << endl;
    kout << "\tFS ID\t\t: 0x" << kout.ToHex(sb->ext2sig) << (sb->ext2sig == 0xEF53 ? "  <-- Valid Sig" : "  <-- NOT VALID") << endl;
}
