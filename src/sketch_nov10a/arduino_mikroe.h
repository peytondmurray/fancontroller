// Arduino Uno implementation of the mikroSDK calls used by these two Click drivers.
// This is a small compatibility layer, not a replacement for the complete mikroSDK.
#ifndef ARDUINO_MIKROE_H
#define ARDUINO_MIKROE_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#if !defined(ARDUINO_AVR_UNO) || !defined(__AVR_ATmega328P__)
#error "Select Arduino Uno (ATmega328P) for this sketch."
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef int8_t err_t;
typedef uint8_t pin_name_t;
#define HAL_PIN_NC 0xFF

typedef struct { pin_name_t pin; } digital_out_t;
typedef struct { pin_name_t pin; } digital_in_t;
#define DIGITAL_IN_UNSUPPORTED_PIN (-1)

err_t digital_out_init(digital_out_t *ctx, pin_name_t pin);
void digital_out_high(digital_out_t *ctx);
void digital_out_low(digital_out_t *ctx);
err_t digital_in_init(digital_in_t *ctx, pin_name_t pin);
uint8_t digital_in_read(digital_in_t *ctx);

typedef enum {
    SPI_MASTER_MODE_0, SPI_MASTER_MODE_1, SPI_MASTER_MODE_2, SPI_MASTER_MODE_3
} spi_master_mode_t;
typedef struct {
    pin_name_t sck, miso, mosi;
} spi_master_config_t;
typedef struct {
    uint32_t speed;
    spi_master_mode_t mode;
    uint8_t dummy;
} spi_master_t;
#define SPI_MASTER_SUCCESS 0
#define SPI_MASTER_ERROR (-1)

void spi_master_configure_default(spi_master_config_t *cfg);
err_t spi_master_open(spi_master_t *ctx, const spi_master_config_t *cfg);
err_t spi_master_set_default_write_data(spi_master_t *ctx, uint8_t data);
err_t spi_master_set_mode(spi_master_t *ctx, spi_master_mode_t mode);
err_t spi_master_set_speed(spi_master_t *ctx, uint32_t speed);
err_t spi_master_write(spi_master_t *ctx, const uint8_t *data, size_t len);

typedef struct { pin_name_t scl, sda; } i2c_master_config_t;
typedef struct {
    uint8_t address;
    uint32_t speed;
} i2c_master_t;
#define I2C_MASTER_SUCCESS 0
#define I2C_MASTER_ERROR (-1)
#define I2C_MASTER_SPEED_STANDARD 100000UL
// AVR Wire has a 32-byte transaction buffer (including the register address).
#define ARDUINO_MIKROE_I2C_BUFFER_SIZE 32

void i2c_master_configure_default(i2c_master_config_t *cfg);
err_t i2c_master_open(i2c_master_t *ctx, const i2c_master_config_t *cfg);
err_t i2c_master_set_slave_address(i2c_master_t *ctx, uint8_t address);
err_t i2c_master_set_speed(i2c_master_t *ctx, uint32_t speed);
err_t i2c_master_set_timeout(i2c_master_t *ctx, uint16_t timeout_ms);
err_t i2c_master_write(i2c_master_t *ctx, const uint8_t *data, size_t len);
err_t i2c_master_write_then_read(i2c_master_t *ctx, const uint8_t *tx,
                               size_t tx_len, uint8_t *rx, size_t rx_len);

void mikroe_delay_ms(uint32_t ms);
#define Delay_1ms() mikroe_delay_ms(1)
#define Delay_10ms() mikroe_delay_ms(10)
#define Delay_100ms() mikroe_delay_ms(100)

#ifdef __cplusplus
}
#endif
#endif
