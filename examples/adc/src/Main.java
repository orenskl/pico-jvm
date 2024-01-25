/*
 * 
 * SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (C) 2024 Oren Sokoler (https://github.com/orenskl)
 * 
 */

import pico.hardware.ADCChannel;

class Main {

    /* A simple ADC test */
    private static final int ADC_CHANNEL = 2;

    public static void main(String[] args) {
        System.out.println("ADC example");
        ADCChannel a = new ADCChannel(ADC_CHANNEL);
        while (true) {
            int value = a.read();
            System.out.println("ADC Channel " + ADC_CHANNEL + " value is " + value);
            try {
                Thread.sleep(500);
            } catch (Exception e) {
                System.out.println(e.getMessage());
            }
        }
    }
    
}