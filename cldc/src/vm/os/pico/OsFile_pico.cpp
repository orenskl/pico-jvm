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

#include "OsFile_pico.hpp"
#include "Debug.hpp"

extern "C" {

OsFile_Handle OsFile_open(const PathChar *fn_filename, const char *mode) {
  return NULL;
}

int OsFile_close(OsFile_Handle handle) {
  return 0;
}

int OsFile_flush(OsFile_Handle handle) {
  // nothing to do
  return 0;
}

size_t OsFile_read(OsFile_Handle handle,
                   void *buffer, size_t size, size_t count) {
  return 0;
}

size_t OsFile_write(OsFile_Handle handle,
                    const void *buffer, size_t size, size_t count) {
  UNIMPLEMENTED();
  return 0;
}

long OsFile_length(OsFile_Handle handle) {
  return 0;
}

bool OsFile_exists(const PathChar *fn_filename) {
  return false;
}

long OsFile_seek(OsFile_Handle handle, long offset, int origin) {
  return 0;
}

int OsFile_error(OsFile_Handle handle) {
  return 0;
}

int OsFile_eof(OsFile_Handle handle) {
  return 0;
}

bool OsFile_rename(const char *from, const char *to) {
  UNIMPLEMENTED();
  return 0;
}

int OsFile_remove(const char *filename) {
  UNIMPLEMENTED();
  return 0;
}

} // extern "C"
