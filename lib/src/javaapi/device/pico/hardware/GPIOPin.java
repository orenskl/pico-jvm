/*
 * 
 * SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (C) 2024 Oren Sokoler (https://github.com/orenskl)
 * 
 */

package pico.hardware;

/**
 * A <code>GPIOPin</code> represents a single GPIO pin, it can be configured
 * to be either as an input or as an output. If configures as an input it can
 * have a pull up or pull down resistor. Each GPIO pin is identified by its
 * number, this number is passed directly to the hardware libray of the 
 * device without any encoding or decoding.
 */
public class GPIOPin {

    /**
     * Output direction for the GPIO pin
     */
    public static final int OUT = 1;

    /**
     * Input direction for the GPIO pin
     */
    public static final int IN  = 0;

    /**
     * High (High voltage) for a GPIO pin
     */
    public static final int HIGH = 1;

    /**
     * Low (Low voltage 0V) for a GPIO pin
     */
    public static final int LOW  = 0;

    /**
     * A pull up resistor is connected to the pin
     */
    public static final int PULLUP = 1;

    /**
     * A pull down resistor is connected to the pin
     */
    public static final int PULLDOWN  = 0;

    /**
     * The GPIO pin number
     */
    private int pinNumber;

    /**
     * 
     * Constructor for a GPIO pin
     * 
     * @param pinNumber The pin number
     * @param pinType The pin direction ({@link GPIOPin#OUT} or {@link GPIOPin#IN})
     * 
     * @throws IllegalArgumentException
     * 
     */
    public GPIOPin( int pinNumber, int pinType ) {
        if ((pinType != OUT) && (pinType != IN)) {
            throw new IllegalArgumentException();
        } 
        this.pinNumber = pinNumber;
        gpio_init(pinNumber,pinType);
    }

    /**
     * 
     * Constructor for a GPIO pin with a pull up or pull down resistor
     * 
     * @param pinNumber The pin number
     * @param pinType The pin direction ({@link GPIOPin#OUT} or {@link GPIOPin#IN})
     * @param pinPull The resistor type ({@link GPIOPin#PULLUP} or {@link GPIOPin#PULLDOWN})
     * 
     * @throws IllegalArgumentException
     * 
     */
    public GPIOPin( int pinNumber, int pinType, int pinPull) {
        this(pinNumber,pinType);
        if ((pinPull != PULLUP) && (pinPull != PULLDOWN)) {
            throw new IllegalArgumentException();
        } 
        gpio_set_pull(pinNumber,pinPull);
    }

    /**
     * 
     * Set the state of the pin to either HIGH or LOW
     * 
     * @param pinState the state of the pin ({@link GPIOPin#HIGH} or {@link GPIOPin#LOW})
     */
    public void set ( int pinState ) {
        gpio_set(this.pinNumber,pinState);
    }

    /**
     * Get the state of the pin ({@link GPIOPin#HIGH} or {@link GPIOPin#LOW})
     * 
     * @return state of the pin
     */
    public int get () {
        return gpio_get(this.pinNumber);
    }

    /*
     * Natives for the device hardware
     */
    private static native void gpio_init     ( int pinNumber , int pinType );
    private static native void gpio_set      ( int pinNumber , int pinState );
    private static native int  gpio_get      ( int pinNumber );
    private static native void gpio_set_pull ( int pinNumber , int pinPull );

}
