![Build status](https://github.com/orenskl/pico-jvm/actions/workflows/main.yml/badge.svg)

# Pico JVM

This is a Java virtual machine for the [Raspberry Pi Pico](https://www.raspberrypi.com/products/raspberry-pi-pico/), this project is based on the [CLDC](https://en.wikipedia.org/wiki/Connected_Limited_Device_Configuration) virtual machine and more specificly on the [phoneME](https://phonej2me.github.io) project from Sun/Oracle. The [phoneME](https://phonej2me.github.io) is a very old project that currently is not maintaned anymore. However I was able to find a github [repo](https://github.com/magicus/phoneME) that I used as a reference. This JVM is targeted to small embedded devices with limited resources so don't expected a full blown Java experience on [Raspberry Pi Pico](https://www.raspberrypi.com/products/raspberry-pi-pico/).

The original [phoneME](https://phonej2me.github.io) project used a Makefile based build system, I converted the build system to CMake so I can use more modern tools and integration with modern CI/CD workflows. There are currently two main targets : Linux (used mainly for debugging) and Pico.

Currently the JVM support [CLDC 1.1](https://docs.oracle.com/javame/config/cldc/ref-impl/cldc1.1/jsr139/index.html) specification which is a limited subset of the Java  J2SE specification and language.

## Running a Java application

To run a Java application on the [Raspberry Pi Pico](https://www.raspberrypi.com/products/raspberry-pi-pico/) you will need to flash the Java virtual machine itself and than flash the Java application at the address `0x10100000`.

The Java VM can be dowloaded from the [Releases](https://github.com/orenskl/pico-jvm/releases) page of this project. You can download the gzipped UF2 file, unpack it and flash it with the standard [picotool(https://github.com/raspberrypi/picotool).

Lets say we have a simple hello world application :

```java
class Main {
    public static void main(String[] args) {
        System.out.println("Hello, World!"); 
    }
}
```

The name of the class `Main` is currently fixed as the first class that is loaded by the VM. Compiler the class :

```
javac -source 1.4 -target 1.4 -d main.dir -bootclasspath ../pico-jvm/build/classes.jar Main.java
```

Make sure your `-bootclasspath` is correct and should point to the `classes.jar` that is built earilier

Package the application as a JAR file :

```
cd main.dir
jar -cfM0 ../main.jar .
```

Now we need to wrap the JAR with a header so we can run it on the Pi Pico :

```
tools/wrapjar.sh main.jar main.jar.bin
```

The `wrapjar` script is located in the `tools` directory of this project. Now we can flash the application to address `0x10100000` using `picotool` :

```
picotool load build/main.jar.bin --offset 10100000
```

Reboot your Pi Pico and you should see `Hello, World!` on your terminal

## Building

This project can be built on Ubuntu 22 as the build machine, please install the following packages :

```
sudo apt-get install -y gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential gcc-multilib g++-multilib ninja-build
```


You will also need JDK 8 (Yes 8) for this, please install it and make sure it is your default Java installation.

After cloning the project cd into it and run the the usual CMake commands :

```
mkdir build
cd build
cmake -DPICO=TRUE -DPICO_SDK_PATH=/home/oren/projects/pico-sdk .. -G Ninja
cmake --build .
```

Make sure you set `PICO_SDK_PATH` to point to your Pico SDK location.

If all goes well you should end up with a `pjvm.uf2` file in your `build` directory. This file can be flashed to the Pi Pico (helper scripts can be found in the `tools` directory). The `pjvm.uf2` file is the Java VM itself and includes the system classes already romized inside it. A Java application is loaded separately into the flash of the Pi Pico at a specific address.

