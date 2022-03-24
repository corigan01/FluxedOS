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

void memtrfs(void* des, void* src, u32 offset, u32 amount) {
    for (int i = 0; i < amount; i++) {
        ((u8*)des)[i] = ((u8*)src)[i + offset];
    }
}

void print_block(block_t block) {
    kout << "Block: " << endl;
    for (int i = 0; i < 64; i++) {
        for (int e = 0; e < 16; e++) {
            kout << " " << kout.ToSmallHex(block[(i * 64) + e]);
        }

        kout << "\t|\t>";

        for (int e = 0; e < 16; e++) {
            char ch = (char)block[(i * 64) + e];

            if (!ch) ch = ' ';
            else if (ch == '\n') ch = '~';
            //else if (ch > 'z') ch = ' ';
            else if (ch < '!') ch = ' ';

            kout << " " << ch;
        }

        kout << endl;
    }
    kout << endl;
}

block_t read_block(fs::fs_node_t node, u32 offset) {
    u32 sectorOffset = node.partition->lba + (offset * (BlockSize / SectorSize));

    auto Block = (u8*) Memory::kmalloc(BlockSize);

    for (u32 i = sectorOffset; i < (sectorOffset + (BlockSize / SectorSize)); i++) {
        Disk::read_sector(*node.partition->disk, i);

        for (int e = 0; e < SectorSize; e++) {
            Block[e + ((i - sectorOffset) * SectorSize)] = node.partition->disk->read_write_buffer[e];
        }
    }

    return Block;
}

superblock_t* read_superblock(fs::fs_node_t node) {

    auto *sb = (superblock_t*) read_block(node, 1);

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
    auto* BlockGroup = (block_group_t *)Memory::kmalloc(sizeof(block_group_t));
    block_t block = read_block(node, superblock->block_of_super_block + 1);

    memtrfs(BlockGroup, block, sizeof(block_group_t) * id, sizeof(block_group_t));

    Memory::kfree(block);

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

    auto * inode = (inode_t*)Memory::kmalloc(superblock->inodeSize);

    // First we need to figure out which block group contains our inode
    u32 ContainingBlockGroup = (id - 1) / superblock->inodes_per_group;

    // next we need to find where in the block group's bitmap would be our inode
    u32 InodeIndex = (id - 1) % superblock->inodes_per_group;

    // Finally, we can get the block offset of that inode
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
    kout << "\tReading Block  : " << InodesBlockgroup->inode_usage_bitmap + BlockOffset << endl;
    kout << endl;

   block_t block = read_block(node, InodesBlockgroup->starting_block_address + BlockOffset);

   memtrfs(inode, block, (InodeIndex % (BlockSize / superblock->inodeSize)) * superblock->inodeSize, superblock->inodeSize);

    Memory::kfree(block);
    Memory::kfree(InodesBlockgroup);

    constexpr char* Type[] = {
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
    kout << "  Type         : " << Type[typeIndex] << endl;
    kout << "  UserID       : " << inode->userID << endl;
    kout << "  low_size     : " << inode->low_size << endl;
    kout << "  disk sectors : " << inode->disk_sectors << endl;
    kout << "  flags        : " << inode->flags << endl;
    kout << "  fragment     : " << inode->fragment_block_address << endl;
    kout << "  blocks 0     : " << inode->blocks[0] << endl;
    kout << "  blocks 1     : " << inode->blocks[1] << endl;
    kout << "  blocks 2     : " << inode->blocks[2] << endl;
    kout << "  blocks 3     : " << inode->blocks[3] << endl;
    kout << "  blocks 4     : " << inode->blocks[4] << endl;
    kout << "  blocks 5     : " << inode->blocks[5] << endl;
    kout << "  blocks 6     : " << inode->blocks[6] << endl;
    kout << "  blocks 7     : " << inode->blocks[7] << endl;
    kout << "  blocks 8     : " << inode->blocks[8] << endl;
    kout << "  blocks 9     : " << inode->blocks[9] << endl;
    kout << "  blocks 10    : " << inode->blocks[10] << endl;

    return inode;
}

directory_t *read_dir(fs::fs_node_t node, superblock_t* superblock, inode_t* inode) {
    block_t dirblock = read_block(node, inode->blocks[0]);

    if (inode->low_size <= 0 || (inode->type_and_perms & 0xF000) != inode_type::DIRECTORY) {
        kout << "Not a Directory!" << endl;
        return {};

    }

    kout << "Size: " << inode->low_size << endl;

    auto *dir = (directory_t*)Memory::kmalloc(inode->low_size);

    memtrfs(dir, dirblock, 0, inode->low_size);

    char* name = (char*) Memory::kmalloc(dir->nameleng + 1);
    memtrfs(name, dir, 8, dir->nameleng);

    name[dir->nameleng] = '\0';
    dir->name = name;

    for (int i = 0; i < sizeof(directory_t); i++) {
        kout << " " << kout.ToHex(((u8*)dir)[i]);
    }

    kout << "Dir: " << endl;
    kout << "\tinode: " << dir->inode << endl;
    kout << "\ttotal size: " << dir->totalsize << endl;
    kout << "\tnamelng: " << dir->nameleng << endl;
    kout << "\ttypeind: " << dir->typeind << endl;
    kout << "\tname: \"" << dir->name << "\"" << endl;

    Memory::kfree(dirblock);

    return dir;
}

void System::fs::ext2::test_node(System::fs::fs_node_t node) {
    kout << "Testing node..." << endl;

    superblock_t *Superblock = read_superblock(node);

    u32 TotalNumberOfBlockGroups = Superblock->total_inods / Superblock->inodes_per_group;
    kout << "Total Number of Block Groups: " << TotalNumberOfBlockGroups << endl;

    inode_t * inode = read_inode(node, Superblock, 13);

    block_t block = read_block(node, inode->blocks[0]);
    print_block(block);
    Memory::kfree(block);

    directory_t* dir = read_dir(node, Superblock, inode);

    Memory::kfree(Superblock);
    Memory::kfree(inode);
    Memory::kfree(dir);



}
