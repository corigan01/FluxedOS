#include "pic.h"

void pic_init() {
    VGA::PRINT_STR("PIC INIT ");

    port_byte_out(PIC_MASTER_CMD, 0x10 + 0x01);
	port_byte_out(PIC_SLAVE_CMD,  0x10 + 0x01);
	
	port_byte_out(PIC_MASTER_DATA, 0x20);
	
	port_byte_out(PIC_SLAVE_DATA, 0x28);
	
	port_byte_out(PIC_MASTER_DATA, 4);
	port_byte_out(PIC_SLAVE_DATA, 2);
	
	port_byte_out(PIC_MASTER_DATA, 0x01);
	port_byte_out(PIC_SLAVE_DATA, 0x01);

	
	port_byte_out(PIC_MASTER_DATA, 0);
	port_byte_out(PIC_SLAVE_DATA, 0);

	VGA::SET_COLOR(VGA::COLORS::GREEN, VGA::COLORS::BLACK);
	VGA::PRINT_STR("OK\n");
	VGA::SET_COLOR(VGA::COLORS::BLACK, VGA::COLORS::BLACK);


}

void pic_send(uint8 irq) {
    if(irq >= 8)
		port_byte_out(PIC_SLAVE_CMD, PIC_CMD_EOI);
	port_byte_out(PIC_MASTER_CMD, PIC_CMD_EOI);


}