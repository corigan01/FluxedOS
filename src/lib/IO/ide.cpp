#include "ide.h"
#include "../mem/mem.h"
#include "../PCI/PCI.h"
#include "../Gtree/Gtree.h"
#include "../vfs/vfs.h"
#include "../String/String.h"
#include "../../cpu/cpu.h"


PICdev_T ata_device;

ata_dev_t primary_master = {.slave = 0};
ata_dev_t primary_slave = {.slave = 1};
ata_dev_t secondary_master = {.slave = 0};
ata_dev_t secondary_slave = {.slave = 1};



void ATA::WaitIO(ata_dev_t * dev) {
    port_byte_in(dev->alt_status);
    port_byte_in(dev->alt_status);
    port_byte_in(dev->alt_status);
    port_byte_in(dev->alt_status);
}


void ATA::SoftReset(ata_dev_t * dev) {
    port_byte_out(dev->control, CONTROL_SOFTWARE_RESET);
    ATA::WaitIO(dev);
    port_byte_out(dev->control, CONTROL_ZERO);
}

void ATA::ata_handler(register_t * reg) {
    port_byte_in(primary_master.status);
    port_byte_in(primary_master.BMR_STATUS);
    port_byte_out(primary_master.BMR_COMMAND, BMR_COMMAND_DMA_STOP);
}


void ATA::ata_open(vfs_node_t * node, uint32 flags) {
        return;
}

void ATA::ata_close(vfs_node_t * node) {
        return;
}

uint32 ATA::ata_read(vfs_node_t * node, uint32 offset, uint32 size, char * buf) {
    
    uint32 start = offset / SECTOR_SIZE;
    uint32 start_offset = offset % SECTOR_SIZE;

    uint32 end = (offset + size - 1) / SECTOR_SIZE;
    uint32 end_offset = (offset + size - 1) % SECTOR_SIZE;

    char * buf_curr = buf;
    uint32 counter = start;
    uint32 read_size;
    uint32 off, total = 0;

    // read from sectors
    while(counter <= end) {
        off = 0;
        read_size = SECTOR_SIZE;

        char * ret = ATA::ata_read_sector((ata_dev_t*)node->device, counter);

        if(counter == start) {
            off = start_offset;
            read_size = SECTOR_SIZE - off;
        }
        if(counter == end)
            read_size = end_offset - off + 1;

        memcpy(buf_curr, ret + off, read_size);
        buf_curr = buf_curr + read_size;
        total = total + read_size;
        counter++;
    }
    return total;
}

uint32 ATA::ata_write(vfs_node_t * node, uint32 offset, uint32 size, char * buf) {
    uint32 start = offset / SECTOR_SIZE;
    uint32 start_offset = offset % SECTOR_SIZE;

    uint32 end = (offset + size - 1) / SECTOR_SIZE;
    uint32 end_offset = (offset + size - 1) % SECTOR_SIZE;

    char * buf_curr = buf;
    uint32 counter = start;
    uint32 write_size;
    uint32 off, total = 0;

    while(counter <= end) {
        off = 0;
        write_size = SECTOR_SIZE;
        char * ret = ATA::ata_read_sector((ata_dev_t*)node->device, counter);
        if(counter == start) {
            off = start_offset;
            write_size = SECTOR_SIZE - off;
        }
        if(counter == end) {
             write_size = end_offset - off + 1;
        }
        memcpy(ret + off, buf_curr, write_size);
        ATA::ata_write_sector((ata_dev_t*)node->device, counter, ret);
        buf_curr = buf_curr + write_size;
        total = total + write_size;
        counter++;
    }
    return total;
}

void ATA::ata_write_sector(ata_dev_t * dev, uint32 lba, char * buf) {
    memcpy(dev->mem_buffer, buf, SECTOR_SIZE);

    port_byte_out(dev->BMR_COMMAND, 0);
    port_long_out(dev->BMR_prdt, (uint32)dev->prdt_phys);
    port_byte_out(dev->drive, 0xe0 | dev->slave << 4 | (lba & 0x0f000000) >> 24);
    port_byte_out(dev->sector_count, 1);
    port_byte_out(dev->lba_lo, lba & 0x000000ff);
    port_byte_out(dev->lba_mid, (lba & 0x0000ff00) >> 8);
    port_byte_out(dev->lba_high, (lba & 0x00ff0000) >> 16);

    
    port_byte_out(dev->command, 0xCA);
    port_byte_out(dev->BMR_COMMAND, 0x1);


    while (1) {
        int status = port_byte_in(dev->BMR_STATUS);
        int dstatus = port_byte_in(dev->status);
        if (!(status & 0x04)) {
            continue;
        }
        if (!(dstatus & 0x80)) {
            break;
        }
    }
}

char * ATA::ata_read_sector(ata_dev_t * dev, uint32 lba) {
    char * buf = (char*)malloc(SECTOR_SIZE);

    port_byte_out(dev->BMR_COMMAND, 0);
    port_long_out(dev->BMR_prdt, (uint32)dev->prdt_phys);
    port_byte_out(dev->drive, 0xe0 | dev->slave << 4 | (lba & 0x0f000000) >> 24);
    port_byte_out(dev->sector_count, 1);
    port_byte_out(dev->lba_lo, lba & 0x000000ff);
    port_byte_out(dev->lba_mid, (lba & 0x0000ff00) >> 8);
    port_byte_out(dev->lba_high, (lba & 0x00ff0000) >> 16);

    port_byte_out(dev->command, 0xC8);

    // Start DMA reading
    port_byte_out(dev->BMR_COMMAND, 0x8 | 0x1);


    while (1) {
        int status = port_byte_in(dev->BMR_STATUS);
        int dstatus = port_byte_in(dev->status);
        if (!(status & 0x04)) {
            continue;
        }
        if (!(dstatus & 0x80)) {
            break;
        }
    }

    memcpy(buf, dev->mem_buffer, SECTOR_SIZE);
    return buf;

}

vfs_node_t * ATA::create_ata_device(ata_dev_t * dev) {
    vfs_node_t * t = (vfs_node_t*)malloc(sizeof(vfs_node_t)* 1);
    strcpy_(t->name,"ata device ");

    t->name[strlen(t->name)] = dev->mountpoint[strlen(dev->mountpoint) - 1];
    t->device = dev;
    t->flags = FS_BLOCKDEVICE;
    t->read = ATA::ata_read;
    t->write = ATA::ata_write;
    t->open = ATA::ata_open;
    t->close = ATA::ata_close;
    return t;
}

void ATA::ata_device_init(ata_dev_t * dev, int primary) {

    dev->prdt = (prdt_t*)malloc(sizeof(prdt_t) * 4096);
    memset(dev->prdt, 0, sizeof(prdt_t));
    dev->prdt_phys = (uint8*)dev->prdt;
    dev->mem_buffer = (uint8*)malloc(4096);
    memset(dev->mem_buffer, 0, 4096);

    dev->prdt[0].buffer_phys = (uint32)dev->mem_buffer;
    dev->prdt[0].transfer_size = SECTOR_SIZE;
    dev->prdt[0].mark_end = MARK_END;


    uint16_t base_addr = primary ? (0x1F0) : (0x170);
    uint16_t alt_status = primary ? (0x3F6) : (0x376);

    dev->data = base_addr;
    dev->error = base_addr + 1;
    dev->sector_count = base_addr + 2;
    dev->lba_lo = base_addr + 3;
    dev->lba_mid = base_addr + 4;
    dev->lba_high = base_addr + 5;
    dev->drive = base_addr + 6;
    dev->command = base_addr + 7;
    dev->alt_status = alt_status;


    dev->bar4 = PCI::read(ata_device, PCI_BAR4);
    if(dev->bar4 & 0x1) {
        dev->bar4 = dev->bar4 & 0xfffffffc;
    }
    dev->BMR_COMMAND = dev->bar4;
    dev->BMR_STATUS = dev->bar4 + 2;
    dev->BMR_prdt = dev->bar4 + 4;


    memset(dev->mountpoint, 0, 32);
    strcpy_(dev->mountpoint, "/dev/hd");
    dev->mountpoint[strlen(dev->mountpoint)] = 'a' + (((!primary) << 1) | dev->slave);
}

void ATA::ata_device_detect(ata_dev_t * dev, int primary) {


    ATA::ata_device_init(dev, primary);

    ATA::SoftReset(dev);
    ATA::WaitIO(dev);

    port_byte_out(dev->drive, (0xA + dev->slave) << 4);
    port_byte_out(dev->sector_count, 0);
    port_byte_out(dev->lba_lo, 0);
    port_byte_out(dev->lba_mid, 0);
    port_byte_out(dev->lba_high, 0);
    

    port_byte_out(dev->command, COMMAND_IDENTIFY);
    if(!port_byte_in(dev->status)) {
        VGA::PRINT_STR("Empty Port\n");
        return;
    }

    uint8_t lba_lo = port_byte_in(dev->lba_lo);
    uint8_t lba_hi = port_byte_in(dev->lba_high);
    if(lba_lo != 0 || lba_hi != 0) {
        VGA::PRINT_STR("Not ATA Interface\n");
        return;
    }

    uint8_t drq = 0, err = 0;
    while(!drq && !err) {
        drq = port_byte_in(dev->status) & STATUS_DRQ;
        err = port_byte_in(dev->status) & STATUS_ERR;
    }
    if(err) {
        VGA::PRINT_STR("ERROR during check\n");
        return;
    }


    for(int i = 0; i < 256; i++) port_word_in(dev->data);

    uint32 pci_command_reg = PCI::read(ata_device, PCI_COMMAND);
    if(!(pci_command_reg & (1 << 2))) {
        pci_command_reg |= (1 << 2);
        PCI::write(ata_device, PCI_COMMAND, pci_command_reg);
    }

    vfs_mount(dev->mountpoint, ATA::create_ata_device(dev));
    
}

void ATA::ata_init() {
    ata_device = PCI::get_device(ATA_VENDOR_ID, ATA_DEVICE_ID, -1);

    
    register_interrupt_handler(32 + 14, ATA::ata_handler);

    
    VGA::PRINT_STR("Primary   Master --> ");
    ATA::ata_device_detect(&primary_master, 1);
    VGA::PRINT_STR("Primary   Slave  --> ");
    ATA::ata_device_detect(&primary_slave, 1);
    VGA::PRINT_STR("Secondary Master --> ");
    ATA::ata_device_detect(&secondary_master, 0);
    VGA::PRINT_STR("Secondary Slave  --> ");
    ATA::ata_device_detect(&secondary_slave, 0);
    

    VGA::PRINT_STR("ATA INIT ");

    G_OK;

}
