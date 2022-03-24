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


#include "ata.hpp"
#include <System/Port/port.hpp>
#include <System/cpu/cpu.hpp>
#include <System/memory/kmemory.hpp>
#include <System/memory/MemorySizes.hpp>
#include <System/Clock/RTC/RTC.hpp>
#include <System/Clock/PIT/PIT.hpp>
#include <System/Disk/vDisk.hpp>
#include <lib/StanderdOperations/Operations.h>

using namespace System;
using namespace System::IO;
using namespace System::Disk;

#define ATA_PRIMARY_IO 0x1F0
#define ATA_SECONDARY_IO 0x170

#define ATA_PRIMARY_DCR_AS 0x3F6
#define ATA_SECONDARY_DCR_AS 0x376

#define ATA_PRIMARY_IRQ 14
#define ATA_SECONDARY_IRQ 15

u8 *ide_buf = 0;



void PIO::ata_primary_irq(register_t * r) {


    CPU::PIC::SendEOI(ATA_PRIMARY_IRQ);
}

void PIO::ata_secondary_irq(register_t * r) {


    CPU::PIC::SendEOI(ATA_SECONDARY_IRQ);
}

void PIO::ide_select_drive(u8 Bus, u8 Device) {
    if (Bus == ATA_PRIMARY) {
        if (Device == ATA_MASTER){
            Port::byte_out(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, 0xA0);
        }
        else Port::byte_out(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, 0xB0);
    }
    else if (Bus == ATA_SECONDARY) {
        if (Device == ATA_MASTER) {
            Port::byte_out(ATA_SECONDARY_IO + ATA_REG_HDDEVSEL, 0xA0);
        }
        else Port::byte_out(ATA_SECONDARY_IO + ATA_REG_HDDEVSEL, 0xB0);
    }

}

u8 PIO::ide_check_device(u8 bus, u8 drive) {
    u16 io = 0;

    PIO::ide_select_drive(bus, drive);

    if (bus == ATA_PRIMARY) io = ATA_PRIMARY_IO;
    else io = ATA_SECONDARY_IO;

    // ATA specs say these values must be zero before sending IDENTIFY
    Port::byte_out(io + ATA_REG_SECCOUNT0, 0);
    Port::byte_out(io + ATA_REG_LBA0, 0);
    Port::byte_out(io + ATA_REG_LBA1, 0);
    Port::byte_out(io + ATA_REG_LBA2, 0);

    // Now, send IDENTIFY
    Port::byte_out(io + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);

    // Now, read status port
    u8 status = Port::byte_in(io + ATA_REG_STATUS);

    if (!status) return 0;

    // Now, poll until BSY is clear.
    while ((Port::byte_in(io + ATA_REG_STATUS) & ATA_SR_BSY) != 0);

    do {
        status = Port::byte_in(io + ATA_REG_STATUS);
        if (status & ATA_SR_ERR) {
            kout.printf("%s%s has ERROR BIT SET.\n", ((bus == ATA_PRIMARY) ? "Primary" : "Secondary"),
                        ((drive == ATA_PRIMARY) ? " master" : " slave"));
            return 0;
        }
    } while (!(status & ATA_SR_DRQ));

    kout.printf("%s%s is online.\n", ((bus == ATA_PRIMARY) ? "Primary" : "Secondary"),
                ((drive == ATA_PRIMARY) ? " master" : " slave"));

    // Now, actually read the data
    for (int i = 0; i < 256; i++) {
        *(u16 *) (ide_buf + i * 2) = Port::word_in(io + ATA_REG_DATA);
    }

    return 1;
}

void PIO::ide_400ns_delay(u16 io)
{
    for(int i = 0;i < 4; i++)
        Port::byte_in(io + ATA_REG_ALTSTATUS);
}

void PIO::ide_poll(u16 io) {
    PIO::ide_400ns_delay(io);

    u8 status = 0;

    do {
        status = Port::byte_in(io + ATA_REG_STATUS);

        if(status & ATA_SR_ERR) {
            kout.printf("ERR set, device failure!\n");
        }

    } while ((status & ATA_SR_BSY) && !(status & ATA_SR_DRQ));


}

u8 PIO::ata_read_one(u8 *buf, u32 lba, u8 drive) {

    u16 io = 0;
    switch(drive)
    {
        case (ATA_PRIMARY << 1 | ATA_MASTER):
            io = ATA_PRIMARY_IO;
            drive = ATA_MASTER;
            break;
        case (ATA_PRIMARY << 1 | ATA_SLAVE):
            io = ATA_PRIMARY_IO;
            drive = ATA_SLAVE;
            break;
        case (ATA_SECONDARY << 1 | ATA_MASTER):
            io = ATA_SECONDARY_IO;
            drive = ATA_MASTER;
            break;
        case (ATA_SECONDARY << 1 | ATA_SLAVE):
            io = ATA_SECONDARY_IO;
            drive = ATA_SLAVE;
            break;
        default:
            kout.printf("FATAL: unknown drive!\n");
            return 0;
    }

    u8 cmd = (drive==ATA_MASTER?0xE0:0xF0);
    u8 slavebit = (drive == ATA_MASTER?0x00:0x01);

    ide_select_drive(io, drive);


    Port::byte_out(io + ATA_REG_HDDEVSEL, (cmd | (u8)((lba >> 24 & 0x0F))));
    Port::byte_out(io + 1, 0x00);
    Port::byte_out(io + ATA_REG_SECCOUNT0, 1);
    Port::byte_out(io + ATA_REG_LBA0, (u8)((lba)));
    Port::byte_out(io + ATA_REG_LBA1, (u8)((lba) >> 8));
    Port::byte_out(io + ATA_REG_LBA2, (u8)((lba) >> 16));
    Port::byte_out(io + ATA_REG_COMMAND, ATA_CMD_READ_PIO);

    ide_poll(io);

    for(int i = 0; i < 256; i++)
    {
        u16 data = Port::word_in(io + ATA_REG_DATA);
        *(u16 *)(buf + i * 2) = data;
    }

    ide_400ns_delay(io);

    return 1;
}

void PIO::ata_write_one(u8* buf, u32 lba, u8 drive) {
    u16 io = 0;
    switch(drive)
    {
        case (ATA_PRIMARY << 1 | ATA_MASTER):
            io = ATA_PRIMARY_IO;
            drive = ATA_MASTER;
            break;
        case (ATA_PRIMARY << 1 | ATA_SLAVE):
            io = ATA_PRIMARY_IO;
            drive = ATA_SLAVE;
            break;
        case (ATA_SECONDARY << 1 | ATA_MASTER):
            io = ATA_SECONDARY_IO;
            drive = ATA_MASTER;
            break;
        case (ATA_SECONDARY << 1 | ATA_SLAVE):
            io = ATA_SECONDARY_IO;
            drive = ATA_SLAVE;
            break;
        default:
            kout.printf("FATAL: unknown drive!\n");
            return ;
    }

    ide_select_drive(io, drive);

    u8 cmd = (drive==ATA_MASTER?0xE0:0xF0);

    Port::byte_out(io + ATA_REG_HDDEVSEL, (cmd | (u8)((lba >> 24 & 0x0F))));
    Port::byte_out(io + 1, 0x00);
    Port::byte_out(io + ATA_REG_SECCOUNT0, 1);
    Port::byte_out(io + ATA_REG_LBA0, (u8)((lba)));
    Port::byte_out(io + ATA_REG_LBA1, (u8)((lba) >> 8));
    Port::byte_out(io + ATA_REG_LBA2, (u8)((lba) >> 16));
    Port::byte_out(io + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);

    //ide_poll(io);

    ide_400ns_delay(io);

    for(int i = 0; i < 256; i++) {
        u16 data = ((u16*)buf)[i];

        Port::word_out(io + ATA_REG_DATA, data);

        Port::word_out(io + ATA_REG_COMMAND, 0xE7);

    }

    ide_400ns_delay(io);

}

void PIO::ide_setup_device(u8 bus, u8 drive) {
    if (!PIO::ide_check_device(bus, drive)) return;

    char *str = (char *)Memory::kmalloc(40);
    for(int i = 0; i < 40; i += 2) {
        str[i] = ide_buf[ATA_IDENT_MODEL + i + 1];
        str[i + 1] = ide_buf[ATA_IDENT_MODEL + i];
    }

    for (int i = 40; i > 0; i --) {
        if (str[i] == ' ' || str[i] == '\0') {
            str[i] = '\0';
        }
        else {
            break;
        }
    }

    Disk::disk_t disk_type;

    disk_type = {
            str,
            .driver = Disk::DiskTypes::ATA_PIO,
            .disk_select = (u32)bus << 1 | (u32)drive,
            .disk_info = (u8*)System::Memory::kmalloc(512),
            .read_write_buffer = (u8*)System::Memory::kmalloc(512),
    };

    memcpy(disk_type.disk_info, ide_buf, 512);

    Disk::add_disk(disk_type);

}

void PIO::ide_init_all() {
    for (int bus = ATA_PRIMARY; bus <= ATA_SECONDARY; bus++) {
        for (int drive = ATA_MASTER; drive <= ATA_SLAVE; drive++) {
            PIO::ide_setup_device(bus, drive);
        }
    }
}

void PIO::init_driver() {
    kout << "Setting IRQ" << endl;
    CPU::IRQ::installIRQ(ATA_PRIMARY_IRQ, System::Disk::PIO::ata_primary_irq);
    CPU::IRQ::installIRQ(ATA_SECONDARY_IRQ, System::Disk::PIO::ata_secondary_irq);

    ide_buf = (u8 *)(Memory::kmalloc(512));

    kout << "Looking for all Disks.." << endl;

    PIO::ide_init_all();
}

void PIO::Disk_write_sector(disk_t disk, u32 lda) {
    PIO::ata_write_one(disk.read_write_buffer, lda, disk.disk_select);
}

void PIO::Disk_read_sector(disk_t disk, u32 lda) {
    PIO::ata_read_one(disk.read_write_buffer, lda, disk.disk_select);
}
