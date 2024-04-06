#pragma once

#include <stdint.h>
#include <stddef.h>

void i2c_read(uint8_t addr, uint8_t reg, void *data, size_t n);
uint8_t i2c_read_u8(uint8_t addr, uint8_t reg);
uint16_t i2c_read_u16(uint8_t addr, uint8_t reg);
uint32_t i2c_read_u32(uint8_t addr, uint8_t reg);
int8_t i2c_read_s8(uint8_t addr, uint8_t reg);
int16_t i2c_read_s16(uint8_t addr, uint8_t reg);
int32_t i2c_read_s32(uint8_t addr, uint8_t reg);

void i2c_write(uint8_t addr, uint8_t reg, const void *buffer, size_t n);
void i2c_write_u8(uint8_t addr,  uint8_t reg, uint8_t data);
void i2c_write_u16(uint8_t addr, uint8_t reg, uint16_t data);
void i2c_write_u32(uint8_t addr, uint8_t reg, uint32_t data);
void i2c_write_s8(uint8_t addr, uint8_t reg, int8_t data);
void i2c_write_s16(uint8_t addr, uint8_t reg, int16_t data);
void i2c_write_s32(uint8_t addr, uint8_t reg, int32_t data);
