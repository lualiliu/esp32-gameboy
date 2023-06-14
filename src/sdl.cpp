#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

#define _cs 22
#define _dc 21
#define _mosi 23
#define _sclk 19
#define _rst 18
#define _miso 25
#define _led 5

Adafruit_ILI9341 tft = Adafruit_ILI9341(_cs, _dc, _mosi, _sclk, _rst, _miso);

void backlighting(bool state) {
    if(!state)
        digitalWrite(_led, LOW);
    else digitalWrite(_led, HIGH);
}

#define GAMEBOY_HEIGHT 160
#define GAMEBOY_WIDTH 144

static int button_start, button_select, button_a, button_b, button_down, button_up, button_left, button_right;

byte pixels[GAMEBOY_HEIGHT * GAMEBOY_WIDTH / 4];
byte getColorIndexFromFrameBuffer(int x, int y) {
  	int offset = x + y * 160;
	return (pixels[offset >> 2] >> ((offset & 3) << 1)) & 3;
}

const int color[] = {0x000000, 0x555555, 0xAAAAAA, 0xFFFFFF};

void SDL_Flip(byte *screen){
	int i,j;

	for(i = 0;i<GAMEBOY_WIDTH;i++)
    for(j = 0;j<GAMEBOY_HEIGHT;j++)
        tft.drawPixel(j, i, color[getColorIndexFromFrameBuffer(j, i)]);
}

void sdl_init(void) {
  tft.begin();
  pinMode(_led, OUTPUT);
  backlighting(true);
  
  uint8_t x = tft.readcommand8(ILI9341_RDMODE);
  Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDMADCTL);
  Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDPIXFMT);
  Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDIMGFMT);
  Serial.print("Image Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDSELFDIAG);
  Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX); 
  tft.fillScreen(ILI9341_RED);

  gpio_pad_select_gpio(GPIO_NUM_14);
  gpio_set_direction(GPIO_NUM_14, GPIO_MODE_INPUT);

  gpio_pad_select_gpio(GPIO_NUM_27);
  gpio_set_direction(GPIO_NUM_27, GPIO_MODE_INPUT);
}

int sdl_update(void){
    button_start = !gpio_get_level(GPIO_NUM_14);
    button_right = !gpio_get_level(GPIO_NUM_27);

	return 0;
}
unsigned int sdl_get_buttons(void)
{
	return (button_start*8) | (button_select*4) | (button_b*2) | button_a;
}

unsigned int sdl_get_directions(void)
{
	return (button_down*8) | (button_up*4) | (button_left*2) | button_right;
}

byte* sdl_get_framebuffer(void)
{
	return pixels;
}

void sdl_frame(void) {
	SDL_Flip(pixels);
}
