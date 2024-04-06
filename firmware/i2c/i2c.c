#include "i2c.h"

#include <neorv32.h>

void i2c_read(uint8_t addr, uint8_t reg, void *buffer, size_t n)
{
    uint8_t data;

    neorv32_twi_generate_start();
    data = 0xFF & (addr << 1);
    neorv32_twi_trans(&data, 0);
    data = reg;
    neorv32_twi_trans(&data, 0);

    neorv32_twi_generate_start();
    data = (addr << 1) | 1;
    neorv32_twi_trans(&data, 0);

    for (int i = 0; i < n; i++)
    {
        data = 0xFF;
        neorv32_twi_trans(&data, i != n-1);
        ((uint8_t*)buffer)[i] = data;
    }

    neorv32_twi_generate_stop();
}

uint8_t i2c_read_u8(uint8_t addr, uint8_t reg)
{
    uint8_t data;
    i2c_read(addr, reg, &data, sizeof(uint8_t));
    return data;
}

uint16_t i2c_read_u16(uint8_t addr, uint8_t reg)
{
    uint16_t data;
    i2c_read(addr, reg, &data, sizeof(uint16_t));
    return data;
}

uint32_t i2c_read_u32(uint8_t addr, uint8_t reg)
{
    uint32_t data;
    i2c_read(addr, reg, &data, sizeof(uint32_t));
    return data;
}

int8_t i2c_read_s8(uint8_t addr, uint8_t reg)
{
    int8_t data;
    i2c_read(addr, reg, &data, sizeof(int8_t));
    return data;
}

int16_t i2c_read_s16(uint8_t addr, uint8_t reg)
{
    int16_t data;
    i2c_read(addr, reg, &data, sizeof(int16_t));
    return data;
}

int32_t i2c_read_s32(uint8_t addr, uint8_t reg)
{
    int32_t data;
    i2c_read(addr, reg, &data, sizeof(int32_t));
    return data;
}

void i2c_write(uint8_t addr, uint8_t reg, const void *buffer, size_t n)
{
    uint8_t data;

    neorv32_twi_generate_start();
    data = 0xFF & (addr << 1);
    neorv32_twi_trans(&data, 0);
    data = reg;
    neorv32_twi_trans(&data, 0);

    for (int i = 0; i < n; i++)
    {
        data = ((uint8_t*)buffer)[i];
        neorv32_twi_trans(&data, 0);
    }

    neorv32_twi_generate_stop();
}

void i2c_write_u8(uint8_t addr, uint8_t reg, uint8_t data)
{
    i2c_write(addr, reg, &data, sizeof(uint8_t));
}

void i2c_write_u16(uint8_t addr, uint8_t reg, uint16_t data)
{
    i2c_write(addr, reg, &data, sizeof(uint16_t));
}

void i2c_write_u32(uint8_t addr, uint8_t reg, uint32_t data)
{
    i2c_write(addr, reg, &data, sizeof(uint32_t));
}

void i2c_write_s8(uint8_t addr, uint8_t reg, int8_t data)
{
    i2c_write(addr, reg, &data, sizeof(int8_t));
}

void i2c_write_s16(uint8_t addr, uint8_t reg, int16_t data)
{
    i2c_write(addr, reg, &data, sizeof(int16_t));
}

void i2c_write_s32(uint8_t addr, uint8_t reg, int32_t data)
{
    i2c_write(addr, reg, &data, sizeof(int32_t));
}

