
#pragma once

#include <driver/i2c.h>

void SE_REG(uint8_t device_address, uint8_t register_address);

uint8_t I2C_READ_BYTES(uint8_t device_address,uint8_t register_address,uint8_t size,uint8_t* data);
uint8_t I2C_READ_BYTE(uint8_t device_address,uint8_t register_address,uint8_t* data);
uint8_t I2C_READ_BITS(uint8_t device_address,uint8_t register_address,uint8_t bit_start,uint8_t size,uint8_t* data);
uint8_t I2C_READ_BIT(uint8_t device_address,uint8_t register_address,uint8_t bit_number,uint8_t* data);

bool I2C_WRITE_BYTES(uint8_t device_address,uint8_t register_address,uint8_t size,uint8_t* data);
bool I2C_WRITE_BYTE(uint8_t device_address,uint8_t register_address,uint8_t data);
bool I2C_WRITE_BITS(uint8_t device_address,uint8_t register_address,uint8_t bit_start,uint8_t size,uint8_t data);
bool I2C_WRITE_BIT(uint8_t device_address,uint8_t register_address,uint8_t bit_number,uint8_t data);
bool I2C_WRITE_WORD(uint8_t device_address,uint8_t register_address,uint8_t data);