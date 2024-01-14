#
# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (C) 2024 Oren Sokoler (https://github.com/orenskl) 
#

cmake_minimum_required(VERSION 3.22)

if (NOT DEFINED TARGET)
    set(TARGET PICO)
endif()

find_package(Java 1.8 EXACT REQUIRED)
include(ExternalProject)

if (${TARGET} STREQUAL PICO)
    # initialize the SDK based on PICO_SDK_PATH
    # note: this must happen before project()
    include(pico_sdk_import.cmake)
endif()

project(pjvm VERSION 0.0.2)

include(CTest) 

if (${TARGET} STREQUAL PICO)
    # initialize the Raspberry Pi Pico SDK
    pico_sdk_init()
endif()

ExternalProject_Add(libjvm SOURCE_DIR ${CMAKE_SOURCE_DIR}/lib
                           CMAKE_ARGS -DLIBJVM_INSTALL_DIR=${CMAKE_BINARY_DIR} 
                                      -DLIBJVM_VERSION=${PROJECT_VERSION}
                                      -DCMAKE_BUILD_TYPE:STRING=MinSizeRel
                                      -DTARGET:STRING=${TARGET}
                           USES_TERMINAL_BUILD TRUE
)

if (${TARGET} STREQUAL PICO)
    set(PJVM_SOURCES src/pico/Main.cpp src/pico/OS.cpp src/pico/Natives.cpp)
endif()
if (${TARGET} STREQUAL LINUX)
    set(PJVM_SOURCES cldc/src/vm/os/linux/Main_linux.cpp)
endif()

add_executable(pjvm ${PJVM_SOURCES})
add_dependencies(pjvm libjvm)

if (${TARGET} STREQUAL PICO)
    set(PJVM_INCLUDE_OS_DIRS lib/src/vm/os/pico)
endif()
if (${TARGET} STREQUAL LINUX)
    set(PJVM_INCLUDE_OS_DIRS lib/src/vm/os/linux)
endif()

target_include_directories(pjvm PRIVATE 
                    lib/src/include 
                    lib/src/vm/share/runtime
                    lib/src/vm/share/utilities
                    lib/src/vm/share/natives
                    lib/src/vm/cpu/c
                    lib/src/vm/share/ROM
                    ${PJVM_INCLUDE_OS_DIRS}
)

if (${TARGET} STREQUAL PICO)
    target_compile_definitions(pjvm PRIVATE PICO ARM SUPPORTS_MONOTONIC_CLOCK=1)
endif()

if (${TARGET} STREQUAL PICO)
    # Add pico_stdlib library which aggregates commonly used features
    target_link_libraries(pjvm jvm pico_stdlib)
    target_link_directories(pjvm PRIVATE ${CMAKE_BINARY_DIR})
    # create map/bin/hex/uf2 file in addition to ELF.
    pico_add_extra_outputs(pjvm)
    pico_enable_stdio_usb(pjvm 1)
    pico_enable_stdio_uart(pjvm 0)
endif()

add_custom_command(OUTPUT main.jar.bin 
                   DEPENDS test/Main.java pjvm
                   COMMAND mkdir ARGS -p ${CMAKE_BINARY_DIR}/main.dir
                   COMMAND javac ARGS -source 1.4 -target 1.4 -d ${CMAKE_BINARY_DIR}/main.dir -bootclasspath ${CMAKE_BINARY_DIR}/classes.jar
                        ${CMAKE_SOURCE_DIR}/test/Main.java
                   COMMAND cd ARGS ${CMAKE_BINARY_DIR}/main.dir
                   COMMAND jar ARGS -cfM0 ${CMAKE_BINARY_DIR}/main.jar .
                   COMMAND ${CMAKE_SOURCE_DIR}/tools/wrapjar.sh ARGS ${CMAKE_BINARY_DIR}/main.jar ${CMAKE_BINARY_DIR}/main.jar.bin
)
add_custom_target(main ALL DEPENDS main.jar.bin)

#
# Tests currently run only on Linux
#
if (${TARGET} STREQUAL LINUX)
    add_test(Sanity ${CMAKE_BINARY_DIR}/pjvm -cp ${CMAKE_BINARY_DIR}/main.jar Main)
endif()  

#
# Build the distribution tree
#
install(FILES ${CMAKE_BINARY_DIR}/pjvm.uf2 
        DESTINATION .
        RENAME pjvm-${PROJECT_VERSION}.uf2
)
install(PROGRAMS ${CMAKE_SOURCE_DIR}/tools/wrapjar.sh ${CMAKE_SOURCE_DIR}/tools/preverify/bin/preverify
        DESTINATION bin
)
install(DIRECTORY ${CMAKE_BINARY_DIR}/doc
        DESTINATION .
)
install(FILES ${CMAKE_BINARY_DIR}/classes.jar
        DESTINATION lib
)
set(CPACK_PACKAGE_FILE_NAME pjvm-${PROJECT_VERSION})
if (${TARGET} STREQUAL PICO)
    include(CPack)
endif()
