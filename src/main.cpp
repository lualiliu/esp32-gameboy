#include "timer.h"
#include "rom.h"
#include "mem.h"
#include "cpu.h"
#include "lcd.h"
#include "sdl.h"
#include "gbrom.h"

void setup() {
    rom_init(gb_rom);

    sdl_init();
    mem_init();
    cpu_init();
}

void loop() {
    cpu_cycle();
    lcd_cycle();
    timer_cycle();
}