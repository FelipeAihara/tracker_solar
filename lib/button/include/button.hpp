#pragma once

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

typedef enum
{
    MODO_AUTOMATICO,
    MODO_MANUAL_1G,
    MODO_MANUAL_2B
} Modo;

class Button
{
private:
    const struct gpio_dt_spec btn_;
    struct gpio_callback button_cb_data_;

    friend void button_isr_static(const struct device *dev, struct gpio_callback *cb, uint32_t pins);

public:
    Modo modo_atual_;
    Button(const struct gpio_dt_spec btn);
    void handle_interrupt();

    int getModoAtual() { return modo_atual_; } 
};