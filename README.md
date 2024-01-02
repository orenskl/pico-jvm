# Pico JVM

This is a Java virtual machine for the [Raspberry Pi Pico](https://www.raspberrypi.com/products/raspberry-pi-pico/), this project is based on the [CLDC](https://en.wikipedia.org/wiki/Connected_Limited_Device_Configuration) virtual machine and more specificly on the [phoneME](https://phonej2me.github.io) project from Sun/Oracle. The [phoneME](https://phonej2me.github.io) is a very old project that currently is not maintaned anymore. However I was able to find a github [repo](https://github.com/magicus/phoneME) that I used as a reference. This JVM is targeted to small embedded devices with limited resources so don't expected a full blown Java experience on [Raspberry Pi Pico](https://www.raspberrypi.com/products/raspberry-pi-pico/).

The original [phoneME](https://phonej2me.github.io) project used a Makefile based build system, I converted the build system to CMake so I can use more modern tools and integration with modern CI/CD workflows. There are currently two main targets : Linux (used mainly for debugging) and Pico.

Currently the JVM support [CLDC 1.1](https://docs.oracle.com/javame/config/cldc/ref-impl/cldc1.1/jsr139/index.html) specification which is a limited subset of the Java  J2SE specification and language.
