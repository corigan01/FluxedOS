#ifndef PIC___CPU__
#define PIC___CPU__

#include "../lib/core/core.h"
#include "../lib/IO/port.h"
#include "../lib/VGA/VGA.h"

#define PIC_MASTER_CMD 0x20
#define PIC_MASTER_DATA 0x21
#define PIC_SLAVE_CMD 0xA0
#define PIC_SLAVE_DATA 0xA1
#define PIC_CMD_EOI 0x20

void pic_init();
void pic_send(uint8 irq);

#endif // !PIC___CPU__
