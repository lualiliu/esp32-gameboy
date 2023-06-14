#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ST7735.h"

#define TFT_CS -1
#define TFT_DC 5
#define TFT_RST 19
#define TFT_MOSI 23
#define TFT_SCLK 18

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

#define GAMEBOY_HEIGHT 160
#define GAMEBOY_WIDTH 144

static int button_start = 0, button_select = 0,
  button_a = 0, button_b = 0,
  button_down = 0, button_up = 0, button_left = 0, button_right = 0;

byte pixels[GAMEBOY_HEIGHT * GAMEBOY_WIDTH / 4];
byte getColorIndexFromFrameBuffer(int x, int y) {
  	int offset = x + y * 160;
	return (pixels[offset >> 2] >> ((offset & 3) << 1)) & 3;
}

const int color[] = {0x000000, 0x555555, 0xAAAAAA, 0xFFFFFF};

void SDL_Flip(byte *screen) {
	int i, j;

	for(i = 0; i < GAMEBOY_WIDTH; i++)
        for(j = 0; j < GAMEBOY_HEIGHT; j++)
            tft.drawPixel(j, i, color[getColorIndexFromFrameBuffer(j, i)]);
}

void sdl_init() {
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(3);
  tft.fillScreen(ST77XX_RED);

  gpio_pad_select_gpio(GPIO_NUM_15);
  gpio_set_direction(GPIO_NUM_15, GPIO_MODE_INPUT);

  gpio_pad_select_gpio(GPIO_NUM_27);
  gpio_set_direction(GPIO_NUM_27, GPIO_MODE_INPUT);
}

int sdl_update() {
  button_start = !gpio_get_level(GPIO_NUM_15);
  button_right = !gpio_get_level(GPIO_NUM_27);

	return 0;
}

unsigned int sdl_get_buttons() {
	return (button_start * 8) | (button_select * 4) | (button_b * 2) | button_a;
}

unsigned int sdl_get_directions() {
	return (button_down * 8) | (button_up * 4) | (button_left * 2) | button_right;
}

byte* sdl_get_framebuffer() {
	return pixels;
}

void sdl_frame() {
	SDL_Flip(pixels);
}
