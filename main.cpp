#include <stdio.h>
#include "timer.h"
#include "rom.h"
#include "mem.h"
#include "cpu.h"
#include "lcd.h"
#include "sdl.h"

int main(int argc, char *argv[])
{
	int r;
	const char usage[] = "Usage: %s <rom>\n";

	if(argc != 2) {
		fprintf(stderr, usage, argv[0]);
		return 0;
	}

	r = rom_load(argv[1]);
	if(!r)
		return 0;

	sdl_init();

	printf("ROM OK!\n");

	mem_init();
	printf("Mem OK!\n");

	cpu_init();
	printf("CPU OK!\n");

	while(1)
	{
		if(!cpu_cycle())
			break;

		if(!lcd_cycle())
			break;

		timer_cycle();
	}

	sdl_quit();

	return 0;
}
