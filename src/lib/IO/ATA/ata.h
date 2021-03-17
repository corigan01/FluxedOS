#ifndef IDE_DRIVE__H__
#define IDE_DRIVE__H__


#include "../../core/core.h"
#include "../port.h"
#include "../../VGA/VGA.h"
#include "../../vfs/vfs.h"


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

// ATA PCI info
#define ATA_VENDOR_ID 0x8086
#define ATA_DEVICE_ID 0x7010

// Control reg
#define CONTROL_STOP_INTERRUPT 0x2
#define CONTROL_SOFTWARE_RESET 0x4
#define CONTROL_HIGH_ORDER_BYTE 0x80
#define CONTROL_ZERO 0x00


// Command reg
#define COMMAND_IDENTIFY 0xEC
#define COMMAND_DMA_READ 0xC8
#define ATA_CMD_READ_PIO 0x20

// Status reg
#define STATUS_ERR 0x0
#define STATUS_DRQ 0x8
#define STATUS_SRV 0x10
#define STATUS_DF  0x20
#define STATUS_RDY 0x40
#define STATUS_BSY 0x80

// Bus Master Reg Command
#define BMR_COMMAND_DMA_START 0x1
#define BMR_COMMAND_DMA_STOP 0x0
#define BMR_COMMAND_READ 0x8
#define BMR_STATUS_INT 0x4
#define BMR_STATUS_ERR 0x2


// Prdt
#define SECTOR_SIZE 512
#define MARK_END 0x8000




typedef struct prdt {
	uint32_t buffer_phys;
	uint16_t transfer_size;
	uint16_t mark_end;
}__attribute__((packed)) prdt_t;

typedef struct ata_dev {
	// A list of register address
	uint16_t data;
	uint16_t error;
	uint16_t sector_count;

	union {
		uint16_t sector_num;
		uint16_t lba_lo ;
	};
	union {
		uint16_t cylinder_low;
		uint16_t lba_mid ;
	};
	union {
		uint16_t cylinder_high;
		uint16_t lba_high;
	};
	union {
		uint16_t drive;
		uint16_t head;
	};
	union {
		uint16_t command;
		uint16_t status;
	};
	union {
		uint16_t control;
		uint16_t alt_status;
	};

	int slave;
	uint32_t bar4;
	uint32_t BMR_COMMAND;
	uint32_t BMR_prdt;
	uint32_t BMR_STATUS;


	prdt_t * prdt;
	uint8_t * prdt_phys;

	uint8_t * mem_buffer;
	uint8_t * mem_buffer_phys;

	char mountpoint[32];
}__attribute__((packed)) ata_dev_t;

namespace ATA
{
	void WaitIO(ata_dev_t * dev);
	void SoftReset(ata_dev_t * dev);
	void ata_handler(register_t * reg);
	
	void ata_open(vfs_node_t * node, uint32_t flags);
	void ata_close(vfs_node_t * node);

	uint32_t ata_read(vfs_node_t * node, uint32_t offset, uint32_t size, char * buf);
	uint32_t ata_write(vfs_node_t * node, uint32_t offset, uint32_t size, char * buf);
	void ata_write_sector(ata_dev_t * dev, uint32_t lba, char * buf);
	char * ata_read_sector(ata_dev_t * dev, uint32_t lba);

	vfs_node_t * create_ata_device(ata_dev_t * dev);
	void ata_device_init(ata_dev_t * dev, int primary);
	void ata_device_detect(ata_dev_t * dev, int primary);

	void ata_init();

} // namespace ATA






#endif // !IDE_DRIVE__H__
