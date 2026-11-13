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

#define SS_SWITCH        24
#define SS_NEOPIX        6

#define SEESAW_ADDR      0x36

// The seesaw is the rotary encoder chip. In addition to the encoder position,
// it also has a button and an RGB LED!
// https://learn.adafruit.com/adafruit-i2c-qt-rotary-encoder/arduino
Adafruit_seesaw ss;
seesaw_NeoPixel sspixel = seesaw_NeoPixel(1, SS_NEOPIX, NEO_GRB + NEO_KHZ800);

int32_t encoder_position;
bool button_position = true;

void setupRotaryEncoder() {
  if (! ss.begin(SEESAW_ADDR) || ! sspixel.begin(SEESAW_ADDR)) {
    Serial.println(F("Couldn't find seesaw on default address"));
    while(1) delay(10);
  }
  uint32_t version = ((ss.getVersion() >> 16) & 0xFFFF);
  if (version  != 4991){
    Serial.print(F("Wrong firmware loaded? Firmware: "));
    Serial.println(version);
    while(1) delay(10);
  }
  encoder_position = ss.getEncoderPosition();  // Initialize position

  sspixel.setBrightness(20);
  sspixel.show();
  ss.pinMode(SS_SWITCH, INPUT_PULLUP);

  delay(10);
  ss.setGPIOInterrupts((uint32_t)1 << SS_SWITCH, 1);
  ss.enableEncoderInterrupt();
  Serial.println(F("Seesaw initialized"));
}

void handleRotaryEncoder() {
  bool new_button_position = ss.digitalRead(SS_SWITCH);
  if (new_button_position != button_position) {
    button_position = new_button_position;
    if (button_position) {
      Serial.println(F("Button released"));
    } else {
      Serial.println(F("Button pushed"));
    }
  }

  int32_t new_position = ss.getEncoderPosition();
  if (encoder_position != new_position) {
    // Encoder positions are 32-bit; AVR printf's %d would truncate them.
    Serial.print(F("Encoder moved: "));
    Serial.println(new_position);
    encoder_position = new_position;
  }
}

// #include "fan.h"

// static fan_t fan;

// void setupFan() {
//     fan_cfg_t cfg;
//     fan_cfg_setup(&cfg);
//     FAN_MAP_MIKROBUS(cfg, MIKROBUS_2); // Use MIKROBUS_1 if the fan is in slot 1.
//     uint8_t info[3];
//     if (fan_init(&fan, &cfg) != I2C_MASTER_SUCCESS ||
//         fan_device_info(&fan, info) != I2C_MASTER_SUCCESS ||
//         fan_enable_fsc(&fan, 0) != I2C_MASTER_SUCCESS ||
//         fan_setting(&fan, 50) != I2C_MASTER_SUCCESS) {
//         Serial.println(F("Fan Click initialization failed"));
//         return;
//     }
//     Serial.println(F("Fan Click set to 50% duty"));
// }

#include "board.h"
#include "ipsdisplay2.h"
#include "ipsdisplay2_resources.h"

// MIKROE-1581 socket containing the IPS Display 2 Click.
// Change this to 2 if the display is in socket 2
#ifndef IPS_SOCKET
#define IPS_SOCKET 1
#endif
#if IPS_SOCKET != 1 && IPS_SOCKET != 2
#error "IPS_SOCKET must be 1 or 2."
#endif

static ipsdisplay2_t ipsdisplay2;

void setupIps() {
    ipsdisplay2_cfg_t ipsdisplay2_cfg;  /**< Click config object. */


    // Click initialization.
    ipsdisplay2_cfg_setup( &ipsdisplay2_cfg );
    IPSDISPLAY2_MAP_MIKROBUS( ipsdisplay2_cfg, IPS_SOCKET );
    if ( SPI_MASTER_ERROR == ipsdisplay2_init( &ipsdisplay2, &ipsdisplay2_cfg ) )
    {

        Serial.println(F("Display communication init failed"));
        for ( ; ; );
    }

    if ( IPSDISPLAY2_ERROR == ipsdisplay2_default_cfg ( &ipsdisplay2 ) )
    {
        Serial.println(F("Display default configuration failed"));
        for ( ; ; );
    }

    Serial.println(F("Display initialized"));
}

// Keep polling the encoder during the display demo's viewing pauses.
void displayPause(unsigned long ms) {
    unsigned long started = millis();
    while (millis() - started < ms) {
        handleRotaryEncoder();
        delay(10);
    }
}

void handleIps() {
    ipsdisplay2_point_t start_pt, end_pt;

#if IPSDISPLAY2_RESOURCES_INCLUDE_IMG
    Serial.print(F(" Drawing MIKROE logo example\r\n\n"));
    ipsdisplay2_draw_picture ( &ipsdisplay2, IPSDISPLAY2_ROTATION_VERTICAL_0, ipsdisplay2_img_mikroe );
    displayPause(3000);
#endif
    Serial.print(F(" Writing text example\r\n\n"));
    ipsdisplay2_fill_screen ( &ipsdisplay2, IPSDISPLAY2_COLOR_BLACK );
    displayPause(1000);
    start_pt.x = 60;
    start_pt.y = 70;
    ipsdisplay2_write_string ( &ipsdisplay2, start_pt, "      MIKROE      ", IPSDISPLAY2_COLOR_RED );
    start_pt.y += 20;
    ipsdisplay2_write_string ( &ipsdisplay2, start_pt, " IPS display Click", IPSDISPLAY2_COLOR_RED );
    start_pt.y += 20;
    ipsdisplay2_write_string ( &ipsdisplay2, start_pt, "     240x240px    ", IPSDISPLAY2_COLOR_RED );
    start_pt.y += 20;
    ipsdisplay2_write_string ( &ipsdisplay2, start_pt, "ST7789V controller", IPSDISPLAY2_COLOR_RED );
    start_pt.y += 20;
    ipsdisplay2_write_string ( &ipsdisplay2, start_pt, "   TEST EXAMPLE   ", IPSDISPLAY2_COLOR_RED );
    displayPause(3000);

    Serial.print(F(" RGB fill screen example\r\n\n"));
    ipsdisplay2_fill_screen ( &ipsdisplay2, IPSDISPLAY2_COLOR_RED );
    displayPause(1000);
    ipsdisplay2_fill_screen ( &ipsdisplay2, IPSDISPLAY2_COLOR_LIME );
    displayPause(1000);
    ipsdisplay2_fill_screen ( &ipsdisplay2, IPSDISPLAY2_COLOR_BLUE );
    displayPause(1000);

    Serial.print(F(" Drawing objects example\r\n\n"));
    ipsdisplay2_fill_screen ( &ipsdisplay2, IPSDISPLAY2_COLOR_BLACK );
    displayPause(1000);
    start_pt.x = IPSDISPLAY2_POS_WIDTH_MIN;
    start_pt.y = IPSDISPLAY2_POS_HEIGHT_MIN;
    end_pt.x = IPSDISPLAY2_POS_WIDTH_MAX;
    end_pt.y = IPSDISPLAY2_POS_HEIGHT_MAX;
    ipsdisplay2_draw_line ( &ipsdisplay2, start_pt, end_pt, IPSDISPLAY2_COLOR_BLUE );
    displayPause(1000);
    start_pt.x = IPSDISPLAY2_POS_WIDTH_MAX;
    start_pt.y = IPSDISPLAY2_POS_HEIGHT_MIN;
    end_pt.x = IPSDISPLAY2_POS_WIDTH_MIN;
    end_pt.y = IPSDISPLAY2_POS_HEIGHT_MAX;
    ipsdisplay2_draw_line ( &ipsdisplay2, start_pt, end_pt, IPSDISPLAY2_COLOR_BLUE );
    displayPause(1000);
    start_pt.x = 60;
    start_pt.y = 40;
    end_pt.x = 180;
    end_pt.y = 100;
    ipsdisplay2_draw_rectangle ( &ipsdisplay2, start_pt, end_pt, IPSDISPLAY2_COLOR_CYAN );
    displayPause(1000);
    start_pt.y += 100;
    end_pt.y += 100;
    ipsdisplay2_draw_rectangle ( &ipsdisplay2, start_pt, end_pt, IPSDISPLAY2_COLOR_CYAN );
    displayPause(1000);
    start_pt.x = 120;
    start_pt.y = 120;
    ipsdisplay2_draw_circle ( &ipsdisplay2, start_pt, 119, IPSDISPLAY2_COLOR_MAGENTA );
    displayPause(2000);
}

void setup() {

  // Initialize serial communications, which are needed to print to the serial monitor
  // in the Arduino IDE
  Serial.begin(9600);
  while (!Serial) delay(10);

  setupRotaryEncoder();
  setupIps();
}

void loop() {
  handleRotaryEncoder();
  handleIps();

  // don't overwhelm serial port
  delay(10);
}
