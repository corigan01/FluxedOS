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
            } superblock_t;

            void test_node(fs::fs_node_t node);

        }
    }
}