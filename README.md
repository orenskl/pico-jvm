[![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/orenskl/pico-jvm/main.yml?label=build)](https://github.com/orenskl/pico-jvm/actions/workflows/main.yml)
[![GitHub Tag](https://img.shields.io/github/v/tag/orenskl/pico-jvm)](https://github.com/orenskl/pico-jvm/tags)

# Pico JVM

This is a Java virtual machine for the [Raspberry Pi Pico](https://www.raspberrypi.com/products/raspberry-pi-pico/), this project is based on the [CLDC](https://en.wikipedia.org/wiki/Connected_Limited_Device_Configuration) virtual machine and more specifically on the [phoneME](https://phonej2me.github.io) project from Sun/Oracle. The [phoneME](https://phonej2me.github.io) is a very old project that currently is not maintained anymore. However I was able to find a github [repo](https://github.com/magicus/phoneME) that I used as a reference. This JVM is targeted to small embedded devices with limited resources so don't expected a full blown Java experience on [Raspberry Pi Pico](https://www.raspberrypi.com/products/raspberry-pi-pico/).

The original [phoneME](https://phonej2me.github.io) project used a Makefile based build system, I converted the build system to CMake so I can use more modern tools and integration with modern CI/CD workflows. There are currently two main targets : Linux (used mainly for debugging) and Pico.

Currently the JVM support [CLDC 1.1](https://docs.oracle.com/javame/config/cldc/ref-impl/cldc1.1/jsr139/index.html) specification which is a limited subset of the Java  J2SE specification and language.

## Features

+ Small footprint - 270KB Flash, 18KB RAM (not including the Java heap)
+ Java 1.4 and [CLDC 1.1](https://docs.oracle.com/javame/config/cldc/ref-impl/cldc1.1/jsr139/index.html) API
+ [Raspberry Pi Pico](https://www.raspberrypi.com/products/raspberry-pi-pico/) Low Level API (See [here](https://github.com/orenskl/pico-jvm/wiki/Examples))

## Installation and setup

Download the release package from the [Releases](https://github.com/orenskl/pico-jvm/releases) page of this repository, extract the package. The package contains the following content :

```
├── bin
├── doc
├── lib
└── pjvm-X.Y.Z.uf2
```

The `bin` directory contains tools and scripts required to post process class and jar files to be able to run them on the [Raspberry Pi Pico](https://www.raspberrypi.com/products/raspberry-pi-pico/). This directory also contains a Linux version of the virtual machine (`pjvm`) that may be used in development
stage to test applications on your host machine.

The `doc` directory contains the javadoc for the device specific (e.g. GPIO) classes.

The `lib` directory contains the run-time class libraries (`classes.jar`)

The `pjvm-X.Y.Z.uf2` (where X.Y.Z is the version of the firmware) is the Java Virtual Machine UF2 file, this file needs to be flashed to the [Raspberry Pi Pico](https://www.raspberrypi.com/products/raspberry-pi-pico/) using [picotool](https://github.com/raspberrypi/picotool). The virtual machine already contains the run time classes so these are not required to be flashed separately.

The `examples` directory in this repository contains some examples you can run with an [Ant](https://ant.apache.org) `build.xml` file. To build the examples and the following Hello World application you will need to setup the environment variable `JAVA_PICO_HOME` to point to the extracted package location.

For example if you extracted the package to `/opt/pjvm-X.Y.Z` you will need to setup the variable with the following command :

```
export JAVA_PICO_HOME=/opt/pjvm-X.Y.Z
```

You will also need [JDK 8](https://www.oracle.com/java/technologies/javase/javase8-archive-downloads.html) to build application, currently the latest versions of Java are not supported.

## Building and running a Java application

To run a Java application on the [Raspberry Pi Pico](https://www.raspberrypi.com/products/raspberry-pi-pico/) you will need to flash the Java virtual machine itself and than flash the Java application at the address `0x10100000`.

Lets say we have a simple hello world application :

```java
class Main {
    public static void main(String[] args) {
        System.out.println("Hello, World!"); 
    }
}
```

**NOTE : The name of the class `Main` is currently fixed as the first class that is loaded by the VM.**

1. Compile the class :

    ```
    javac -source 1.4 -target 1.4 -d main.dir -bootclasspath $JAVA_PICO_HOME/lib/classes.jar Main.java
    ```

    Make sure to setup you environment correctly so that `JAVA_PICO_HOME` points to the right place (see [here](#installation-and-setup))

2. Preverify the classes

    Before running the compiled classes on the [Raspberry Pi Pico](https://www.raspberrypi.com/products/raspberry-pi-pico/) we will need to Preverify them. Preverifying is the process of post processing the class files so they can be run more efficiently on the target system. Preverifying is done with the `preverify` tool in the `bin` directory of the package.

    ```
    $JAVA_PICO_HOME/bin/preverify -d main.preverify -classpath $JAVA_PICO_HOME/lib/classes.jar main.dir
    ```

3. Package the application as a JAR file :

    ```
    cd main.preverify
    jar -cfM0 ../main.jar .
    ```

4. Wrap the JAR file 

    Now we need to wrap the JAR with a header so we can run it on the Pi Pico :

    ```
    $JAVA_PICO_HOME/bin/wrapjar.sh main.jar main.jar.bin
    ```

    The `wrapjar.sh` script is located in the `bin` directory of the package.

5. Flash the binary file and reboot

    Now we can flash the application to address `0x10100000` using `picotool` :

    ```
    picotool load build/main.jar.bin --offset 10100000
    ```

    Reboot your Pi Pico and you should see `Hello, World!` on your terminal

This repository includes an `example` directory with a complete [Ant](https://ant.apache.org) `build.xml` for each example that runs all the above steps in a single command.

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
cmake -DTARGET=PICO -DPICO_SDK_PATH=/home/oren/projects/pico-sdk .. -G Ninja
cmake --build .
```

Make sure you set `PICO_SDK_PATH` to point to your Pico SDK location.

If all goes well you should end up with a `pjvm.uf2` file in your `build` directory. This file can be flashed to the Pi Pico (helper scripts can be found in the `tools` directory). The `pjvm.uf2` file is the Java VM itself and includes the system classes already romized inside it. A Java application is loaded separately into the flash of the Pi Pico at a specific address.

To build the virtual machine for Linux use these commands :

```
mkdir build
cd build
cmake -DTARGET=LINUX .. -G Ninja
cmake --build .
```

The output of this build is a Linux executable named `pjvm`.


