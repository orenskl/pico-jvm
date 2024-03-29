/*
 * 
 * SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (C) 2024 Oren Sokoler (https://github.com/orenskl)
 * 
 */

#include <stdio.h>

#include "kni.h"

#include "hardware/gpio.h"
#include "hardware/adc.h"

#define         ADC_GPIO_PIN_MIN        26
#define         ADC_GPIO_PIN_MAX        29

extern "C" {

/*
 * This file is the native implementation of the device specific
 * classes that access the hardware, these functions are called from
 * the Java classes. All the functions are void but they get their
 * input parameters from the JVM stack via KNI calls
 */

/**
 * Initialize a GPIO pin
 * 
 * @param The 1st parameter is the pin number
 * @param The 2nd parameter is the pin direction
 * 
 */
void Java_pico_hardware_GPIOPin_gpio_1init( void )
{
    int pinNumber = KNI_GetParameterAsInt(1);
    int direction = KNI_GetParameterAsInt(2);
    gpio_init(pinNumber);
    gpio_set_dir(pinNumber, direction);
}

/**
 * Set a pin output state
 * 
 * @param The 1st parameter is the pin number
 * @param The 2nd parameter is the pin state
 * 
 */
void Java_pico_hardware_GPIOPin_gpio_1set( void )
{
    int pinNumber = KNI_GetParameterAsInt(1);
    int value     = KNI_GetParameterAsInt(2);
    gpio_put(pinNumber, value);
}

/**
 * Set a pin pull up or down resistor
 * 
 * @param The 1st parameter is the pin number
 * @param The 2nd parameter is 1 for pull up, 0 for pull down
 * 
 */
void Java_pico_hardware_GPIOPin_gpio_1set_1pull( void )
{
    int pinNumber = KNI_GetParameterAsInt(1);
    int pullUp    = KNI_GetParameterAsInt(2);
    if (pullUp) {
        gpio_pull_up(pinNumber);
    }
    else {
        gpio_pull_down(pinNumber);
    }
}

/**
 * Returns the state of the pin (high or low)
 * 
 * @param The 1st parameter is the pin number
 * 
 */
int Java_pico_hardware_GPIOPin_gpio_1get( void )
{
    int pinNumber = KNI_GetParameterAsInt(1);
    return gpio_get(pinNumber);
}

/**
 * Initialize an ADC channel
@ * 
 * @param The 1st parameter is the channel number
 * 
 */
void Java_pico_hardware_ADCChannel_adc_1init ( void )
{
    int channel = KNI_GetParameterAsInt(1);
    adc_gpio_init(ADC_GPIO_PIN_MIN + channel);
}

/**
 * Read a single sample from the ADC channel
 * 
 * @param The 1st parameter is the channel number
 * @return The sample read as an integer
 * 
 */
int Java_pico_hardware_ADCChannel_adc_1read ( void ) 
{
    int channel = KNI_GetParameterAsInt(1);
    adc_select_input(channel);
    return adc_read();
}

} /* extern "C" */