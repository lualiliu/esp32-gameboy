#include <stdio.h>
#include "timer.h"
#include "rom.h"
#include "mem.h"
#include "cpu.h"
#include "lcd.h"
#include "sdl.h"
#include "gbrom.h"


void setup() {
  // put your setup code here, to run once:
  int r = rom_init(gb_rom);

  sdl_init();

  printf("ROM OK!\n");

  mem_init();
  printf("Mem OK!\n");

  cpu_init();
  printf("CPU OK!\n");
}

void loop() {
  // put your main code here, to run repeatedly:
    cpu_cycle();

    lcd_cycle();

    timer_cycle();
}
