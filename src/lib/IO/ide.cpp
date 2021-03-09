#include "ide.h"

// Globals
uint8           ide_buf[2048] = {0};
static uint8    ide_irq_invoked = 0;
static uint8    atapi_packet[12] = {0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

ATA::channel channels[2];
ATA::ide_device ide_devices[4];

// R/W
uint8 ATA::ide_read  (uint8 channel, uint8 reg) {
    unsigned char result;
    if   (reg > 0x07 && reg < 0x0C) ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
    if   (reg < 0x08) result = port_byte_in(channels[channel].base  + reg - 0x00);
    else if   (reg < 0x0C) result = port_byte_in(channels[channel].base  + reg - 0x06);
    else if   (reg < 0x0E) result = port_byte_in(channels[channel].ctrl  + reg - 0x0A);
    else if   (reg < 0x16) result = port_byte_in(channels[channel].bmide + reg - 0x0E);
    if   (reg > 0x07 && reg < 0x0C) ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
    return result;
}

void  ATA::ide_write (uint8 channel, uint8 reg, uint8 data) {
    if   (reg > 0x07 && reg < 0x0C) ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
    if   (reg < 0x08) port_byte_out(data, channels[channel].base  + reg - 0x00);
    else if   (reg < 0x0C) port_byte_out(data, channels[channel].base  + reg - 0x06);
    else if   (reg < 0x0E) port_byte_out(data, channels[channel].ctrl  + reg - 0x0A);
    else if   (reg < 0x16) port_byte_out(data, channels[channel].bmide + reg - 0x0E);
    if   (reg > 0x07 && reg < 0x0C) ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
}

// buffer / polling
void ATA::ide_read_buffer(uint8 channel, uint8 reg, uint32 *buffer, uint32 quads) {
    if   (reg > 0x07 && reg < 0x0C) ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
    asm(R"(
    pushw %es 
    movw %ds, %ax 
    movw %ax, %es)"); 

    if   (reg < 0x08) insl(channels[channel].base  + reg - 0x00, buffer, quads);
    else if   (reg < 0x0C) insl(channels[channel].base  + reg - 0x06, buffer, quads);
    else if   (reg < 0x0E) insl(channels[channel].ctrl  + reg - 0x0A, buffer, quads);
    else if   (reg < 0x16) insl(channels[channel].bmide + reg - 0x0E, buffer, quads);
    asm("popw %es");
    if   (reg > 0x07 && reg < 0x0C) ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
}

uint8 ATA::ide_polling(uint8 channel, uint16 advanced_check) {
    // TODO: Fix with PIC timer
    for(int i = 0; i < 4; i++)
        ide_read(channel, ATA_REG_ALTSTATUS); // Reading the Alternate Status port wastes 100ns; loop four times.
    
    //  Wait for BSY to be cleared:
    while (ide_read(channel, ATA_REG_STATUS) & ATA_SR_BSY)
        ; // Wait for BSY to be zero.
    
    if (advanced_check) {
        unsigned char state = ide_read(channel, ATA_REG_STATUS); // Read Status Register.
    
        // Check For Errors:
        if (state & ATA_SR_ERR)
            return 2; // Error.
    
        // Check If Device fault:
        if (state & ATA_SR_DF)
            return 1; // Device Fault.
    
        // Check DRQ:
        // BSY = 0; DF = 0; ERR = 0 so we should check for DRQ now.
        if ((state & ATA_SR_DRQ) == 0)
            return 3; // DRQ need to be set
    
    }
    
    return 0; // No Error.
}

// Error
uint8 ATA::ide_ERR(uint32 drive, uint8 err) {
    if (err == 0)
        return err;

    VGA::SET_COLOR(VGA::COLORS::RED, VGA::COLORS::BLACK);
    VGA::PRINT_STR("IDE:");
    if (err == 1) {VGA::PRINT_STR("- Device Fault\n     "); err = 19;}
    else if (err == 2) {
        unsigned char st = ide_read(ide_devices[drive].Channel, ATA_REG_ERROR);
        if (st & ATA_ER_AMNF)   {VGA::PRINT_STR("- No Address Mark Found\n     ");   err = 7;}
        if (st & ATA_ER_TK0NF)   {VGA::PRINT_STR("- No Media or Media Error\n     ");   err = 3;}
        if (st & ATA_ER_ABRT)   {VGA::PRINT_STR("- Command Aborted\n     ");      err = 20;}
        if (st & ATA_ER_MCR)   {VGA::PRINT_STR("- No Media or Media Error\n     ");   err = 3;}
        if (st & ATA_ER_IDNF)   {VGA::PRINT_STR("- ID mark not Found\n     ");      err = 21;}
        if (st & ATA_ER_MC)   {VGA::PRINT_STR("- No Media or Media Error\n     ");   err = 3;}
        if (st & ATA_ER_UNC)   {VGA::PRINT_STR("- Uncorrectable Data Error\n     ");   err = 22;}
        if (st & ATA_ER_BBK)   {VGA::PRINT_STR("- Bad Sectors\n     ");       err = 13;}
    } 
    else  if (err == 3)           {VGA::PRINT_STR("- Reads Nothing\n     "); err = 23;}
    else  if (err == 4)  {VGA::PRINT_STR("- Write Protected\n     "); err = 8;}
    VGA::kprintf("- [%s %s] %s\n",
      (const char *[]){"Primary", "Secondary"}[ide_devices[drive].Channel], // Use the channel as an index into the array
      (const char *[]){"Master", "Slave"}[ide_devices[drive].Drive], // Same as above, using the drive
      ide_devices[drive].Model);
 
    VGA::SET_COLOR(VGA::COLORS::WHITE, VGA::COLORS::BLACK);
    return err;
}

// ide init
void ATA::ide_init(uint32 BAR0, uint32 BAR1, uint32 BAR2, uint32 BAR3, uint32 BAR4) {
    int j, k, count = 0;

    VGA::PRINT_STR("Detecting I/O ports\n");
    // 1- Detect I/O Ports which interface IDE Controller:
    channels[ATA_PRIMARY  ].base  = (BAR0 & 0xFFFFFFFC) + 0x1F0 * (!BAR0);
    channels[ATA_PRIMARY  ].ctrl  = (BAR1 & 0xFFFFFFFC) + 0x3F6 * (!BAR1);
    channels[ATA_SECONDARY].base  = (BAR2 & 0xFFFFFFFC) + 0x170 * (!BAR2);
    channels[ATA_SECONDARY].ctrl  = (BAR3 & 0xFFFFFFFC) + 0x376 * (!BAR3);
    channels[ATA_PRIMARY  ].bmide = (BAR4 & 0xFFFFFFFC) + 0; // Bus Master IDE
    channels[ATA_SECONDARY].bmide = (BAR4 & 0xFFFFFFFC) + 8; // Bus Master IDE

    // 2- Disable IRQs:
    VGA::PRINT_STR("Disable IRQs\n");
    ide_write(ATA_PRIMARY  , ATA_REG_CONTROL, 2);
    ide_write(ATA_SECONDARY, ATA_REG_CONTROL, 2);


    VGA::PRINT_STR("Detect ATA\n");
    // 3- Detect ATA-ATAPI Devices:
    for (int i = 0; i < 2; i++)
        for (j = 0; j < 2; j++) {

            unsigned char err = 0, type = IDE_ATA, status;
            ide_devices[count].Reserved = 0; // Assuming that no drive here.

            // (I) Select Drive:
            VGA::PRINT_STR("Select Drive\n");
            ide_write(i, ATA_REG_HDDEVSEL, 0xA0 | (j << 4)); // Select Drive.
            

            VGA::PRINT_STR("Send ATA\n");
            ide_write(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
            
                    

            VGA::PRINT_STR("Polling ATA\n");
            // (III) Polling:
            if (ide_read(i, ATA_REG_STATUS) == 0) continue; // If Status = 0, No Device.


            VGA::PRINT_STR("ide_read()\n");
            //while (!(port_byte_in(0x1F7) & 0x08)) {}
            /*while(1) {
            status = ide_read(i, ATA_REG_STATUS);
            if ((status == ATA_SR_ERR)) {err = 1; break;} // If Err, Device is not ATA.

            if (!(status == ATA_SR_BSY) && (status == ATA_SR_DRQ)) break; // Everything is right.

            }*/

            // (IV) Probe for ATAPI Devices:
            VGA::PRINT_STR("Probe ATAPI\n");
            if (err != 0) {
            unsigned char cl = ide_read(i, ATA_REG_LBA1);
            unsigned char ch = ide_read(i, ATA_REG_LBA2);

            if (cl == 0x14 && ch ==0xEB)
                type = IDE_ATAPI;
            else if (cl == 0x69 && ch == 0x96)
                type = IDE_ATAPI;
            else
                continue; // Unknown Type (may not be a device).

            ide_write(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY_PACKET);
            sleep(1);
            }

            uint32 * ide_buf;

            // (V) Read Identification Space of the Device:
            ide_read_buffer(i, ATA_REG_DATA,  ide_buf, 128);

            // (VI) Read Device Parameters:
            ide_devices[count].Reserved     = 1;
            ide_devices[count].Type         = type;
            ide_devices[count].Channel      = i;
            ide_devices[count].Drive        = j;
            ide_devices[count].Sign    = *((unsigned short *)(ide_buf + ATA_IDENT_DEVICETYPE));
            ide_devices[count].Capabilities = *((unsigned short *)(ide_buf + ATA_IDENT_CAPABILITIES));
            ide_devices[count].Commandsets  = *((unsigned int *)(ide_buf + ATA_IDENT_COMMANDSETS));

            // (VII) Get Size:
            if (ide_devices[count].Commandsets & (1 << 26))
            // Device uses 48-Bit Addressing:
            ide_devices[count].Size   = *((unsigned int *)(ide_buf + ATA_IDENT_MAX_LBA_EXT));
            else
            // Device uses CHS or 28-bit Addressing:
            ide_devices[count].Size   = *((unsigned int *)(ide_buf + ATA_IDENT_MAX_LBA));

            // (VIII) String indicates model of device (like Western Digital HDD and SONY DVD-RW...):
            for(k = 0; k < 40; k += 2) {
            ide_devices[count].Model[k] = ide_buf[ATA_IDENT_MODEL + k + 1];
            ide_devices[count].Model[k + 1] = ide_buf[ATA_IDENT_MODEL + k];}
            ide_devices[count].Model[40] = 0; // Terminate String.

            count++;
        }

 

    // 4- Print Summary:
    for (int i = 0; i < 4; i++)
        if (ide_devices[i].Reserved == 1) {
            VGA::kprintf(" Found %s Drive %dGB - %s\n",
            (const char *[]){"ATA", "ATAPI"}[ide_devices[i].Type],         /* Type */
            ide_devices[i].Size / 1024 / 1024 / 2,               /* Size */
            ide_devices[i].Model);
        }
}

uint8 ATA::ide_ata_access(uint8 direction, uint8 drive, uint32 lba, uint8 numsects, uint16 selector, uint32 edi) {

}

