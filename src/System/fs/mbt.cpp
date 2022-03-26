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

#include "mbt.hpp"
#include <System/kout/kout.hpp>
#include <System/memory/kmemory.hpp>
#include <lib/vector/KernelVector.hpp>

using namespace System;

K_Vector<fs::partition_t*>* partitions;

void System::fs::import_disk_partitions(System::Disk::disk_t disk) {

    if (partitions == nullptr) {
        partitions = new K_Vector<fs::partition_t*>();
    }

    Disk::read_sector(disk, 0);


    for (int e = 0; e < 4; e ++) {
        partition_t *partition = (partition_t*)Memory::kmalloc(sizeof(partition_t));

        for (int i = 446 + (e * 16); i < 446 + ((e + 1) * 16); i++) {
            ((u8*)partition)[i - (446 + (e * 16))] = disk.read_write_buffer[i];
        }

        if (partition->size_in_sectors > 0) {
            partition->disk = new Disk::disk_t ;
            memcpy(partition->disk, &disk, sizeof(Disk::disk_t));

            kout << endl;
            kout << "New Disk partition \'" << e << "\' on disk \"" << partition->disk->name << "\" : " << endl;
            kout << "  - Drive Attributes\t:  " << partition->DriveAttributes << endl;
            kout << "  - PartitionType   \t:  " << partition->PartitionType << "\t<-- " << (partition->PartitionType == 0x83 ? "Valid Linux Partition" : "Unknown Partition") << endl;
            kout << "  - Start Sector    \t:  " << partition->lba << endl;
            kout << "  - Size in Sectors \t:  " << partition->size_in_sectors << endl;
            kout << endl;


            partitions->push_back(partition);
        }
        else {
            Memory::kfree((void*)partition);
        }
    }

    kout << "Partitions Number: " << partitions->size() << endl;
}

K_Vector<fs::partition_t*>* System::fs::GetAllPartitions() {
    return partitions;
}


