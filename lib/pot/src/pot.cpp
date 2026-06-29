#include "pot.hpp"

Potenciometro::Potenciometro(const struct device *adc, adc_channel_cfg adc_ch) :
    adc_(adc), adc_ch_(adc_ch)
{   
    vref_mv = DT_PROP(DT_ALIAS(my_adc_channel), zephyr_vref_mv);
    seq = {
        .channels = BIT(adc_ch_.channel_id),
        .buffer = &buf,
        .buffer_size = sizeof(buf),
        .resolution = DT_PROP(DT_ALIAS(my_adc_channel), zephyr_resolution)
    };

    if (!device_is_ready(adc_)) {
        printk("ADC peripheral is not ready\r\n");
    }

    int ret = adc_channel_setup(adc_, &adc_ch_);
    if (ret < 0) {
        printk("Could not set up ADC\r\n");
    }
}

int Potenciometro::read()
{
    if (adc_ == NULL || !device_is_ready(adc_)) {
        return -1; 
    }

    int theta;
    int ret = adc_read(adc_, &seq);
    if (ret < 0) {
        printk("Could not read ADC: %d\r\n", ret);
    }

    val_mv = ((uint32_t)buf * vref_mv )/ (1 << seq.resolution);
    theta = (val_mv * 180) / 3300;
    return theta;
}