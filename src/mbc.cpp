#include "mbc.h"
#include "mem.h"
#include "rom.h"

enum {
	NO_FILTER_WRITE,
	FILTER_WRITE
};

static unsigned int bank_upper_bits;
static unsigned int ram_select;

unsigned int MBC3_write_byte(unsigned short d, unsigned char i) {
	int bank;

	if(d < 0x2000)
		return FILTER_WRITE;

	if(d < 0x4000) {
		bank = i & 0x7F;

		if(bank == 0)
			bank++;
		mem_bank_switch(bank);

		return FILTER_WRITE;
	}

	if(d < 0x8000)
		return FILTER_WRITE;

	return NO_FILTER_WRITE;
}

unsigned int MBC1_write_byte(unsigned short d, unsigned char i) {
	int bank;

	if(d < 0x2000)
		return FILTER_WRITE;

	if(d >= 0x2000 && d < 0x4000) {
		bank = i & 0x1F;
		if(!ram_select)
			bank |= bank_upper_bits;

		if(bank == 0 || bank == 0x20 || bank == 0x40 || bank == 0x60)
			bank++;

		mem_bank_switch(bank);
		return FILTER_WRITE;
	}

	if(d >= 0x4000 && d < 0x6000) {
		bank_upper_bits = (i & 0x3) << 5;
		return FILTER_WRITE;
	}

	if(d >= 0x6000 && d <= 0x7FFF) {
		ram_select = i & 1;
		return FILTER_WRITE;
	}

	return NO_FILTER_WRITE;
}
