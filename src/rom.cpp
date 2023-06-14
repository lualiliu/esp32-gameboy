#include <stdio.h>
#include <string.h>
#include "rom.h"

const unsigned char *bytes;
unsigned int mapper;

static char *banks[] = {
	" 32KiB",
	" 64KiB",
	"128KiB",
	"256KiB",
	"512KiB",
	"  1MiB",
	"  2MiB",
	"  4MiB",
	"1.1MiB",
	"1.2MiB",
	"1.5MiB",
	"Unknown"
};

static char *rams[] = {
	"None",
	"  2KiB",
	"  8KiB",
	" 32KiB",
	"Unknown"
};

static char *regions[] = {
	"Japan",
	"Non-Japan",
	"Unknown"
};

static unsigned char header[] = {
	0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B,
	0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
	0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
	0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
	0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
	0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E
};

int rom_init(const unsigned char *rombytes) {
	char buf[17];
	int type, bank_index, ram, region, version, i, pass;
	unsigned char checksum = 0;

	if(memcmp(&rombytes[0x104], header, sizeof(header)) != 0)
		return 0;
	memcpy(buf, &rombytes[0x134], 16);

	buf[16] = '\0';
	type = rombytes[0x147];
	bank_index = rombytes[0x148];

	if(bank_index >= 0x52 && bank_index <= 0x54)
		bank_index -= 74;
	else if(bank_index > 7)
		bank_index = 11;
	ram = rombytes[0x149];

	if(ram > 3)
		ram = 4;
	region = rombytes[0x14A];

	if(region > 2)
		region = 2;
	version = rombytes[0x14C];

	for(i = 0x134; i <= 0x14C; i++)
		checksum = checksum - rombytes[i] - 1;
	pass = rombytes[0x14D] == checksum;

	if(!pass)
		return 0;
	bytes = rombytes;

	switch(type) {
		case 0x00:
		case 0x08:
		case 0x09:
			mapper = NROM;
			break;

		case 0x01:
		case 0x02:
		case 0x03:
			mapper = MBC1;
			break;

		case 0x05:
		case 0x06:
			mapper = MBC2;
			break;

		case 0x0B:
		case 0x0C:
			mapper = MMM01;
			break;

		case 0x0F:
		case 0x10:
		case 0x11:
		case 0x12:
		case 0x13:
			mapper = MBC3;
			break;

		case 0x15:
		case 0x16:
		case 0x17:
			mapper = MBC4;
			break;

		case 0x19:
		case 0x1A:
		case 0x1B:
		case 0x1C:
		case 0x1D:
		case 0x1E:
			mapper = MBC5;
			break;
	}

	return 1;
}

unsigned int rom_get_mapper() {
	return mapper;
}

int rom_load(const char *filename) {
  return 0;
}

const unsigned char *rom_getbytes() {
	return bytes;
}
