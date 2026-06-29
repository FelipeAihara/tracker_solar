#pragma once

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/sys/util.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>

#define DS3231_I2C_ADDR  0x68U
#define BIN2BCD(val) ((uint8_t)((((val) / 10) << 4) + ((val) % 10)))
#define BCD2BIN(val) ((uint8_t)((((val) >> 4) * 10) + ((val) & 0x0F)))

struct tempo {
    uint8_t sec, min, hour, wday, mday, month, year;
};

class MyRTC
{
private:
    struct tempo tempo_;
    const struct device *i2c_dev_;

public:
    MyRTC(const struct device *i2c_dev);
    int write(tempo *t);
    int read();
    void printHorario();
    const tempo& getTempo() const { return tempo_; }
};