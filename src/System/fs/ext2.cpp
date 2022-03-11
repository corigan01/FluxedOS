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
using namespace System::fs;
using namespace System::fs::ext2;

u32 SuperblockSize = 1024;
u32 SectorSize = 512;
u32 SectorsInBlock = 2;
u32 BlockSize = 1024;

superblock_t* read_superblock(fs::fs_node_t node) {
    superblock_t *sb = (superblock_t*)Memory::kmalloc(1024);

    u32 BeginingOfPartition = node.partition->lba;
    u32 BeginingOfSuperblock = BeginingOfPartition + 2;


    for (u32 i = BeginingOfSuperblock; i < (BeginingOfSuperblock + (SuperblockSize / SectorSize)); i ++) {
        Disk::read_sector(*node.partition->disk, i);

        for (int e = 0; e < 512; e ++) {
            ((u8*)sb)[e + ((i - BeginingOfSuperblock) * 512)] = node.partition->disk->read_write_buffer[e];
        }
    }


    kout << "Superblock info:" << endl;
    kout << "\tInodes               : " << sb->total_inods << endl;
    kout << "\tBlocks               : " << sb->total_bocks << endl;
    kout << "\tReserved             : " << sb->reserved_blocks << endl;
    kout << "\tunallocBlocks        : " << sb->total_unalloc_blocks << endl;
    kout << "\tunallocInodes        : " << sb->total_unalloc_inoodes << endl;
    kout << "\tSuperblock           : " << sb->block_of_super_block << endl;
    kout << "\tShift Block          : " << sb->block_size_shift << endl;
    kout << "\tBlocks per Group     : " << sb->blocks_per_group << endl;
    kout << "\tShift Inode          : " << sb->total_unalloc_inoodes << endl;
    kout << "\tFile System ID       : 0x" << kout.ToHex(sb->ext2sig) << (sb->ext2sig == 0xEF53 ? "  <-- Valid Sig" : "  <-- NOT VALID") << endl;
    kout << endl;

    return sb;
}

block_group_t* read_blockgroup(fs::fs_node_t node, superblock_t* superblock, u32 id) {
    block_group_t* BlockGroup = (block_group_t *)Memory::kmalloc(1024);

    u32 BeginingOfSuperblock = node.partition->lba + (SectorsInBlock * superblock->block_of_super_block);
    u32 BlockGroupOffset = id * 32;
    u32 BlockGroupStart = BeginingOfSuperblock + (SuperblockSize / SectorSize) + (BlockGroupOffset / BlockSize);

    for (u32 i = BlockGroupStart; i < (BlockGroupStart + 1); i ++) {
        Disk::read_sector(*node.partition->disk, i);

        kout << "reading: " << i << endl;

        for (int e = 0; e < 512; e ++) {
            ((u8*)BlockGroup)[e + ((i - BlockGroupStart) * 512)] =
                    node.partition->disk->read_write_buffer[e + BlockGroupOffset];
        }
    }

    kout << "Block Group Info (" << id << ") :" << endl;
    kout << "\tblock_usage_bitmap       : " << BlockGroup->block_usage_bitmap << endl;
    kout << "\tinode_usage_bitmap       : " << BlockGroup->inode_usage_bitmap << endl;
    kout << "\tstarting_block_address   : " << BlockGroup->starting_block_address << endl;
    kout << "\tunallocated_blocks       : " << BlockGroup->unallocated_blocks << endl;
    kout << "\tunallocated_inodes       : " << BlockGroup->unallocated_inodes << endl;
    kout << "\tdirectories              : " << BlockGroup->directories << endl;
    kout << endl;

    return BlockGroup;
}

void System::fs::ext2::test_node(System::fs::fs_node_t node) {
    kout << "Testing node..." << endl;

    superblock_t *Superblock = read_superblock(node);

    u32 TotalNumberOfBlockGroups = Superblock->total_inods / Superblock->inodes_per_group;
    kout << "Total Number of Blocks Per Group: " << TotalNumberOfBlockGroups << endl;

    block_group_t* FirstBlockGroup = read_blockgroup(node, Superblock, 0);




}
