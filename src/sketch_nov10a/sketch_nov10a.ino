

// void setup() {
//   Serial.begin(9600);
// }

// void loop() {
//   Serial.println(" Hello World ");
//   delay(2000);
// }

/*
 * This example shows how to read from a seesaw encoder module.
 * The available encoder API is:
 *      int32_t getEncoderPosition();
        int32_t getEncoderDelta();
        void enableEncoderInterrupt();
        void disableEncoderInterrupt();
        void setEncoderPosition(int32_t pos);
 */
#include "Adafruit_seesaw.h"
#include <seesaw_neopixel.h>
#include <stdio.h>

#define SS_SWITCH        24
#define SS_NEOPIX        6

#define SEESAW_ADDR      0x36
#define MSGBUF_SIZE      40

// The seesaw is the rotary encoder chip. In addition to the encoder position,
// it also has a button and an RGB LED!
// https://learn.adafruit.com/adafruit-i2c-qt-rotary-encoder/arduino
Adafruit_seesaw ss;
seesaw_NeoPixel sspixel = seesaw_NeoPixel(1, SS_NEOPIX, NEO_GRB + NEO_KHZ800);

int32_t encoder_position;
bool button_position = true;
char msgbuf[MSGBUF_SIZE];

void setupRotaryEncoder() {
  if (! ss.begin(SEESAW_ADDR) || ! sspixel.begin(SEESAW_ADDR)) {
    Serial.println("Couldn't find seesaw on default address");
    while(1) delay(10);
  }
  uint32_t version = ((ss.getVersion() >> 16) & 0xFFFF);
  if (version  != 4991){
    snprintf(msgbuf, MSGBUF_SIZE, "Wrong firmware loaded? Firmware: %zu", version);
    Serial.println(msgbuf);
    while(1) delay(10);
  }
  encoder_position = ss.getEncoderPosition();  // Initialize position

  sspixel.setBrightness(20);
  sspixel.show();
  ss.pinMode(SS_SWITCH, INPUT_PULLUP);

  Serial.println("Turning on interrupts");
  delay(10);
  ss.setGPIOInterrupts((uint32_t)1 << SS_SWITCH, 1);
  ss.enableEncoderInterrupt();
}

void handleRotaryEncoder() {
  bool new_button_position = ss.digitalRead(SS_SWITCH);
  if (new_button_position != button_position) {
    button_position = new_button_position;
    if (button_position) {
      Serial.println("Button released");
    } else {
      Serial.println("Button pushed");
    }
  }

  int32_t new_position = ss.getEncoderPosition();
  if (encoder_position != new_position) {
    snprintf(msgbuf, MSGBUF_SIZE, "Encoder moved: %d", new_position);
    Serial.println(msgbuf);
    encoder_position = new_position;
  }
}

void setup() {
  
  // Initialize the message buffer
  for (int i = 0; i<MSGBUF_SIZE; i++) {
    msgbuf[i] = "";
  }
  
  // Initialize serial communications, which are needed to print to the serial monitor
  // in the Arduino IDE
  Serial.begin(9600);
  while (!Serial) delay(10);

  setupRotaryEncoder();
}

void loop() {
  handleRotaryEncoder();

  // don't overwhelm serial port
  delay(10);
}