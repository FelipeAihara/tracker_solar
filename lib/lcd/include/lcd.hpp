#pragma once

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/**
 * @brief Driver para LCD HD44780 16x2 em modo 4-bit.
 *
 * Não usa STL (sem <string>, sem <ctime>) — compatível com a
 * libc++ mínima do Zephyr (-nostdinc++ / picolibc).
 * Strings são passadas como const char* (C-strings).
 */
class LCD {
public:
    LCD(const gpio_dt_spec &rs,
        const gpio_dt_spec &en,
        const gpio_dt_spec &d4,
        const gpio_dt_spec &d5,
        const gpio_dt_spec &d6,
        const gpio_dt_spec &d7);

    /**
     * @brief Inicializa o hardware. Deve ser chamado antes de qualquer escrita.
     * @return 0 em sucesso, código negativo em falha de GPIO.
     */
    int init();

    /**
     * @brief Escreve uma C-string na primeira linha (linha 0).
     *        Texto maior que 16 caracteres é truncado.
     *        O restante da linha é preenchido com espaços.
     */
    void writeLine1(const char *text);

    /**
     * @brief Escreve uma C-string na segunda linha (linha 1).
     *        Texto maior que 16 caracteres é truncado.
     *        O restante da linha é preenchido com espaços.
     */
    void writeLine2(const char *text);

    /** @brief Limpa o display e retorna o cursor à posição inicial. */
    void clear();

    /** @brief Retorna o cursor à posição inicial sem apagar o conteúdo. */
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
