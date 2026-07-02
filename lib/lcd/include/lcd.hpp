#pragma once

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

class LCD {
public:
    LCD(const gpio_dt_spec &rs,
        const gpio_dt_spec &en,
        const gpio_dt_spec &d4,
        const gpio_dt_spec &d5,
        const gpio_dt_spec &d6,
        const gpio_dt_spec &d7);

    int init();

    void writeLine1(const char *text);

    void writeLine2(const char *text);

    void clear();

    void home();

private:
    gpio_dt_spec rs_, en_, d4_, d5_, d6_, d7_;
    bool initialized_{false};

    /* Comandos HD44780 */
    static constexpr uint8_t LCD_CLEARDISPLAY   = 0x01;
    static constexpr uint8_t LCD_RETURNHOME     = 0x02;
    static constexpr uint8_t LCD_ENTRYMODESET   = 0x04;
    static constexpr uint8_t LCD_DISPLAYCONTROL = 0x08;
    static constexpr uint8_t LCD_FUNCTIONSET    = 0x20;
    static constexpr uint8_t LCD_SETDDRAMADDR   = 0x80;

    static constexpr uint8_t LCD_DISPLAYON              = 0x04;
    static constexpr uint8_t LCD_CURSOROFF              = 0x00;
    static constexpr uint8_t LCD_BLINKOFF               = 0x00;
    static constexpr uint8_t LCD_ENTRYLEFT              = 0x02;
    static constexpr uint8_t LCD_ENTRYSHIFTDECREMENT    = 0x00;
    static constexpr uint8_t LCD_4BITMODE               = 0x00;
    static constexpr uint8_t LCD_2LINE                  = 0x08;
    static constexpr uint8_t LCD_5x8DOTS                = 0x00;

    static constexpr uint8_t ROW0_ADDR = 0x00;
    static constexpr uint8_t ROW1_ADDR = 0x40;
    static constexpr uint8_t COLS      = 16;

    int  configurePin(const gpio_dt_spec &pin);
    void pulseEnable();
    void write4bits(uint8_t value);
    void send(uint8_t value, bool isData);
    void sendCommand(uint8_t cmd);
    void sendData(uint8_t data);
    void setCursor(uint8_t col, uint8_t row);
    void writeString(const char *text, uint8_t row);
};
