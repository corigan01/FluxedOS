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
#include "vDisk.hpp"
#include <System/kout/kout.hpp>
#include <lib/vector/KernelVector.hpp>
#include <System/Disk/ata.hpp>

using namespace System;
using namespace System::Memory;

K_Vector<Disk::disk_t> disks;

void System::Disk::init_all_disks() {

    // Global Constructors don't work in the kernel for memory management,
    // so we need to actually init it.
    disks.ChangePointer(kmalloc(1000 * sizeof(Disk::disk_t) + 10 * sizeof(Disk::disk_t)));

    kout << "\nStarting Disk lookup..." << endl;

    PIO::init_driver();
}

System::Disk::disk_t System::Disk::get_disk(u8 disk) {
    return disks[disk];
}

u8 System::Disk::disk_count() {
    return disks.size();
}

void System::Disk::add_disk(disk_t disk) {

    kout << "\n\nSystem has detected a new Disk: " << endl;
    kout << "\t - Name: " << disk.name << endl;
    kout << "\t - Data: 0x" << kout.ToHex((u32)disk.disk_info) << endl;
    kout << "\t - Disk: 0x" << kout.ToHex((u32)disk.disk_select) << endl;
    kout << endl;

    disks.push_back(disk);
}

void Disk::write_sector(Disk::disk_t disk, u32 lda) {
    if (((u32)disk.read_write_buffer) == 0) {
        kout << "Disk write must contain a valid buffer" << endl;
    }

    switch (disk.driver){
        case DiskTypes::ATA_PIO:
            PIO::Disk_write_sector(disk, lda);
            return;
        case DiskTypes::ATA_DMA:
        default:
            kout << "ERROR Disk: " << disk.name << " does not have a supported Driver type \'" << disk.driver << "\'"
                 << endl;

            break;
    }
}

void Disk::read_sector(Disk::disk_t disk, u32 lda) {

    memset(disk.read_write_buffer, 0x00, 512);

    switch (disk.driver){
        case DiskTypes::ATA_PIO:
            PIO::Disk_read_sector(disk, lda);
            break;
        case DiskTypes::ATA_DMA:
        default:
            kout << "ERROR Disk: " << disk.name << " does not have a supported Driver type \'" << disk.driver << "\'"
                 << endl;

            break;
    }

    if (((u32)disk.read_write_buffer) == 0) {
        kout << "Disk read did not return a valid buffer" << endl;
    }
}
