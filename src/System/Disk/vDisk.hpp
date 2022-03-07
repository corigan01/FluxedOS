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

namespace System {
    namespace Disk {

        namespace DiskTypes {
            enum Driver {
              ATA_PIO = 0,
              ATA_DMA
            };

            enum Disk {
                IDE_PRIMARY     = 0,
                IDE_SECONDARY   = 1,
                IDE_MASTER      = 0,
                IDE_SLAVE       = 1
            };
        }

        typedef struct {
            // Name of te disk
            const char* name;

            // ID of the disk to the system (EX "sda")
            char id[4];

            // Which driver to use
            DiskTypes::Driver driver;

            // Selecting the disk
            u32 disk_select;

            // if the driver needs to store info about the disk
            u8* disk_info;

            // the data used for read and write
            u8* read_write_buffer;
        } disk_t;

        void init_all_disks();
        disk_t get_disk(u8 disk);
        u8 disk_count();

        void write_sector(disk_t disk, u32 lda);
        void read_sector(disk_t disk, u32 lda);

        void add_disk(disk_t disk);

    }
}
