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


/*
 *  Equivalent to 400 ns delay
 */
void io_wait(ata_dev_t * dev) {
    port_byte_in(dev->alt_status);
    port_byte_in(dev->alt_status);
    port_byte_in(dev->alt_status);
    port_byte_in(dev->alt_status);
}

/*
 * From osdev wiki
 * "For non-ATAPI drives, the only method a driver has of resetting a drive after a major error is to do a "software reset" on the bus.
 * Set bit 2 (SRST, value = 4) in the proper Control Register for the bus. This will reset both ATA devices on the bus."
 */
void software_reset(ata_dev_t * dev) {
    port_byte_out(dev->control, CONTROL_SOFTWARE_RESET);
    io_wait(dev);
    port_byte_out(dev->control, CONTROL_ZERO);
}

void ata_handler(register_t * reg) {
    port_byte_in(primary_master.status);
    port_byte_in(primary_master.BMR_STATUS);
    port_byte_out(primary_master.BMR_COMMAND, BMR_COMMAND_DMA_STOP);
    //irq_ack(14);
}


void ata_open(vfs_node_t * node, uint32_t flags) {
        return;
}

void ata_close(vfs_node_t * node) {
        return;
}
/*
 * ata read size bytes starting from offset, the offset can be viewed as nth byte of the total number of disk bytes
 * */
uint32_t ata_read(vfs_node_t * node, uint32_t offset, uint32_t size, char * buf) {
    /*
     * Consider, for the specified offset, from which block should we start reading, and what's the in-block offset ?
     * Which block is the end block, and what's the in-block offset ?
     * */
    uint32_t start = offset / SECTOR_SIZE;
    uint32_t start_offset = offset % SECTOR_SIZE;

    uint32_t end = (offset + size - 1) / SECTOR_SIZE;
    uint32_t end_offset = (offset + size - 1) % SECTOR_SIZE;

    char * buf_curr = buf;
    uint32_t counter = start;
    uint32_t read_size;
    uint32_t off, total = 0;

    // Start reading from sectors
    while(counter <= end) {
        off = 0;
        read_size = SECTOR_SIZE;

        char * ret = ata_read_sector((ata_dev_t*)node->device, counter);

        // When reading the start block / the end block, things are a bit different
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

/*
 * ata write
 * */
uint32_t ata_write(vfs_node_t * node, uint32_t offset, uint32_t size, char * buf) {
    uint32_t start = offset / SECTOR_SIZE;
    uint32_t start_offset = offset % SECTOR_SIZE;

    uint32_t end = (offset + size - 1) / SECTOR_SIZE;
    uint32_t end_offset = (offset + size - 1) % SECTOR_SIZE;

    char * buf_curr = buf;
    uint32_t counter = start;
    uint32_t write_size;
    uint32_t off, total = 0;

    while(counter <= end) {
        off = 0;
        write_size = SECTOR_SIZE;
        char * ret = ata_read_sector((ata_dev_t*)node->device, counter);
        if(counter == start) {
            off = start_offset;
            write_size = SECTOR_SIZE - off;
        }
        if(counter == end) {
             write_size = end_offset - off + 1;
        }
        memcpy(ret + off, buf_curr, write_size);
        ata_write_sector((ata_dev_t*)node->device, counter, ret);
        buf_curr = buf_curr + write_size;
        total = total + write_size;
        counter++;
    }
    return total;
}

void ata_write_sector(ata_dev_t * dev, uint32_t lba, char * buf) {
    // First, copy the buffer over to dev->mem_buffer(Pointed to by prdt[0].buffer_phys)
    memcpy(dev->mem_buffer, buf, SECTOR_SIZE);

    // Reset bus master register's command register
    port_byte_out(dev->BMR_COMMAND, 0);
    // Set prdt
    port_long_out(dev->BMR_prdt, (uint32_t)dev->prdt_phys);
    // Select drive
    port_byte_out(dev->drive, 0xe0 | dev->slave << 4 | (lba & 0x0f000000) >> 24);
    // Set sector counts and LBAs
    port_byte_out(dev->sector_count, 1);
    port_byte_out(dev->lba_lo, lba & 0x000000ff);
    port_byte_out(dev->lba_mid, (lba & 0x0000ff00) >> 8);
    port_byte_out(dev->lba_high, (lba & 0x00ff0000) >> 16);

    // Write the WRITE_DMA to the command register (0xCA)
    port_byte_out(dev->command, 0xCA);

    // Start DMA Writing
    port_byte_out(dev->BMR_COMMAND, 0x1);

    // Wait for dma write to complete
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

char * ata_read_sector(ata_dev_t * dev, uint32_t lba) {
    char * buf = (char*)malloc(SECTOR_SIZE);

    // Reset bus master register's command register
    port_byte_out(dev->BMR_COMMAND, 0);
    // Set prdt
    port_long_out(dev->BMR_prdt, (uint32_t)dev->prdt_phys);
    // Select drive
    port_byte_out(dev->drive, 0xe0 | dev->slave << 4 | (lba & 0x0f000000) >> 24);
    // Set sector counts and LBAs
    port_byte_out(dev->sector_count, 1);
    port_byte_out(dev->lba_lo, lba & 0x000000ff);
    port_byte_out(dev->lba_mid, (lba & 0x0000ff00) >> 8);
    port_byte_out(dev->lba_high, (lba & 0x00ff0000) >> 16);

    // Write the READ_DMA to the command register (0xC8)
    port_byte_out(dev->command, 0xC8);

    // Start DMA reading
    port_byte_out(dev->BMR_COMMAND, 0x8 | 0x1);

    // Wait for dma write to complete
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

vfs_node_t * create_ata_device(ata_dev_t * dev) {
    vfs_node_t * t = (vfs_node_t*)malloc(sizeof(vfs_node_t)* 1);
    strcpy_(t->name,"ata device ");
    // Get device char from its moungpoint, yeah, weird code, I know
    t->name[strlen(t->name)] = dev->mountpoint[strlen(dev->mountpoint) - 1];
    // Not very useful to store the device, just in case we need it later
    t->device = dev;
    t->flags = FS_BLOCKDEVICE;
    t->read = ata_read;
    t->write = ata_write;
    t->open = ata_open;
    t->close = ata_close;
    return t;
}
/*
 * Init device, setup dma
 */
void ata_device_init(ata_dev_t * dev, int primary) {

    // Setup DMA
    // Prdt must not cross 64kb boundary / contiguous in physical memory. So simply allocate a 4kb aligned page satisfy both conditions
    dev->prdt = (prdt_t*)malloc(sizeof(prdt_t) * 4096);
    memset(dev->prdt, 0, sizeof(prdt_t));
    dev->prdt_phys = (uint8*)dev->prdt;
    dev->mem_buffer = (uint8*)malloc(4096);
    memset(dev->mem_buffer, 0, 4096);

    dev->prdt[0].buffer_phys = (uint32_t)dev->mem_buffer;
    dev->prdt[0].transfer_size = SECTOR_SIZE;
    dev->prdt[0].mark_end = MARK_END;

    // Setup register address
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

    // Set device's mountpoint(like /dev/hda)
    memset(dev->mountpoint, 0, 32);
    strcpy_(dev->mountpoint, "/dev/hd");
    // Primary master(hda, 00), primary slave(hdb, 01), secondary master(hdc, 10), secondary slave(hdd, 11)
    dev->mountpoint[strlen(dev->mountpoint)] = 'a' + (((!primary) << 1) | dev->slave);
}
/*
 * This function follows the following article to detect an ata device
 * http://wiki.osdev.org/ATA_PIO_Mode#IDENTIFY_command
 */
void ata_device_detect(ata_dev_t * dev, int primary) {

    VGA::PRINT_STR("ATA DEVICE INIT : ");

    // Must init some register address before detection
    ata_device_init(dev, primary);

    software_reset(dev);
    io_wait(dev);
    // Select drive, send 0xA0 to master device, 0xB0 to slave device
    port_byte_out(dev->drive, (0xA + dev->slave) << 4);
    // Set sector counts and LBAs to 0
    port_byte_out(dev->sector_count, 0);
    port_byte_out(dev->lba_lo, 0);
    port_byte_out(dev->lba_mid, 0);
    port_byte_out(dev->lba_high, 0);
    // Send identify command to command port

    port_byte_out(dev->command, COMMAND_IDENTIFY);
    if(!port_byte_in(dev->status)) {
        VGA::PRINT_STR("ata_detect_device: device does not exist\n");
        return;
    }

    uint8_t lba_lo = port_byte_in(dev->lba_lo);
    uint8_t lba_hi = port_byte_in(dev->lba_high);
    if(lba_lo != 0 || lba_hi != 0) {
        VGA::PRINT_STR("ata_detect_device: not ata device\n");
        return;
    }
    // Polling
    uint8_t drq = 0, err = 0;
    // If either drq or err is set, stop the while loop
    while(!drq && !err) {
        drq = port_byte_in(dev->status) & STATUS_DRQ;
        err = port_byte_in(dev->status) & STATUS_ERR;
    }
    if(err) {
        VGA::PRINT_STR("ata_detect_device: err when polling\n");
        return;
    }

    // Read 256 words(don't care the return value)
    for(int i = 0; i < 256; i++) port_word_in(dev->data);

    uint32_t pci_command_reg = PCI::read(ata_device, PCI_COMMAND);
    if(!(pci_command_reg & (1 << 2))) {
        pci_command_reg |= (1 << 2);
        PCI::write(ata_device, PCI_COMMAND, pci_command_reg);
    }

    VGA::PRINT_STR(" ");
    G_OK;

    //VGA::PRINT_STR("VFS MOUNT");

    // vfs not done yet
    vfs_mount(dev->mountpoint, create_ata_device(dev));

    
}

void ata_init() {
    // First, find pci device
    ata_device = PCI::get_device(ATA_VENDOR_ID, ATA_DEVICE_ID, -1);

    // Second, install irq handler
    register_interrupt_handler(32 + 14, ata_handler);

    // Third, detect four ata devices
    ata_device_detect(&primary_master, 1);
    ata_device_detect(&primary_slave, 1);
    ata_device_detect(&secondary_master, 0);
    ata_device_detect(&secondary_slave, 0);

    VGA::PRINT_STR("ATA INIT ");

    G_OK;

}
