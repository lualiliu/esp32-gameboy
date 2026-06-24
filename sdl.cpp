#include "sdl.h"

#include <Arduino_GFX_Library.h>

#include "SPI.h"

#define _cs 5     // 3 goes to TFT CS
#define _dc 21    // 4 goes to TFT DC
#define _mosi 23  // 5 goes to TFT MOSI
#define _sclk 18  // 6 goes to TFT SCK/CLK
#define _rst 25   // ESP RST to TFT RESET
#define _miso 19  // Not connected
#define _led 22
//       3.3V     // Goes to TFT LED
//       5v       // Goes to TFT Vcc
//       Gnd      // Goes to TFT Gnd
#define _left GPIO_NUM_4
#define _right GPIO_NUM_26
#define _up GPIO_NUM_17
#define _down GPIO_NUM_27
#define _select GPIO_NUM_15
#define _start GPIO_NUM_35
#define _a GPIO_NUM_33
#define _b GPIO_NUM_32

Arduino_DataBus *bus =
    new Arduino_ESP32SPI(
        _dc,
        _cs,
        _sclk,
        _mosi,
        GFX_NOT_DEFINED);

Arduino_GFX *tft =
    new Arduino_ST7735(
        bus,
        _rst,
        1,
        false,
        128,
        160);

void backlighting(bool state) {
  if (!state) {
    digitalWrite(_led, LOW);
  } else {
    digitalWrite(_led, HIGH);
  }
}

#define GAMEBOY_HEIGHT 144
#define GAMEBOY_WIDTH 160
#define DRAW_HEIGHT 144
#define DRAW_WIDTH 160
#define SCREEN_HEIGHT 128
#define SCREEN_WIDTH 160

#define SPI_FREQ 27000000

static uint8_t *frame_buffer;

static int button_start, button_select, button_a, button_b, button_down,
    button_up, button_left, button_right;

static volatile bool frame_ready = false;
TaskHandle_t draw_task_handle;

void draw_button(bool value, int x_pos, int y_pos,
                 const char *label = nullptr) {
  if (value) {
    tft->fillCircle(x_pos, y_pos, 7, 0xffff);
  } else {
    tft->fillCircle(x_pos, y_pos, 7, 0x0000);
  }
  if (label) {
    int len = strlen(label);
    constexpr int char_len = 8;
    int width = char_len * len;
    for (int i = 0; i < len; ++i) {
      tft->drawChar(x_pos - width / 2 + char_len * i, y_pos + 20, label[i],
                    0xffff, RED);
    }
  }
}

void draw_task(void *parameter) {
  uint16_t color_palette[] = {
    0xFFFF, // white
    0xC618, // light gray
    0x7BEF, // dark gray
    0x0000  // black
  };

  int h_offset = (SCREEN_WIDTH - DRAW_WIDTH) / 2;
  int v_offset = (SCREEN_HEIGHT - DRAW_HEIGHT) / 2;
  while (true) {
    while (!frame_ready) {
      delay(1);
    }
    frame_ready = false;
    tft->drawIndexedBitmap(h_offset, v_offset, frame_buffer, color_palette,
                           DRAW_WIDTH, DRAW_HEIGHT);
    //   draw_button(button_up, 30, SCREEN_HEIGHT / 2 - 15);
    //   draw_button(button_left, 15, SCREEN_HEIGHT / 2);
    //   draw_button(button_right, 45, SCREEN_HEIGHT / 2);
    //   draw_button(button_down, 30, SCREEN_HEIGHT / 2 + 15);

    //   draw_button(button_select, 30, SCREEN_HEIGHT / 2 + 70, "select");
    //   draw_button(button_start, SCREEN_WIDTH - 30, SCREEN_HEIGHT / 2 + 70,
    //               "start");
    //   draw_button(button_a, SCREEN_WIDTH - 45, SCREEN_HEIGHT / 2, "a");
    //   draw_button(button_b, SCREEN_WIDTH - 15, SCREEN_HEIGHT / 2 - 15, "b");
    //
  }
}

void sdl_init(void) {
  frame_buffer = new uint8_t[DRAW_WIDTH * DRAW_HEIGHT];
  // GFX_EXTRA_PRE_INIT();
  tft->begin(SPI_FREQ);
  pinMode(_led, OUTPUT);
  backlighting(true);
  tft->fillScreen(RED);

  gpio_num_t gpios[] = {_left, _right, _down, _up, _start, _select, _a, _b};
  for (gpio_num_t pin : gpios) {
    gpio_pad_select_gpio(pin);
    gpio_set_direction(pin, GPIO_MODE_INPUT);
    // uncomment to use builtin pullup resistors
    //    gpio_set_pull_mode(pin, GPIO_PULLUP_ONLY);
  }
  xTaskCreatePinnedToCore(draw_task,  /* Function to implement the task */
                          "drawTask", /* Name of the task */
                          10000,      /* Stack size in words */
                          NULL,       /* Task input parameter */
                          0,          /* Priority of the task */
                          &draw_task_handle, /* Task handle. */
                          0); /* Core where the task should run */
}

int sdl_update(void) {
  button_up = !gpio_get_level(_up);
  button_left = !gpio_get_level(_left);
  button_down = !gpio_get_level(_down);
  button_right = !gpio_get_level(_right);

  button_start = !gpio_get_level(_start);
  button_select = !gpio_get_level(_select);

  button_a = !gpio_get_level(_a);
  button_b = !gpio_get_level(_b);
  sdl_frame();
  return 0;
}

unsigned int sdl_get_buttons(void) {
  unsigned int buttons =
      (button_start * 8) | (button_select * 4) | (button_b * 2) | button_a;
  return buttons;
}

unsigned int sdl_get_directions(void) {
  return (button_down * 8) | (button_up * 4) | (button_left * 2) | button_right;
}

uint8_t *sdl_get_framebuffer(void) { return frame_buffer; }

void sdl_frame(void) { frame_ready = true; }
