#ifndef MEM_H
#define MEM_H

#include "rom.h"

void mem_init();
void mem_write_byte(unsigned short, unsigned char);
void mem_write_word(unsigned short, unsigned short);
void mem_bank_switch(unsigned int);

unsigned char mem_get_byte(unsigned short);
unsigned char mem_get_raw(unsigned short);
unsigned short mem_get_word(unsigned short);

#endif
