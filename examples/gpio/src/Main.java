/*
 * 
 * SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (C) 2024 Oren Sokoler (https://github.com/orenskl)
 * 
 */

import pico.hardware.GPIOPin;

class Main {

    /* A simple GPIO input test */
    private static final int PIN_GPIO_TEST = 2;

    public static void main(String[] args) {
        System.out.println("GPIO example");
        GPIOPin p = new GPIOPin(PIN_GPIO_TEST, GPIOPin.IN, GPIOPin.PULLUP);
        while (true) {
            int value = p.get();
            if (value == GPIOPin.HIGH) {
                System.out.println("Pin state is HIGH");
            }
            else {
                System.out.println("Pin state is LOW");
            }
            try {
                Thread.sleep(500);
            } catch (Exception e) {
                System.out.println(e.getMessage());
            }
        }
    }
    
}