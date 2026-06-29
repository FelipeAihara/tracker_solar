#include "lcd.hpp"
#include <zephyr/kernel.h>
#include <string.h>   /* strlen — disponível via picolibc (C, não C++) */

/* ------------------------------------------------------------------ */
/*  Construtor                                                          */
/* ------------------------------------------------------------------ */

LCD::LCD(const gpio_dt_spec &rs,
         const gpio_dt_spec &en,
         const gpio_dt_spec &d4,
         const gpio_dt_spec &d5,
         const gpio_dt_spec &d6,
         const gpio_dt_spec &d7)
    : rs_(rs), en_(en), d4_(d4), d5_(d5), d6_(d6), d7_(d7)
{}

/* ------------------------------------------------------------------ */
/*  Inicialização                                                       */
/* ------------------------------------------------------------------ */

int LCD::configurePin(const gpio_dt_spec &pin)
{
    if (!gpio_is_ready_dt(&pin)) {
        return -ENODEV;
    }
    return gpio_pin_configure_dt(&pin, GPIO_OUTPUT_INACTIVE);
}

int LCD::init()
{
    int ret;
    ret = configurePin(rs_); if (ret) return ret;
    ret = configurePin(en_); if (ret) return ret;
    ret = configurePin(d4_); if (ret) return ret;
    ret = configurePin(d5_); if (ret) return ret;
    ret = configurePin(d6_); if (ret) return ret;
    ret = configurePin(d7_); if (ret) return ret;

    /* Sequência de inicialização HD44780 em 4-bit (datasheet §4.4) */
    k_msleep(50);

    gpio_pin_set_dt(&rs_, 0);
    gpio_pin_set_dt(&en_, 0);

    write4bits(0x03); k_usleep(4500);
    write4bits(0x03); k_usleep(4500);
    write4bits(0x03); k_usleep(150);
    write4bits(0x02); /* entra em modo 4-bit */

    sendCommand(LCD_FUNCTIONSET | LCD_2LINE | LCD_5x8DOTS | LCD_4BITMODE);
    sendCommand(LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF);
    sendCommand(LCD_CLEARDISPLAY);
    k_msleep(2);
    sendCommand(LCD_ENTRYMODESET | LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT);

    initialized_ = true;
    return 0;
}

/* ------------------------------------------------------------------ */
/*  API pública                                                         */
/* ------------------------------------------------------------------ */

void LCD::writeLine1(const char *text)
{
    writeString(text, 0);
}

void LCD::writeLine2(const char *text)
{
    writeString(text, 1);
}

void LCD::clear()
{
    sendCommand(LCD_CLEARDISPLAY);
    k_msleep(2);
}

void LCD::home()
{
    sendCommand(LCD_RETURNHOME);
    k_msleep(2);
}

/* ------------------------------------------------------------------ */
/*  Métodos privados                                                    */
/* ------------------------------------------------------------------ */

void LCD::setCursor(uint8_t col, uint8_t row)
{
    const uint8_t rowOffsets[] = { ROW0_ADDR, ROW1_ADDR };
    if (row >= 2)    row = 1;
    if (col >= COLS) col = COLS - 1;
    sendCommand(LCD_SETDDRAMADDR | (col + rowOffsets[row]));
}

void LCD::writeString(const char *text, uint8_t row)
{
    if (!initialized_ || !text) return;

    setCursor(0, row);

    uint8_t len = (uint8_t)strlen(text);

    for (uint8_t i = 0; i < COLS; i++) {
        sendData((i < len) ? (uint8_t)text[i] : (uint8_t)' ');
    }
}

/* ------------------------------------------------------------------ */
/*  Comunicação com o HD44780                                           */
/* ------------------------------------------------------------------ */

void LCD::pulseEnable()
{
    gpio_pin_set_dt(&en_, 0); k_usleep(1);
    gpio_pin_set_dt(&en_, 1); k_usleep(1);
    gpio_pin_set_dt(&en_, 0); k_usleep(50);
}

void LCD::write4bits(uint8_t value)
{
    gpio_pin_set_dt(&d4_, (value >> 0) & 0x01);
    gpio_pin_set_dt(&d5_, (value >> 1) & 0x01);
    gpio_pin_set_dt(&d6_, (value >> 2) & 0x01);
    gpio_pin_set_dt(&d7_, (value >> 3) & 0x01);
    pulseEnable();
}

void LCD::send(uint8_t value, bool isData)
{
    gpio_pin_set_dt(&rs_, isData ? 1 : 0);
    write4bits(value >> 4);
    write4bits(value & 0x0F);
}

void LCD::sendCommand(uint8_t cmd)  { send(cmd,  false); }
void LCD::sendData(uint8_t data)    { send(data, true);  }
