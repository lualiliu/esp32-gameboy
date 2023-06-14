#ifndef SDL_H
#define SDL_H

#include <Arduino.h>

int sdl_update();

void sdl_init();
void sdl_frame();
void sdl_quit();

byte* sdl_get_framebuffer();

unsigned int sdl_get_buttons();
unsigned int sdl_get_directions();

#endif
