#include <Arduino.h>

#include "lcd.h"
#include "cpu.h"
#include "interrupt.h"
#include "sdl.h"
#include "mem.h"

static int lcd_line;
static int lcd_ly_compare;

static int ly_int;
static int mode2_oam_int;
static int mode1_vblank_int;
static int mode0_hblank_int;
static int ly_int_flag;
static int lcd_mode;

static int lcd_enabled;
static int window_tilemap_select;
static int window_enabled;
static int tilemap_select;
static int bg_tiledata_select;
static int sprite_size;
static int sprites_enabled;
static int bg_enabled;
static int scroll_x, scroll_y;
static int window_x, window_y;

static byte bgpalette[] = {3, 2, 1, 0};
static byte sprpalette1[] = {0, 1, 2, 3};
static byte sprpalette2[] = {0, 1, 2, 3};

static unsigned long colours[4] = {0xFFFFFF, 0xC0C0C0, 0x808080, 0x000000};

struct sprite { int y, x, tile, flags; };

enum {
	PRIO  = 0x80,
	VFLIP = 0x40,
	HFLIP = 0x20,
	PNUM  = 0x10
};

unsigned char lcd_get_stat() {
	return (ly_int) << 6 | lcd_mode;
}

void lcd_write_bg_palette(unsigned char n) {
	bgpalette[0] = (n >> 0) & 3;
	bgpalette[1] = (n >> 2) & 3;
	bgpalette[2] = (n >> 4) & 3;
	bgpalette[3] = (n >> 6) & 3;
}

void lcd_write_spr_palette1(unsigned char n) {
	sprpalette1[0] = 0;
	sprpalette1[1] = (n >> 2) & 3;
	sprpalette1[2] = (n >> 4) & 3;
	sprpalette1[3] = (n >> 6) & 3;
}

void lcd_write_spr_palette2(unsigned char n) {
	sprpalette2[0] = 0;
	sprpalette2[1] = (n >> 2) & 3;
	sprpalette2[2] = (n >> 4) & 3;
	sprpalette2[3] = (n >> 6) & 3;
}

void lcd_write_scroll_x(unsigned char n) {
	scroll_x = n;
}

void lcd_write_scroll_y(unsigned char n) {
	scroll_y = n;
}

int lcd_get_line() {
	return lcd_line;
}

void lcd_write_stat(unsigned char c) {
	ly_int = !!(c & 0x40);
}

void lcd_write_control(unsigned char c) {
	bg_enabled            = !!(c & 0x01);
	sprites_enabled       = !!(c & 0x02);
	sprite_size           = !!(c & 0x04);
	tilemap_select        = !!(c & 0x08);
	bg_tiledata_select    = !!(c & 0x10);
	window_enabled        = !!(c & 0x20);
	window_tilemap_select = !!(c & 0x40);
	lcd_enabled           = !!(c & 0x80);
}

void lcd_set_ly_compare(unsigned char c) {
	lcd_ly_compare = c;
}

void lcd_set_window_y(unsigned char n) {
	window_y = n;
}

void lcd_set_window_x(unsigned char n) {
	window_x = n;
}

static void swap(struct sprite *a, struct sprite *b) {
	struct sprite c;

	 c = *a;
	*a = *b;
	*b =  c;
}

static void sort_sprites(struct sprite *s, int n) {
	int swapped, i;

	do {
		swapped = 0;

		for(i = 0; i < n - 1; i++) {
			if(s[i].x < s[i + 1].x) {
				swap(&s[i], &s[i + 1]);
				swapped = 1;
			}
		}
	} while(swapped);
}

void drawColorIndexToFrameBuffer(int x, int y, byte idx, byte *b) {
	int offset = x + y * 160;
	b[offset >> 2] |= (idx << ((offset & 3) << 1));
}

static void draw_bg_and_window(byte *b, int line) {
	int x;

	for(x = 0; x < 160; x++) {
		unsigned int map_select, map_offset, tile_num, tile_addr, xm, ym;
		unsigned char b1, b2, mask, colour;

		if(line >= window_y && window_enabled && line - window_y < 144) {
			xm = x;
			ym = line - window_y;

			map_select = window_tilemap_select;
		}
		else {
			if(!bg_enabled) {
        		drawColorIndexToFrameBuffer(x, line, 0, b);
				return;
			}

			xm = (x + scroll_x) % 256;
			ym = (line + scroll_y) % 256;

			map_select = tilemap_select;
		}

		map_offset = (ym / 8) * 32 + xm / 8;
		tile_num = mem_get_raw(0x9800 + map_select * 0x400 + map_offset);

		if(bg_tiledata_select)
			tile_addr = 0x8000 + tile_num*16;
		else tile_addr = 0x9000 + ((signed char) tile_num) * 16;

		b1 = mem_get_raw(tile_addr + (ym % 8) * 2);
		b2 = mem_get_raw(tile_addr + (ym % 8) * 2 + 1);

		mask = 128 >> (xm % 8);
		colour = (!!(b2 & mask) << 1) | !!(b1 & mask);

	  	drawColorIndexToFrameBuffer(x, line, bgpalette[colour], b);
	}
}

static void draw_sprites(byte *b, int line, int nsprites, struct sprite *s) {
	int i;

	for(i = 0; i < nsprites; i++) {
		unsigned int b1, b2, tile_addr, sprite_line, x;
		if(s[i].x < -7)
			continue;

		sprite_line = s[i].flags & VFLIP ? (sprite_size ? 15 : 7)-(line - s[i].y) : line - s[i].y;
		tile_addr = 0x8000 + (s[i].tile * 16) + sprite_line * 2;

		b1 = mem_get_raw(tile_addr);
		b2 = mem_get_raw(tile_addr + 1);

		for(x = 0; x < 8; x++) {
			unsigned char mask, colour;
			byte *pal;

			if((s[i].x + x) >= 160)
				continue;

			mask = s[i].flags & HFLIP ? 128 >> (7 - x) : 128 >> x;
			colour = ((!!(b2 & mask)) << 1) | !!(b1 & mask);

			if(colour == 0)
				continue;

			pal = (s[i].flags & PNUM) ? sprpalette2 : sprpalette1;
			if(s[i].flags & PRIO) {
				unsigned int temp = b[line * 640 + (x + s[i].x)];

				if(temp != colours[bgpalette[0]])
					continue;
			}

		  	drawColorIndexToFrameBuffer(x + s[i].x, line, pal[colour], b);
		}
	}
}

static void render_line(int line) {
	int i, c = 0;
	struct sprite s[10];
	byte *b = sdl_get_framebuffer();

	for(i = 0; i < 40; i++) {
		int y;

		y = mem_get_raw(0xFE00 + (i * 4)) - 16;
		if(line < y || line >= y + 8 + (sprite_size * 8))
			continue;

		s[c].y = y;
		s[c].x = mem_get_raw(0xFE00 + (i * 4) + 1) - 8;
		s[c].tile = mem_get_raw(0xFE00 + (i * 4) + 2);
		s[c].flags = mem_get_raw(0xFE00 + (i * 4) + 3);

		c++;
		if(c == 10)
			break;
	}

	if(c) sort_sprites(s, c);

	draw_bg_and_window(b, line);
	draw_sprites(b, line, c, s);
}

static void draw_stuff() {
	byte *b = sdl_get_framebuffer();
	int y, tx, ty;

	for(ty = 0; ty < 24; ty++) {
		for(tx = 0; tx < 16; tx++) {
			for(y = 0; y < 8; y++) {
				unsigned char b1, b2;
				int tileaddr = 0x8000 + ty*0x100 + tx*16 + y*2;

				b1 = mem_get_raw(tileaddr);
				b2 = mem_get_raw(tileaddr + 1);
			}
		}
	}
}

int lcd_cycle() {
	int cycles = cpu_get_cycles();
	int this_frame, subframe_cycles;
	static int prev_line;

	if(sdl_update())
		return 0;

	this_frame = cycles % (70224/4);
	lcd_line = this_frame / (456/4);

	if(this_frame < 204 / 4)
		lcd_mode = 2;
	else if(this_frame < 284 / 4)
		lcd_mode = 3;
	else if(this_frame < 456 / 4)
		lcd_mode = 0;

	if(lcd_line >= 144)
		lcd_mode = 1;
		
	if(lcd_line != prev_line && lcd_line < 144)
		render_line(lcd_line);

	if(ly_int && lcd_line == lcd_ly_compare)
		interrupt(INTR_LCDSTAT);

	if(prev_line == 143 && lcd_line == 144) {
		interrupt(INTR_VBLANK);
		sdl_frame();
	}

	prev_line = lcd_line;
	return 1;
}
