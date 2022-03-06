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


using namespace System;
using namespace System::IO;

#define ATA_PRIMARY_IO 0x1F0
#define ATA_SECONDARY_IO 0x170

#define ATA_PRIMARY_DCR_AS 0x3F6
#define ATA_SECONDARY_DCR_AS 0x376

#define ATA_PRIMARY_IRQ 14
#define ATA_SECONDARY_IRQ 15

u8 *ide_buf = 0;

void Disk::ata_primary_irq(register_t * r) {


    CPU::PIC::SendEOI(ATA_PRIMARY_IRQ);
}

void Disk::ata_secondary_irq(register_t * r) {


    CPU::PIC::SendEOI(ATA_SECONDARY_IRQ);
}

void Disk::ide_select_drive(u8 Bus, u8 Device) {
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

u8 Disk::ide_check_device(u8 bus, u8 drive) {
    u16 io = 0;

    Disk::ide_select_drive(bus, drive);

    if (bus == ATA_PRIMARY) io = ATA_PRIMARY_IO;
    else io = ATA_SECONDARY_IO;

    // ATA specs say these values must be zero before sending IDENTIFY
    Port::byte_out(io + ATA_REG_SECCOUNT0, 0);
    Port::byte_out(io + ATA_REG_LBA0, 0);
    Port::byte_out(io + ATA_REG_LBA1, 0);
    Port::byte_out(io + ATA_REG_LBA2, 0);

    // Now, send IDENTIFY
    Port::byte_out(io + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
    kout.printf("Sent IDENTIFY\n");

    // Now, read status port
    u8 status = Port::byte_in(io + ATA_REG_STATUS);

    if (!status) return 0;

    // Now, poll until BSY is clear.
    while ((Port::byte_in(io + ATA_REG_STATUS) & ATA_SR_BSY) != 0);

    do {
        status = Port::byte_in(io + ATA_REG_STATUS);
        if (status & ATA_SR_ERR) {
            kout.printf("%s%s has ERR set. Disabled.\n", ((bus == ATA_PRIMARY) ? "Primary" : "Secondary"),
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

void Disk::init_driver() {
    kout << "Setting IRQ" << endl;
    CPU::IRQ::installIRQ(ATA_PRIMARY_IRQ, System::Disk::ata_primary_irq);
    CPU::IRQ::installIRQ(ATA_SECONDARY_IRQ, System::Disk::ata_secondary_irq);

    kout << "Looking for all Disks.." << endl;

    ide_buf = (u8 *)(Memory::kmalloc(sizeof(u8) * 512));

    kout << (ide_check_device(ATA_PRIMARY, ATA_MASTER) > 0 ? "Found ATA Primary Master!" : "NO PRIMARY MASTER DISK") << endl;
    kout << (ide_check_device(ATA_PRIMARY, ATA_SLAVE) > 0 ? "Found ATA Primary Slave!" : "NO PRIMARY SLAVE DISK") << endl;
    kout << (ide_check_device(ATA_SECONDARY, ATA_MASTER) > 0 ? "Found ATA Secondary Master!" : "NO SECONDARY MASTER DISK") << endl;
    kout << (ide_check_device(ATA_SECONDARY, ATA_SLAVE) > 0 ? "Found ATA Secondary Slave!" : "NO SECONDARY SLAVE DISK") << endl;

    char *str = (char *)Memory::kmalloc(40);
    for(int i = 0; i < 40; i += 2) {
        str[i] = ide_buf[ATA_IDENT_MODEL + i + 1];
        str[i + 1] = ide_buf[ATA_IDENT_MODEL + i];
    }

    kout << "Drive: " << str << endl;
}


/*#define ATA_PRIMARY_IO 0x1F0
#define ATA_SECONDARY_IO 0x170

#define ATA_PRIMARY_DCR_AS 0x3F6
#define ATA_SECONDARY_DCR_AS 0x376

#define ATA_PRIMARY_IRQ 14
#define ATA_SECONDARY_IRQ 15


u8 ata_pm = 0; // Primary master exists? 
u8 ata_ps = 0; // Primary Slave exists? 
u8 ata_sm = 0; // Secondary master exists? 
u8 ata_ss = 0; // Secondary slave exists? 

u8 *ide_buf = 0;

void ide_select_drive(u8 bus, u8 i)
{
	if(bus == ATA_PRIMARY)
		if(i == ATA_MASTER)
			Port::byte_out(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, 0xA0);
		else Port::byte_out(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, 0xB0);
	else
		if(i == ATA_MASTER)
			Port::byte_out(ATA_SECONDARY_IO + ATA_REG_HDDEVSEL, 0xA0);
		else Port::byte_out(ATA_SECONDARY_IO + ATA_REG_HDDEVSEL, 0xB0);
}

void ide_primary_irq()
{
	IRQ_START;
	send_eoi(ATA_PRIMARY_IRQ);
	IRQ_END;
}

void ide_secondary_irq()
{
	IRQ_START;
	send_eoi(ATA_SECONDARY_IRQ);
	IRQ_END;
}

u8 ide_identify(u8 bus, u8 drive)
{
	u16 io = 0;
	ide_select_drive(bus, drive);
	if(bus == ATA_PRIMARY) io = ATA_PRIMARY_IO;
	else io = ATA_SECONDARY_IO;
	// ATA specs say these values must be zero before sending IDENTIFY 
	Port::byte_out(io + ATA_REG_SECCOUNT0, 0);
	Port::byte_out(io + ATA_REG_LBA0, 0);
	Port::byte_out(io + ATA_REG_LBA1, 0);
	Port::byte_out(io + ATA_REG_LBA2, 0);
	// Now, send IDENTIFY 
	Port::byte_out(io + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
	kout.printf("Sent IDENTIFY\n");
	// Now, read status port 
	u8 status = Port::byte_in(io + ATA_REG_STATUS);
	if(status)
	{
		// Now, poll untill BSY is clear. 
		while(Port::byte_in(io + ATA_REG_STATUS) & ATA_SR_BSY != 0) ;
pm_stat_read:		status = Port::byte_in(io + ATA_REG_STATUS);
		if(status & ATA_SR_ERR)
		{
			kout.printf("%s%s has ERR set. Disabled.\n", bus==ATA_PRIMARY?"Primary":"Secondary", drive==ATA_PRIMARY?" master":" slave");
			return 0;
		}
		while(!(status & ATA_SR_DRQ)) goto pm_stat_read;
		kout.printf("%s%s is online.\n", bus==ATA_PRIMARY?"Primary":"Secondary", drive==ATA_PRIMARY?" master":" slave");
		// Now, actually read the data 
		set_task(0);
		for(int i = 0; i<256; i++)
		{
			*(u16 *)(ide_buf + i*2) = inportw(io + ATA_REG_DATA);
		}
		set_task(1);
	}
}

void ide_400ns_delay(u16 io)
{
	for(int i = 0;i < 4; i++)
		Port::byte_in(io + ATA_REG_ALTSTATUS);
}

void ide_poll(u16 io)
{
	
	for(int i=0; i< 4; i++)
		Port::byte_in(io + ATA_REG_ALTSTATUS);

retry:;
	u8 status = Port::byte_in(io + ATA_REG_STATUS);
	//kout.printf("testing for BSY\n");
	if(status & ATA_SR_BSY) goto retry;
	//kout.printf("BSY cleared\n");
retry2:	status = Port::byte_in(io + ATA_REG_STATUS);
	if(status & ATA_SR_ERR)
	{
		kout.printf("ERR set, device failure!\n");
	}
	//kout.printf("testing for DRQ\n");
	if(!(status & ATA_SR_DRQ)) goto retry2;
	//kout.printf("DRQ set, ready to PIO!\n");
	return;
}

u8 ata_read_one(u8 *buf, u32 lba, device_t *dev)
{
	//lba &= 0x00FFFFFF; // ignore topmost byte
	// We only support 28bit LBA so far 
	u8 drive = ((ide_private_data *)(dev->priv))->drive;
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
	//kout.printf("io=0x%x %s\n", io, drive==ATA_MASTER?"Master":"Slave");
	u8 cmd = (drive==ATA_MASTER?0xE0:0xF0);
	u8 slavebit = (drive == ATA_MASTER?0x00:0x01);
	kout.printf("LBA = 0x%x\n", lba);
	kout.printf("LBA>>24 & 0x0f = %d\n", (lba >> 24)&0x0f);
	kout.printf("(u8)lba = %d\n", (u8)lba);
	kout.printf("(u8)(lba >> 8) = %d\n", (u8)(lba >> 8));
	kout.printf("(u8)(lba >> 16) = %d\n", (u8)(lba >> 16));
	//Port::byte_out(io + ATA_REG_HDDEVSEL, cmd | ((lba >> 24)&0x0f));
	Port::byte_out(io + ATA_REG_HDDEVSEL, (cmd | (u8)((lba >> 24 & 0x0F))));
	//kout.printf("issued 0x%x to 0x%x\n", (cmd | (lba >> 24)&0x0f), io + ATA_REG_HDDEVSEL);
	//for(int k = 0; k < 10000; k++) ;
	Port::byte_out(io + 1, 0x00);
	//kout.printf("issued 0x%x to 0x%x\n", 0x00, io + 1);
	//for(int k = 0; k < 10000; k++) ;
	Port::byte_out(io + ATA_REG_SECCOUNT0, 1);
	//kout.printf("issued 0x%x to 0x%x\n", (u8) numsects, io + ATA_REG_SECCOUNT0);
	//for(int k = 0; k < 10000; k++) ;
	Port::byte_out(io + ATA_REG_LBA0, (u8)((lba)));
	//kout.printf("issued 0x%x to 0x%x\n", (u8)((lba)), io + ATA_REG_LBA0);
	//for(int k = 0; k < 10000; k++) ;
	Port::byte_out(io + ATA_REG_LBA1, (u8)((lba) >> 8));
	//kout.printf("issued 0x%x to 0x%x\n", (u8)((lba) >> 8), io + ATA_REG_LBA1);
	//for(int k = 0; k < 10000; k++) ;
	Port::byte_out(io + ATA_REG_LBA2, (u8)((lba) >> 16));
	//kout.printf("issued 0x%x to 0x%x\n", (u8)((lba) >> 16), io + ATA_REG_LBA2);
	//for(int k = 0; k < 10000; k++) ;
	Port::byte_out(io + ATA_REG_COMMAND, ATA_CMD_READ_PIO);
	//kout.printf("issued 0x%x to 0x%x\n", ATA_CMD_READ_PIO, io + ATA_REG_COMMAND);

	ide_poll(io);

	set_task(0);
	for(int i = 0; i < 256; i++)
	{
		u16 data = inportw(io + ATA_REG_DATA);
		*(u16 *)(buf + i * 2) = data;
	}
	ide_400ns_delay(io);
	set_task(1);
	return 1;
}

void ata_read(u8 *buf, u32 lba, u32 numsects, device_t *dev)
{
	for(int i = 0; i < numsects; i++)
	{
		ata_read_one(buf, lba + i, dev);
		buf += 512;
	}
}

void ata_probe()
{
	// First check the primary bus,
	// and inside the master drive.
	 
	
	if(ide_identify(ATA_PRIMARY, ATA_MASTER))
	{
		ata_pm = 1;
		device_t *dev = (device_t *)malloc(sizeof(device_t));
		ide_private_data *priv = (ide_private_data *)malloc(sizeof(ide_private_data));
		// Now, process the IDENTIFY data 
		// Model goes from W#27 to W#46 
		char *str = (char *)malloc(40);
		for(int i = 0; i < 40; i += 2)
		{
			str[i] = ide_buf[ATA_IDENT_MODEL + i + 1];
			str[i + 1] = ide_buf[ATA_IDENT_MODEL + i];
		}
		dev->name = str;
		dev->unique_id = 32;
		dev->dev_type = DEVICE_BLOCK;
		priv->drive = (ATA_PRIMARY << 1) | ATA_MASTER;
		dev->priv = priv;
		dev->read = ata_read;
		device_add(dev);
		kout.printf("Device: %s\n", dev->name);
	}
	ide_identify(ATA_PRIMARY, ATA_SLAVE);
	//ide_identify(ATA_SECONDARY, ATA_MASTER);
	//ide_check_device(ATA_SECONDARY, ATA_SLAVE);
}

void ata_init()
{
	kout.printf("Checking for ATA drives\n");
	ide_buf = (u16 *)malloc(512);
	set_int(ATA_PRIMARY_IRQ, ide_primary_irq);
	set_int(ATA_SECONDARY_IRQ, ide_secondary_irq);
	ata_probe();
	_kill();
}*/