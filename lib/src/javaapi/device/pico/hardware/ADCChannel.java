/*
 * 
 * SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (C) 2024 Oren Sokoler (https://github.com/orenskl)
 * 
 */

 package pico.hardware;

 /**
  * A <code>ADCChannel</code> class represents a single Analog To Digital
  * conversion channel, the channel number is hardware specific and is not 
  * decoded or encoded in anyway, it is passed to the hardware level as is. 
  * The values that are read from the ADC are integers and are the direct 
  * read of the ADC, e.g. if the ADC is a 12 bit ADC then the values you 
  * could get are from 0 to 4095.
  */
 
public class ADCChannel {
    
    /**
     * The ADC channel number
     */
    private int channel;

    /**
     * 
     * Constructor for a single ADC channel
     * 
     * @param channel The channel number
     * 
     */
    public ADCChannel ( int channel ) {
        this.channel = channel;
        adc_init(channel);
    }

    /**
     * Perform a single conversion and read its value from the ADC channel
     * 
     * @return the ADC channel digital value
     */
    public int read () {
        return adc_read(this.channel);
    }

    /*
     * Natives for the device hardware
     */
    private static native void adc_init ( int channel );
    private static native int  adc_read ( int channel );

}
