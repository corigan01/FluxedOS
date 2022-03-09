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

namespace System {
    namespace fs {

        typedef struct {
            u8 low;
            u8 mid;
            u8 high;
        } chs_t;

        typedef struct {
            u8 DriveAttributes;
            chs_t CHS_address;
            u8 PartitionType;
            chs_t CHS_end_address;
            u32 lba;
            u32 size_in_sectors;

            Disk::disk_t* disk;
        } partition_t;

        void read_mbt_disk(Disk::disk_t disk);

        K_Vector<partition_t *> *GetAllPartitions(Disk::disk_t disk);
    }
}