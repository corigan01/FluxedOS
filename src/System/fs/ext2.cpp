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
    kout << "\tInode Size           : " << sb->inodeSize << endl;
    kout << "\tBlocks per Group     : " << sb->blocks_per_group << endl;
    kout << "\tShift Inode          : " << sb->total_unalloc_inoodes << endl;
    kout << "\tFile System ID       : 0x" << kout.ToHex(sb->ext2sig) << (sb->ext2sig == 0xEF53 ? "  <-- Valid Sig" : "  <-- NOT VALID") << endl;
    kout << endl;

    return sb;
}

block_group_t* read_blockgroup(fs::fs_node_t node, superblock_t* superblock, u32 id) {
    block_group_t* BlockGroup = (block_group_t *)Memory::kmalloc(32);

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

inode_t* read_inode(fs::fs_node_t node, superblock_t* superblock, u32 id) {

    inode_t * inode = (inode_t*)Memory::kmalloc(superblock->inodeSize);

    // First we need to figure out which block group contains our inode
    u32 ContainingBlockGroup = (id - 1) / superblock->inodes_per_group;

    // next we need to find where in the block group's bitmap would be our inode
    u32 InodeIndex = (id - 1) % superblock->inodes_per_group;

    // Finally we can get the block offset of that inode
    u32 BlockOffset = InodeIndex / (BlockSize / superblock->inodeSize);//((id * superblock->inodeSize) / BlockSize) - 1;

    // Now lets get the block group
    block_group_t * InodesBlockgroup = read_blockgroup(node, superblock, ContainingBlockGroup);

    // We can figure out the real block at which the inode is stored
    u32 InodeBlock = ((node.partition->lba * SectorSize) / BlockSize) + InodesBlockgroup->inode_usage_bitmap + BlockOffset;

    // We need the Inodes per block
    u32 InodesPerBlock = BlockSize / superblock->inodeSize;

    // next we need the offset that is inside that block
    u32 InodeIndexWithinBlock = (InodeIndex % InodesPerBlock) * superblock->inodeSize;


    kout << "Finding Inode: " << endl;
    kout << "\tInode ID       : " << id << endl;
    kout << "\tBlock Group    : " << ContainingBlockGroup << endl;
    kout << "\tInode Index    : " << InodeIndex << endl;
    kout << "\tBlock of Inode : " << BlockOffset << endl;
    kout << "\tIndex          : " << InodeIndexWithinBlock << endl;
    kout << endl;

    // Read the disk
    u32 Sector = InodeBlock * (BlockSize / SectorSize) + (InodeIndexWithinBlock / SectorSize);

    Disk::read_sector(*node.partition->disk, Sector);

    InodeIndexWithinBlock = InodeIndexWithinBlock % 512;

    for (int i = 0; i < 512; i ++) {
        kout << ((i >= InodeIndexWithinBlock && (i < InodeIndexWithinBlock + superblock->inodeSize)) ? kout.GREEN : kout.YELLOW )<< "  " << node.partition->disk->read_write_buffer[i];

        if (i >= InodeIndexWithinBlock && i < InodeIndexWithinBlock + superblock->inodeSize) {
            ((u8*)inode)[i - InodeIndexWithinBlock] = node.partition->disk->read_write_buffer[i];
        }
    }
    kout << kout.YELLOW << endl;

    const char* Type[] = {
            "FIFO",
            "Character Device",
            "Directory",
            "Block Device",
            "Regular File",
            "Symbolic Link",
            "Unix Socket",
            "UNKNOWN"
    };

    u32 HighNode = inode->type_and_perms & 0xF000;
    u32 typeIndex = 0;

    switch (HighNode) {
        case inode_type::FIFO:
            typeIndex = 0;
            break;
        case inode_type::CHARACTER_DEVICE:
            typeIndex = 1;
            break;
        case inode_type::DIRECTORY:
            typeIndex = 2;
            break;
        case inode_type::BLOCK_DEVICE:
            typeIndex = 3;
            break;
        case inode_type::REGULAR_FILE:
            typeIndex = 4;
            break;
        case inode_type::SYMBOLIC_LINK:
            typeIndex = 5;
            break;
        case inode_type::UNIX_SOCKET:
            typeIndex = 6;
            break;
        default:
            typeIndex = 7;
            break;
    }


    kout << "Inode: " << endl;
    kout << "  Type: " << Type[typeIndex] << endl;
    kout << "  UserID: " << inode->userID << endl;
    kout << "  low_size: " << inode->low_size << endl;
    kout << "  disk sectors: " << inode->disk_sectors << endl;
    kout << "  flags: " << inode->flags << endl;
    kout << "  fragment: " << inode->fragment_block_address << endl;


    return inode;
}

void System::fs::ext2::test_node(System::fs::fs_node_t node) {
    kout << "Testing node..." << endl;

    superblock_t *Superblock = read_superblock(node);

    u32 TotalNumberOfBlockGroups = Superblock->total_inods / Superblock->inodes_per_group;
    kout << "Total Number of Block Groups: " << TotalNumberOfBlockGroups << endl;

    read_inode(node, Superblock, 11);

    /*kout << "Calculating Inode: " << endl;

    Superblock->inodeSize;

    u32 InodeID = 11;
    u32 BlockGroup = (InodeID - 1) / Superblock->inodes_per_group;
    u32 InodeIndex = (InodeID - 1) % Superblock->inodes_per_group;
    u32 BlockOfInode = ((InodeIndex * Superblock->inodeSize) / BlockSize) ;

    kout << "Finding Inode: " << endl;
    kout << "\tInode ID       : " << InodeID << endl;
    kout << "\tBlock Group    : " << BlockGroup << endl;
    kout << "\tInode Index    : " << InodeIndex << endl;
    kout << "\tBlock of Inode : " << BlockOfInode << endl;
    kout << endl;

    u32 BeginingOfPartition = node.partition->lba;

    block_group_t* FirstBlockGroup = read_blockgroup(node, Superblock, BlockGroup);
    kout << "Block Group --> (*" << FirstBlockGroup->inode_usage_bitmap << " + " <<  BlockOfInode << ")[" << InodeIndex << "]" << endl;

    u32 SectorRead = (((FirstBlockGroup->inode_usage_bitmap + (BlockOfInode - 1)) * BlockSize) / SectorSize) + BeginingOfPartition;

    Disk::read_sector(*node.partition->disk, SectorRead);

    InodeIndex -= (BlockOfInode * 2) * (SectorSize / Superblock->inodeSize);

    kout << "index: " << InodeIndex * Superblock->inodeSize << endl;

    kout << "Reading Sector: " << SectorRead << "  Block: " << BlockOfInode + FirstBlockGroup->inode_usage_bitmap << endl;

    inode_t *inode = (inode_t*)Memory::kmalloc(Superblock->inodeSize);

    for (int i = 0; i < 512; i ++) {
        kout << ((i > (InodeIndex * Superblock->inodeSize) && i < (InodeIndex * Superblock->inodeSize + Superblock->inodeSize)) ? kout.GREEN : kout.YELLOW) << "  " << node.partition->disk->read_write_buffer[i];

        if ((i > (InodeIndex * Superblock->inodeSize) && i < (InodeIndex * Superblock->inodeSize + Superblock->inodeSize))) {
            ((u8*)inode)[ i - (InodeIndex * Superblock->inodeSize)] =  node.partition->disk->read_write_buffer[i];
        }
    }

    kout << kout.YELLOW << endl;

    const char* Type[] = {
            "FIFO",
            "Character Device",
            "Directory",
            "Block Device",
            "Regular File",
            "Symbolic Link",
            "Unix Socket",
            "UNKNOWN"
            };

    u32 HighNode = inode->type_and_perms & 0xF000;
    u32 typeIndex = 0;

    switch (HighNode) {
        case inode_type::FIFO:
            typeIndex = 0;
            break;
        case inode_type::CHARACTER_DEVICE:
            typeIndex = 1;
            break;
        case inode_type::DIRECTORY:
            typeIndex = 2;
            break;
        case inode_type::BLOCK_DEVICE:
            typeIndex = 3;
            break;
        case inode_type::REGULAR_FILE:
            typeIndex = 4;
            break;
        case inode_type::SYMBOLIC_LINK:
            typeIndex = 5;
            break;
        case inode_type::UNIX_SOCKET:
            typeIndex = 6;
            break;
        default:
            typeIndex = 7;
            break;
    }


    kout << "Inode: " << endl;
    kout << "  Type: " << Type[typeIndex] << endl;
    kout << "  UserID: " << inode->userID << endl;
    kout << "  low_size: " << inode->low_size << endl;
    kout << "  disk sectors: " << inode->disk_sectors << endl;
    kout << "  flags: " << inode->flags << endl;
    kout << "  fragment: " << inode->fragment_block_address << endl;

    */


}
