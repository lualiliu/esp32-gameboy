/*
 * This example shows how to read from a seesaw encoder module.
 * The available encoder API is:
 *      int32_t getEncoderPosition();
        int32_t getEncoderDelta();
        void enableEncoderInterrupt();
        void disableEncoderInterrupt();
 */
#include "Adafruit_seesaw.h"

#define SS_SWITCH_SELECT 1
#define SS_SWITCH_UP     2
#define SS_SWITCH_LEFT   3
#define SS_SWITCH_DOWN   4
#define SS_SWITCH_RIGHT  5

#define SEESAW_ADDR      0x49

Adafruit_seesaw ss;
int32_t encoder_position;

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println("Looking for seesaw!");
  
  if (! ss.begin(SEESAW_ADDR)) {
    Serial.println("Couldn't find seesaw on default address");
    while(1) delay(10);
  }
  Serial.println("seesaw started");
  uint32_t version = ((ss.getVersion() >> 16) & 0xFFFF);
  if (version  != 5740){
    Serial.print("Wrong firmware loaded? ");
    Serial.println(version);
    while(1) delay(10);
  }
  Serial.println("Found Product 5740");

  ss.pinMode(SS_SWITCH_UP, INPUT_PULLUP);
  ss.pinMode(SS_SWITCH_DOWN, INPUT_PULLUP);
  ss.pinMode(SS_SWITCH_LEFT, INPUT_PULLUP);
  ss.pinMode(SS_SWITCH_RIGHT, INPUT_PULLUP);
  ss.pinMode(SS_SWITCH_SELECT, INPUT_PULLUP);

  // get starting position
  encoder_position = ss.getEncoderPosition();

  Serial.println("Turning on interrupts");
  ss.enableEncoderInterrupt();
  ss.setGPIOInterrupts((uint32_t)1 << SS_SWITCH_UP, 1);

}

void loop() {
  if (! ss.digitalRead(SS_SWITCH_UP)) {
    Serial.println("UP pressed!");
  }
  if (! ss.digitalRead(SS_SWITCH_DOWN)) {
    Serial.println("DOWN pressed!");
  }
  if (! ss.digitalRead(SS_SWITCH_SELECT)) {
    Serial.println("SELECT pressed!");
  }
  if (! ss.digitalRead(SS_SWITCH_LEFT)) {
    Serial.println("LEFT pressed!");
  }
  if (! ss.digitalRead(SS_SWITCH_RIGHT)) {
    Serial.println("RIGHT pressed!");
  }

  int32_t new_position = ss.getEncoderPosition();
  // did we move around?
  if (encoder_position != new_position) {
    Serial.println(new_position);         // display new position

    encoder_position = new_position;      // and save for next round
  }

  // don't overwhelm serial port
  delay(10);
}
