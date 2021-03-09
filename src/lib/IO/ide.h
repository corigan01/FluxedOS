#ifndef IDE_DRIVE__H__
#define IDE_DRIVE__H__


#include "../core/core.h"
#include "port.h"
#include "../VGA/VGA.h"


namespace ATA
{

    // Status
    #define    ATA_SR_BSY      0x80
    #define    ATA_SR_DRDY     0x40
    #define    ATA_SR_DF       0x20
    #define    ATA_SR_DSC      0x10
    #define    ATA_SR_DRQ      0x08
    #define    ATA_SR_CORR     0x04
    #define    ATA_SR_IDX      0x02
    #define    ATA_SR_ERR      0x01

    // ERRORS
    #define    ATA_ER_BBK      0x80
    #define    ATA_ER_UNC      0x40
    #define    ATA_ER_MC       0x20
    #define    ATA_ER_IDNF     0x10
    #define    ATA_ER_MCR      0x08
    #define    ATA_ER_ABRT     0x04
    #define    ATA_ER_TK0NF    0x02
    #define    ATA_ER_AMNF     0x01

    // Commands
    #define    ATA_CMD_READ_PIO           0x20
    #define    ATA_CMD_READ_PIO_EXT       0x24
    #define    ATA_CMD_READ_DMA           0xC8
    #define    ATA_CMD_READ_DMA_EXT       0x25
    #define    ATA_CMD_WRITE_PIO          0x30
    #define    ATA_CMD_WRITE_PIO_EXT      0x34
    #define    ATA_CMD_WRITE_DMA          0xCA
    #define    ATA_CMD_WRITE_DMA_EXT      0x35
    #define    ATA_CMD_CACHE_FLUSH        0xE7
    #define    ATA_CMD_CACHE_FLUSH_EXT    0xEA
    #define    ATA_CMD_PACKET             0xA0
    #define    ATA_CMD_IDENTIFY_PACKET    0xA1
    #define    ATA_CMD_IDENTIFY           0xEC

    // R/E
    #define    ATAPI_CMD_READ      0xA8
    #define    ATAPI_CMD_EJECT     0x1B

    // IDENT
    #define    ATA_IDENT_DEVICETYPE     0
    #define    ATA_IDENT_CYLINDERS      2
    #define    ATA_IDENT_HEADS          6
    #define    ATA_IDENT_SECTORS        12
    #define    ATA_IDENT_SERIAL         20
    #define    ATA_IDENT_MODEL          54
    #define    ATA_IDENT_CAPABILITIES   98
    #define    ATA_IDENT_FIELDVALID     106
    #define    ATA_IDENT_MAX_LBA        120
    #define    ATA_IDENT_COMMANDSETS    164
    #define    ATA_IDENT_MAX_LBA_EXT    200

    // Configure
    #define    ATA_MASTER      0x00
    #define    ATA_SLAVE       0x01


    // API TYPE
    #define    IDE_ATA         0x00
    #define    IDE_ATAPI       0x01

    // ATA-ATAPI Task-File:
    #define    ATA_REG_DATA        0x00
    #define    ATA_REG_ERROR       0x01
    #define    ATA_REG_FEATURES    0x01
    #define    ATA_REG_SECCOUNT0   0x02
    #define    ATA_REG_LBA0        0x03
    #define    ATA_REG_LBA1        0x04
    #define    ATA_REG_LBA2        0x05
    #define    ATA_REG_HDDEVSEL    0x06
    #define    ATA_REG_COMMAND     0x07
    #define    ATA_REG_STATUS      0x07
    #define    ATA_REG_SECCOUNT1   0x08
    #define    ATA_REG_LBA3        0x09
    #define    ATA_REG_LBA4        0x0A
    #define    ATA_REG_LBA5        0x0B
    #define    ATA_REG_CONTROL     0x0C
    #define    ATA_REG_ALTSTATUS   0x0C
    #define    ATA_REG_DEVADDRESS  0x0D

    // Channels:
    #define    ATA_PRIMARY      0x00
    #define    ATA_SECONDARY    0x01

    // Directions:
    #define    ATA_READ         0x00
    #define    ATA_WRITE        0x01


    // Channel
    struct channel {
        uint16 base;    // I/O Base
        uint16 ctrl;    // Control base
        uint16 bmide;   // Bus Master IDE
        uint16 nIEN;    // nIEN (No Interupt)
    };


    // Device
    struct ide_device {
        uint8   Reserved;       // 0 (Empty) or 1 (This Drive really exists).
        uint8   Channel;        // 0 (Primary Channel) or 1 (Secondary Channel).
        uint8   Drive;          // 0 (Master Drive) or 1 (Slave Drive).
        uint16  Type;           // 0: ATA, 1:ATAPI.
        uint16  Sign;           // Drive Signature
        uint16  Capabilities;   // Features.
        uint32  Commandsets;    // Command Sets Supported.
        uint32  Size;           // Size in Sectors.
        uint8   Model[41];      // Model in string.
    };

    // R/W
    uint8 ide_read  (uint8 channel, uint8 reg);
    void  ide_write (uint8 channel, uint8 reg, uint8 data);

    // buffer / polling
    void ide_read_buffer(uint8 channel, uint8 reg, uint32 *buffer, uint32 quads);
    uint8 ide_polling(uint8 channel, uint16 advanced_check);

    // Err 
    uint8 ide_ERR(uint32 drive, uint8 err);

    // ide init
    void ide_init(uint32 BAR0, uint32 BAR1, uint32 BAR2, uint32 BAR3, uint32 BAR4);
    uint8 ide_ata_access(uint8 direction, uint8 drive, uint32 lba, uint8 numsects, uint16 selector, uint32 edi);
    






} // namespace ATA







#endif // !IDE_DRIVE__H__
