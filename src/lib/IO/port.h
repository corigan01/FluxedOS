#ifndef KERNEL_PORT_H
#define KERNEL_PORT_H

#include "../core/core.h"

unsigned char port_byte_in (unsigned short port);
void port_byte_out (unsigned short port, unsigned char data);

uint16_t port_word_in (uint16_t port);
void port_word_out (unsigned short port, unsigned short data);

void port_long_out(uint32_t port, uint32_t value);
uint32_t port_long_in(uint32_t port);

void insl(unsigned reg, unsigned int *buffer, int quads);

#endif // !KERNEL_PORT_H
