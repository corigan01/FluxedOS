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

#pragma once

#include <lib/core/core.h>
#include <System/Disk/vDisk.hpp>
#include "lib/vector/k_vector/k_vector.hpp"
#include <System/fs/vfs.hpp>
#include <lib/SmartPointer/smartpointer.hpp>

namespace System {
    namespace fs {
        namespace ext2 {

            typedef struct {
                u32 total_inods;
                u32 total_bocks;
                u32 reserved_blocks;
                u32 total_unalloc_blocks;
                u32 total_unalloc_inoodes;
                u32 block_of_super_block;
                u32 block_size_shift;
                u32 fragment_size_shift;
                u32 blocks_per_group;
                u32 fragments_per_group;
                u32 inodes_per_group;
                u32 last_mount_time;
                u32 last_written_time;
                u16 mounted_check;
                u16 allowed_check;
                u16 ext2sig;
                u16 file_system_state;
                u16 wtd_error;
                u16 minor_version;
                u32 last_check;
                u32 max_last_check;
                u32 osID;
                u32 major_version;
                u16 usrID;
                u16 GroupID;
                u32 FirstReservedInode;
                u16 inodeSize;
                u16 blockOfSuperblock;
            } superblock_t;

            typedef struct {
                u32 block_usage_bitmap;
                u32 inode_usage_bitmap;
                u32 starting_block_address;
                u16 unallocated_blocks;
                u16 unallocated_inodes;
                u16 directories;
                u32 reserved[3];
            } block_group_t;

            namespace inode_type {
                enum type {
                    FIFO = 0x1000,
                    CHARACTER_DEVICE = 0x2000,
                    DIRECTORY = 0x4000,
                    BLOCK_DEVICE = 0x6000,
                    REGULAR_FILE = 0x8000,
                    SYMBOLIC_LINK = 0xA000,
                    UNIX_SOCKET = 0xC000
                };
            }

            namespace inode_perms {
                enum perms {
                    OTHER_X = 0x001,
                    OTHER_W = 0x002,
                    OTHER_R = 0x004,
                    GROUP_X = 0x008,
                    GROUP_W = 0x010,
                    GROUP_R = 0x020,
                    USER_X = 0x040,
                    USER_W = 0x080,
                    USER_R = 0x100,
                    STICKY = 0x200,
                    GROUPID = 0x400,
                    USERID = 0x800
                };
            }

            typedef u8* block_t;

            typedef struct {
                u16 type_and_perms;
                u16 userID;
                u32 low_size;
                u32 last_access;
                u32 creation_time;
                u32 last_modification_time;
                u32 deletion_time;
                u16 group_id;
                u16 hard_links;
                u32 disk_sectors;
                u32 flags;
                u32 ossv1;
                u32 blocks[11];
                u32 singly_block_pointer;
                u32 doubly_block_pointer;
                u32 triply_block_pointer;
                u32 generation_number;
                u32 reserved1;
                u32 reserved2;
                u32 fragment_block_address;
                u32 ossv2[4];
            } inode_t;

            typedef struct {
                u32 inode;
                u16 totalsize;
                u8 nameleng;
                u8 typeind;
                char* name;
            } directory_t;

            bool test_node(fs::fs_node_t node);

            size_t num_of_dir_entries(directory_t parent);

        }
    }
}