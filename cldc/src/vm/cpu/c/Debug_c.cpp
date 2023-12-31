/*
 *   
 *
 * Copyright  1990-2007 Sun Microsystems, Inc. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 only, as published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included at /legal/license.txt).
 * 
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 * 
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa
 * Clara, CA 95054 or visit www.sun.com if you need additional
 * information or have any questions.
 */

#include "jvmconfig.h"

#include "BuildFlags.hpp"
#include "GlobalDefinitions.hpp"
#include "Globals.hpp"

#include "Thread.hpp"

#if !defined(PRODUCT) || ENABLE_TTY_TRACE

extern "C" bool update_java_pointers();
extern "C" void revert_java_pointers();

void ps() {
  Thread* current = Thread::current();
  if (current == NULL) {
    tty->print_cr("cannot print stack (no current thread)");
  } else if (current->last_java_frame_exists()) {
    current->trace_stack(tty);
  } else if (update_java_pointers()) {
    current->trace_stack(tty);
    revert_java_pointers();
  } else {
    tty->print_cr("cannot print stack (inconsistent state of the topmost frame)");
  }
}

#endif
