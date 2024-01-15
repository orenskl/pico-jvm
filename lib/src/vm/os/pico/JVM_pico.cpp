/*
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (C) Oren Sokoler (https://github.com/orenskl)
 *
 */

/*
 * JVM_pico.cpp: RPi Pico specific VM startup and
 *               shutdown routines.
 *
 * This file provides RPi Pico specific virtual machine
 * startup and shutdown routines.  Refer to file
 * "/src/vm/share/runtime/JVM.hpp" and the Porting
 * Guide for details.
 */

#include "jvmconfig.h"

#include "BuildFlags.hpp"
#include "GlobalDefinitions.hpp"
#include "Globals.hpp"

#include "JVM.hpp"
#include "Stream.hpp"
#include "Arguments.hpp"

static int executeVM( void ) 
{
  const int result = JVM::start();
  Arguments::finalize();
  return result;
}

extern "C" int JVM_Start(const char *classpath, char *main_class, int argc, char **argv) 
{
  JVM::set_arguments(classpath, main_class, argc, argv);
  return executeVM();
}

extern "C" int JVM_Start2(const char *classpath, char *main_class, int argc, jchar **u_argv) 
{
  JVM::set_arguments2(classpath, main_class, argc, NULL, u_argv, true);
  return executeVM();
}
