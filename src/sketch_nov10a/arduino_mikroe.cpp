#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "arduino_mikroe.h"

err_t digital_out_init(digital_out_t *ctx, pin_name_t pin) {
    if (pin >= NUM_DIGITAL_PINS) return -1;
    ctx->pin = pin;
    digitalWrite(pin, LOW);
    pinMode(pin, OUTPUT);
    return 0;
}

void digital_out_high(digital_out_t *ctx) { digitalWrite(ctx->pin, HIGH); }
void digital_out_low(digital_out_t *ctx) { digitalWrite(ctx->pin, LOW); }

err_t digital_in_init(digital_in_t *ctx, pin_name_t pin) {
    if (pin >= NUM_DIGITAL_PINS) return DIGITAL_IN_UNSUPPORTED_PIN;
    ctx->pin = pin;
    pinMode(pin, INPUT);
    return 0;
}

uint8_t digital_in_read(digital_in_t *ctx) { return digitalRead(ctx->pin); }

void spi_master_configure_default(spi_master_config_t *cfg) {
    cfg->sck = SCK;
    cfg->miso = MISO;
    cfg->mosi = MOSI;
}

err_t spi_master_open(spi_master_t *ctx, const spi_master_config_t *cfg) {
    if (cfg->sck != SCK || cfg->miso != MISO || cfg->mosi != MOSI) {
        return SPI_MASTER_ERROR;
    }
    ctx->speed = 4000000UL;
    ctx->mode = SPI_MASTER_MODE_0;
    ctx->dummy = 0;
    // SPI.begin also keeps hardware SS (D10) an output, even with slot 2 CS.
    SPI.begin();
    return SPI_MASTER_SUCCESS;
}

err_t spi_master_set_default_write_data(spi_master_t *ctx, uint8_t data) {
    ctx->dummy = data;
    return SPI_MASTER_SUCCESS;
}

err_t spi_master_set_mode(spi_master_t *ctx, spi_master_mode_t mode) {
    if (mode < SPI_MASTER_MODE_0 || mode > SPI_MASTER_MODE_3) {
        return SPI_MASTER_ERROR;
    }
    ctx->mode = mode;
    return SPI_MASTER_SUCCESS;
}

err_t spi_master_set_speed(spi_master_t *ctx, uint32_t speed) {
    if (!speed) return SPI_MASTER_ERROR;
    ctx->speed = speed;
    return SPI_MASTER_SUCCESS;
}

err_t spi_master_write(spi_master_t *ctx, const uint8_t *data, size_t len) {
    if (!data && len) return SPI_MASTER_ERROR;
    // AVR SPI_MODE0..3 encode CPOL/CPHA in bits 3:2.
    SPI.beginTransaction(SPISettings(ctx->speed, MSBFIRST, (uint8_t)ctx->mode << 2));
    for (size_t i = 0; i < len; ++i) SPI.transfer(data[i]);
    SPI.endTransaction();
    return SPI_MASTER_SUCCESS;
}

void i2c_master_configure_default(i2c_master_config_t *cfg) {
    cfg->scl = SCL;
    cfg->sda = SDA;
}

err_t i2c_master_open(i2c_master_t *ctx, const i2c_master_config_t *cfg) {
    if (cfg->scl != SCL || cfg->sda != SDA) return I2C_MASTER_ERROR;
    ctx->address = 0;
    ctx->speed = I2C_MASTER_SPEED_STANDARD;
    // The Fan Click and seesaw share the same Arduino Wire instance.
    Wire.begin();
    return I2C_MASTER_SUCCESS;
}

err_t i2c_master_set_slave_address(i2c_master_t *ctx, uint8_t address) {
    if (address > 0x7F) return I2C_MASTER_ERROR;
    ctx->address = address;
    return I2C_MASTER_SUCCESS;
}

err_t i2c_master_set_speed(i2c_master_t *ctx, uint32_t speed) {
    if (speed != 100000UL && speed != 400000UL) return I2C_MASTER_ERROR;
    ctx->speed = speed;
    Wire.setClock(speed);
    return I2C_MASTER_SUCCESS;
}

err_t i2c_master_set_timeout(i2c_master_t *ctx, uint16_t timeout_ms) {
    (void)ctx;
    Wire.setWireTimeout((uint32_t)timeout_ms * 1000UL, true);
    return I2C_MASTER_SUCCESS;
}

err_t i2c_master_write(i2c_master_t *ctx, const uint8_t *data, size_t len) {
    if ((!data && len) || len > ARDUINO_MIKROE_I2C_BUFFER_SIZE) {
        return I2C_MASTER_ERROR;
    }
    Wire.setClock(ctx->speed);
    Wire.beginTransmission(ctx->address);
    size_t written = Wire.write(data, len);
    uint8_t status = Wire.endTransmission();
    return written == len && status == 0 ? I2C_MASTER_SUCCESS : I2C_MASTER_ERROR;
}

err_t i2c_master_write_then_read(i2c_master_t *ctx, const uint8_t *tx,
                               size_t tx_len, uint8_t *rx, size_t rx_len) {
    if ((!tx && tx_len) || (!rx && rx_len) || !rx_len ||
        tx_len > ARDUINO_MIKROE_I2C_BUFFER_SIZE ||
        rx_len > ARDUINO_MIKROE_I2C_BUFFER_SIZE) return I2C_MASTER_ERROR;

    Wire.setClock(ctx->speed);
    Wire.beginTransmission(ctx->address);
    size_t written = Wire.write(tx, tx_len);
    uint8_t status = Wire.endTransmission(false); // repeated START for register read
    if (status != 0 || written != tx_len) return I2C_MASTER_ERROR;
    uint8_t received = Wire.requestFrom(ctx->address, (uint8_t)rx_len, (uint8_t)true);
    for (uint8_t i = 0; i < received; ++i) rx[i] = Wire.read();
    return received == rx_len ? I2C_MASTER_SUCCESS : I2C_MASTER_ERROR;
}

void mikroe_delay_ms(uint32_t ms) { delay(ms); }
