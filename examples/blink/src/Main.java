/*
 * 
 * SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (C) 2024 Oren Sokoler (https://github.com/orenskl)
 * 
 */

import pico.hardware.GPIOPin;

class Main {

    /* This is where the LED is connected on the RPi Pico */
    private static final int PIN_LED = 25;

    public static void main(String[] args) {
        System.out.println("Blinking LED example");
        GPIOPin p = new GPIOPin(PIN_LED, GPIOPin.OUT);
        while (true) {
            try {
                p.set(GPIOPin.HIGH);
                Thread.sleep(1000);
                p.set(GPIOPin.LOW);
                Thread.sleep(1000);
            } catch (Exception e) {
                System.out.println(e.getMessage());
            }
        }
    }
    
}