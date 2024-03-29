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
#include <lib/vector/KernelVector.hpp>

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

void print_block(block_t block, u32 highlightbegin = 0, u32 highlightzone = 0) {
    kout << "Block: " << endl;
    for (int i = 0; i < 64; i++) {
        kout << kout.ToHex(i * 16) << "  | ";

        for (int e = 0; e < 16; e++) {

            u32 address = (i * 16) + e;

            char ch = (char)block[address];

            if (ch == 0) kout << kout.YELLOW;
            else if (ch == ' ') kout << kout.GREEN;
            else if (ch < '!') kout << kout.BLUE;
            else kout << kout.GREEN;

            if (address >= highlightbegin && address < highlightzone) {
                kout << kout.BOLD_MAGENTA;
            }

            kout << " " << kout.ToSmallHex(block[(i * 16) + e]);

            kout << kout.YELLOW;
        }

        kout << "   |   >";

        for (int e = 0; e < 16; e++) {
            char ch = (char)block[(i * 16) + e];

            if (!ch) ch = '.';
            else if (ch == ' ') kout << kout.GREEN;
            else if (ch == '\n') {
                ch = '~';
                kout << kout.BLUE;
            }
            else if (ch < '!') {
                ch = '.';
                kout << kout.BLUE;
            }
            else kout << kout.GREEN;

            kout << " " << ch;

            kout << kout.YELLOW;
        }

        kout << " <" << endl;
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

    return sb;
}



block_group_t* read_blockgroup(fs::fs_node_t node, superblock_t* superblock, u32 id) {
    auto* BlockGroup = (block_group_t *)Memory::kmalloc(sizeof(block_group_t));
    block_t block = read_block(node, superblock->block_of_super_block + 1);

    memtrfs(BlockGroup, block, sizeof(block_group_t) * id, sizeof(block_group_t));

    Memory::kfree(block);

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

    return inode;
}

directory_t *read_dir(fs::fs_node_t node, superblock_t* superblock, inode_t* inode, u32 offset) {
    block_t dirblock = read_block(node, inode->blocks[0]);

    if (inode->low_size <= 0 || (inode->type_and_perms & 0xF000) != inode_type::DIRECTORY) {
        kout << "Not a Directory!" << endl;
        return {};

    }

    kout << "Size: " << inode->low_size << endl;

    auto *dir = (directory_t*)Memory::kmalloc(inode->low_size);
    memtrfs(dir, dirblock, offset, inode->low_size);

    char* name = (char*) Memory::kmalloc(dir->nameleng + 1);
    memtrfs(name, dir, 8 + offset, dir->nameleng);

    name[dir->nameleng] = '\0';
    dir->name = name;

    for (int i = 0; i < sizeof(directory_t); i++) {
        kout << " " << kout.ToHex(((u8*)dir)[i]);
    }

    Memory::kfree(dirblock);

    return dir;
}

K_Vector<directory_t*> parse_dir(fs::fs_node_t node, superblock_t* superblock, inode_t* inode) {
    block_t block = read_block(node, inode->blocks[0]);

    K_Vector<directory_t*> dir_entries;

    auto* dir = (directory_t*)Memory::kmalloc(inode->low_size);

    for (int i = 0; i < inode->low_size;) {

        // TODO: FIX ME
        memtrfs(dir, block, i, inode->low_size - i);
        memtrfs(dir, dir, 0, dir->totalsize);

        if (dir->totalsize == 0) break;
        i += dir->totalsize;

        char* name = (char*) Memory::kmalloc(dir->nameleng + 1);
        memtrfs(name, dir, 8, dir->nameleng);

        name[dir->nameleng] = '\0';
        dir->name = name;

        /*kout << "Dir: " << endl;
        kout << "\tinode: " << dir->inode << endl;
        kout << "\ttotal size: " << dir->totalsize << endl;
        kout << "\tnamelng: " << dir->nameleng << endl;
        kout << "\ttypeind: " << dir->typeind << endl;
        kout << "\tname: " << dir->name << endl;*/

        dir_entries.push_back(dir);
        dir = (directory_t*)Memory::kmalloc(inode->low_size);
    }


    Memory::kfree(dir);
    Memory::kfree(block);

    return dir_entries;
}

K_Vector<directory_t*> ext2::get_root_directory(System::fs::fs_node_t node) {
    superblock_t *Superblock = read_superblock(node);
    inode_t *root_inode = read_inode(node, Superblock, RootDirectoryInode);

    auto returningVector = parse_dir(node, Superblock, root_inode);

    /*kout << "\nRoot Dir:" << endl;
    for (int i = 0; i < returningVector.size(); i++) {
        kout << "/" << returningVector[i]->name << endl;
    }
    kout << endl;*/

    Memory::kfree(Superblock);
    Memory::kfree(root_inode);

    return returningVector;
}

K_Vector<directory_t*> ext2::get_directories(System::fs::fs_node_t node, directory_t* parent) {
    superblock_t* superblock = read_superblock(node);
    inode_t* inode = read_inode(node, superblock, parent->inode);

    auto parsed_directory = parse_dir(node, superblock, inode);

    Memory::kfree(superblock);
    Memory::kfree(inode);

    return parsed_directory;
}

bool System::fs::ext2::test_node(System::fs::fs_node_t node) {
    kout << "[EXT2]: Testing node..." << endl;

    // Get the Superblock
    superblock_t *Superblock = read_superblock(node);

    // Test condition
    bool superblockSuccess =  Superblock->ext2sig == 0xEF53;

    // Give the memory back
    Memory::kfree(Superblock);

    // return final output
    return superblockSuccess;
}

K_Vector<directory_t*> System::fs::ext2::get_entries_from_path(path_t path, fs_node_t node) {
    auto root = get_root_directory(node);
    auto split_up_fs = fs::PathToVector(path);

    for (int i = 0; i < split_up_fs.size(); i++) {
        kout << "SPLIT: " << split_up_fs[i] << endl;
    }

    bool found_next_entry = false;
    if (split_up_fs.size() == 0) {
        kout << "Root dir!" << endl;

    } else {
        for (int i = 0; i < split_up_fs.size(); i++) {
            for (int e = 0; e < root.size(); e++) {
                found_next_entry = false;
                if (strcmp(split_up_fs[i], root[e]->name) == 0) {

                    auto new_root = get_directories(node, root[e]);

                    for (size_t f = 0; f < root.size(); f++) {
                        Memory::kfree(root[f]->name);
                        Memory::kfree(root[f]);
                    }
                    root.delete_all();


                    for (size_t f = 0; f < new_root.size(); f++) {
                        root.push_back(new_root[f]);
                    }

                    found_next_entry = true;
                    break;
                }
            }

            if (!found_next_entry) {
                kout << "Could not find entry \"" << split_up_fs[i] << "\" in our directory!" << endl;

                return {};
            }
        }
    }

    for (int i = 0; i < split_up_fs.size(); i++) {
        Memory::kfree(split_up_fs[i]);
    }

    return root;
}

directory_t* get_entry_from_path(path_t path, fs_node_t node) {
    path_t file_name;
    size_t begin = 0;

    for (size_t i = strlen(path) - 2; i >= 0; i--) {
        if (path[i] == '/') {
            path[i + 1] = '\0';
            begin = i + 1;
            kout << "path: " << path << " begin: " << begin << endl;
            break;
        }
    }
    file_name = (path_t) Memory::kmalloc((strlen(path) - begin) * sizeof(path_t));

    for (size_t i = begin; i < strlen(path) - 1; i++) {
        file_name[i - begin] = path[i];
    }

    kout << "Directory: \"" << path << "\" File name: \"" << file_name << "\" " << endl;

    directory_t* dir = nullptr;

    if (strcmp(path, "/") == 0) {
        kout << "Working in a root dir" << endl;
        file_name = "..";
    }

    auto entries = get_entries_from_path(path, node);

    kout << entries.size() << endl;

    for (size_t i = 0; i < entries.size(); i++) {
        kout << "Found Entry: " << entries[i]->name << endl;
        if (strcmp(entries[i]->name, file_name) == 0) {
            dir = entries[i];
        }
    }

    for (size_t i = 0; i < entries.size(); i++) {
        if (entries[i] != dir) {
            Memory::kfree(entries[i]->name);
            Memory::kfree(entries[i]);
        }
    }

    Memory::kfree(file_name);

    return dir;
}


vfs::vfs_response_t ext2::FileSystemServer (void* prv_data, fs_node_t node, vfs::request::type request,
                                      vfs::request::buffer_t buffer) {

    auto private_data = (ext2::ext2_info_t*)prv_data;

    // INIT REQUEST
    if (request == vfs::request::INIT) {
        auto is_ext2fs = ext2::test_node(node);

        if (is_ext2fs) {
            private_data->superblock = read_superblock(node);
            private_data->current_directory_inode = read_inode(node, private_data->superblock, RootDirectoryInode);
            private_data->refresh_event = false; // only true on write events so the next read event can re-read the blocks

            return {vfs::request::VALID};
        }
        else {
            return {vfs::request::NOT_VALID};
        }
    }
    // List files REQUEST
    else if (request == vfs::request::LIST_ENTRIES) {

        vfs::vfs_response_t response = {vfs::request::OK};

        auto root = get_entries_from_path((path_t)buffer.storage, node);

        auto *resp = (vfs::request::buffer_t *) Memory::kmalloc(
                sizeof(vfs::request::buffer_t) * root.size() + sizeof(size_t));
        for (int i = 0; i < root.size(); i++) {
            resp[i].size = root[i]->nameleng + 1;
            resp[i].storage = (u8 *) Memory::kmalloc(resp[i].size);

            memcpy(resp[i].storage, root[i]->name, resp[i].size);
        }

        response.buffer.storage = (u8 *) resp;
        response.buffer.size = root.size();

        for (size_t i = 0; i < root.size(); i++) {
            Memory::kfree(root[i]->name);
            Memory::kfree(root[i]);
        }

        return response;
    }
    else if (request == vfs::request::GET_ENTRY) {
        vfs::vfs_response_t response = {vfs::request::OK};

        auto entry = get_entry_from_path((path_t)buffer.storage, node);

        ASSERT(entry != nullptr);

        auto inode_info = read_inode(node, private_data->superblock, entry->inode);

        auto file_info = (vfs::vfs_file_info_t*) Memory::kmalloc(sizeof(vfs::vfs_file_info_t));

        file_info->size = inode_info->low_size;
        file_info->path = (path_t)buffer.storage;
        file_info->fileName = entry->name;

        switch (inode_info->type_and_perms) {
            case ext2::inode_type::DIRECTORY:
                inode_info->type_and_perms = vfs::request::DIR;
                break;
            case ext2::inode_type::REGULAR_FILE:
                inode_info->type_and_perms = vfs::request::FILE;
                break;
            default:
                inode_info->type_and_perms = vfs::request::UNKNOWN;
                break;
        }

        response.buffer.storage = (u8*)file_info;
        response.buffer.size = sizeof(vfs::vfs_file_info_t);

        return response;
    }
    else {
        kout << "[EXT2]: Request Not Supported!" << endl;
        return {vfs::request::BAD_REQUEST};
    }


    return {vfs::request::ERROR};
}