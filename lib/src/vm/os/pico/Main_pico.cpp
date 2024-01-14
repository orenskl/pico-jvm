/*
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (C) Oren Sokoler (https://github.com/orenskl)
 *
 */

#include "jvmconfig.h"

#include "BuildFlags.hpp"
#include "GlobalDefinitions.hpp"
#include "Globals.hpp"

#include "Debug.hpp"
#include "jvm.h"
#include "jvmspi.h"
#include "sni.h"

#include "pico/stdlib.h"

#include <stdio.h>
#include <stdlib.h>

/*
 * This file implements the launching of the stand-along VM for RPi Pico
 */

void JVMSPI_PrintRaw(const char* s, int length) 
{
  jvm_fwrite(s, length, 1, stdout);
  jvm_fflush(stdout);
}

void JVMSPI_Exit(int code) 
{
  ::jvm_exit(code);
}

void JVMSPI_DisplayUsage(char* message) {
}

char *JVMSPI_GetSystemProperty(const char *property_name) {
  return NULL;
}

int JVMSPI_HandleOutOfMemory(const int isolate_id,
                             const int limit,
                             const int reserve,
                             const int available,
                             const int alloc_size,
                             const int flags,
                             int * exit_code) 
{
  GUARANTEE(flags & JVMSPI_IGNORE, "JVMSPI_IGNORE must be supprted");
  return JVMSPI_IGNORE;
}

int JVMSPI_HandleUncaughtException(const int isolate_id,
                                   const char * exception_class_name,
                                   const int exception_class_name_length,
                                   const char * message,
                                   const int flags,
                                   int * exit_code) 
{
  GUARANTEE(flags & JVMSPI_IGNORE, "JVMSPI_IGNORE must be supprted");
  return JVMSPI_IGNORE;
}

void JVMSPI_FreeSystemProperty(const char * /*prop_value*/) 
{
  // do nothing
}

jboolean JVMSPI_CheckExit(void) 
{
  return KNI_TRUE;
}

int main( void ) 
{
  int code = 0;

  /* Initialize our stdio */
  stdio_usb_init();
  /* We have to wait for the USB stack */
  sleep_ms(3000);

  // Call this before any other Jvm_ functions.
  JVM_Initialize();

  JVM_SetConfig(JVM_CONFIG_HEAP_CAPACITY,64*1024);

  if (JVM_GetConfig(JVM_CONFIG_SLAVE_MODE) == KNI_FALSE) {
    // Run the VM in regular mode -- JVM_Start won't return until
    // the VM completes execution.
    code = JVM_Start((char *)"classes.jar:main.jar", (char *)"Main", 0, NULL);
  } else {
    JVM_Start((char *)"classes.jar:main.jar", (char *)"Main", 0, NULL);

    for (;;) {
      long timeout = JVM_TimeSlice();
      if (timeout <= -2) {
        break;
      } else {
        int blocked_threads_count;
        JVMSPI_BlockedThreadInfo * blocked_threads;

        blocked_threads = SNI_GetBlockedThreads(&blocked_threads_count);
        JVMSPI_CheckEvents(blocked_threads, blocked_threads_count, timeout);
      }
    }

    code = JVM_CleanUp();
  }

end:
  return code;
}

/**
 * @brief This gets called by exit(), we override the default one.
 * @param status Exit status
 */
void _exit(int status)
{
    /* If we got here - just loop (nothing to do) */
    for (;;) {
        sleep_ms(3000);
    }
}
