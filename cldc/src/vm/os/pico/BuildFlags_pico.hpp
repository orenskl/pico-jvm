/*
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (C) Oren Sokoler (https://github.com/orenskl)
 *
 */

/*
 * BuildFlags_pico.hpp: compile-time
 * configuration options for the RPi Pico platform.
 */

// Enable the following flag if you want to test the UNICODE
// FilePath handling under Generic
// #define USE_UNICODE_FOR_FILENAMES 1

// We don't use BSDSocket.cpp to implement sockets on this platform
#define USE_BSD_SOCKET 0

// The Generic port support TIMER_THREAD but not TIMER_INTERRUPT
#define SUPPORTS_TIMER_THREAD        1
#define SUPPORTS_TIMER_INTERRUPT     1

// The Generic port does not support adjustable memory chunks for
// implementing the Java heap.
#define SUPPORTS_ADJUSTABLE_MEMORY_CHUNK 0

#define SUPPORTS_DIRECTORIES 0

