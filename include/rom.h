#ifndef ROM_H
#define ROM_H

#ifdef __cplusplus
extern "C" {
#endif

enum {
	NROM,
	MBC1,
	MBC2,
	MMM01,
	MBC3,
	MBC4,
	MBC5,
};

int rom_load(const char *);
int rom_init(const unsigned char *);

const unsigned char *rom_getbytes();
unsigned int rom_get_mapper();

#ifdef __cplusplus
}
#endif

#endif
