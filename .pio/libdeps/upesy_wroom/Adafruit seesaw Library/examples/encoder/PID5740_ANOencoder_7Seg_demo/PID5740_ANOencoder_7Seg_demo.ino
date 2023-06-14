/*
 * This is a demo for a QT Py RP2040 connected to an ANO encoder breakout and a 7-segment breakout
 * using the onboard Stemma QT Port
 * https://www.adafruit.com/product/878
 * https://www.adafruit.com/product/5740
 * 
 */
#include "Adafruit_seesaw.h"
#include "Adafruit_LEDBackpack.h"


#define SS_SWITCH_SELECT 1
#define SS_SWITCH_UP     2
#define SS_SWITCH_LEFT   3
#define SS_SWITCH_DOWN   4
#define SS_SWITCH_RIGHT  5

#define SEESAW_ADDR      0x49

Adafruit_7segment seven = Adafruit_7segment();
Adafruit_seesaw ss = Adafruit_seesaw(&Wire1);
int32_t encoder_position;

void setup() {
  Serial.begin(115200);
  //while (!Serial) delay(10);

  seven.begin(0x70, &Wire1);

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
  seven.print(encoder_position, DEC);
  seven.writeDisplay();
  
  Serial.println("Turning on interrupts");
  ss.enableEncoderInterrupt();
  ss.setGPIOInterrupts((uint32_t)1 << SS_SWITCH_UP, 1);

}

void loop() {
  if (! ss.digitalRead(SS_SWITCH_UP)) {
    seven.println(" UP ");
    seven.writeDisplay();
    Serial.println("UP pressed!");
  }
  else if (! ss.digitalRead(SS_SWITCH_DOWN)) {
    seven.println("DOWN");
    seven.writeDisplay();
    Serial.println("DOWN pressed!");
  }
  else if (! ss.digitalRead(SS_SWITCH_SELECT)) {
    seven.println("SELE");
    seven.writeDisplay();
    Serial.println("SELECT pressed!");
  }
  else if (! ss.digitalRead(SS_SWITCH_LEFT)) {
    seven.println("LEFT");
    seven.writeDisplay();
    Serial.println("LEFT pressed!");
  }
  else if (! ss.digitalRead(SS_SWITCH_RIGHT)) {
    seven.println("RIGT");
    seven.writeDisplay();
    Serial.println("RIGHT pressed!");
  } else {
    seven.print(encoder_position, DEC);
    seven.writeDisplay();
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
