#ifndef _IPSDISPLAY2_RESOURCES_H_
#define _IPSDISPLAY2_RESOURCES_H_
#include <stdint.h>
#include <avr/pgmspace.h>

#ifdef __cplusplus
extern "C" {
#endif

// A full 240x240 RGB565 bitmap needs 115200 bytes: it cannot fit on the Uno.
#define IPSDISPLAY2_RESOURCES_INCLUDE_IMG   0

/**
 * @brief Memory buffers for 240x240 IPS display.
 */

#if IPSDISPLAY2_RESOURCES_INCLUDE_IMG
#error "Full-screen bitmap resources do not fit in ATmega328P memory."
// 'MIKROE' image, 240x240px
extern const uint16_t ipsdisplay2_img_mikroe[ ];
#endif

// 6x12 font
extern const uint8_t ipsdisplay2_font_6x12[ ] PROGMEM;

#ifdef __cplusplus
}
#endif

#endif // _IPSDISPLAY2_RESOURCES_H_
