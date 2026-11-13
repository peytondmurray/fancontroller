# Uno R3 / Click shield sketch

### Hardware

- Arduino UNO R3
- [Mikroe Click Shield (MIKROE-5739)](https://www.mikroe.com/click-shield-for-arduino-uno)
- [Fan Click (MIKROE-5739)](https://www.mikroe.com/fan-click)
- [IPS Display 2 Click (MIKROE-6078)](https://www.mikroe.com/ips-display-2-click)

| Display signal | mikroBUS pin | Socket 1 / Uno | Socket 2 / Uno |
| ---            | ---          | ---            | ---            |
| SCK            | SCK          | D13            | D13            |
| MOSI           | MOSI         | D11            | D11            |
| CS             | CS           | D10            | D9             |
| Reset          | RST          | A3             | A2             |
| Data/command   | INT          | D2             | D3             |
| Backlight      | AN           | A0             | A1             |


### Notes

- Set IPS_SOCKET (1 or 2) before running
- SEESAW_ADDR is the default 0x36
- Fan Click is at 0x2F
- Board mappings use Arduino pin numbers, including analog pins as digital
  outputs, rather than mikroSDK GPIO identifiers.

The full mikroSDK AVR HAL depends on MikroE toolchain configuration (for example
`Get_Fosc_kHz()`), so the local drivers use Arduino GPIO, SPI, Wire and delays
through `arduino_mikroe.*` instead. This adapter implements only the calls
needed by these two drivers.
