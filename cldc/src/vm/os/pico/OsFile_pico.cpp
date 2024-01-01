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
#include "Stream.hpp"

#define       PICO_JAR_START        0x10100000
#define       PICO_JAR_SIGNATURE    0xCAFE1973

typedef struct __attribute__((packed)) {
  uint32_t Signature;
  uint32_t Length;
} FileHeader;

extern "C" {

struct OsFile {
    int index;                    // index in fs_image_table
    const unsigned char * data;   // start of data
    int length;                   // length of data
    int pos;                      // current position
};

static OsFile file_handles[10];

OsFile_Handle OsFile_open(const PathChar *fn_filename, const char *mode) 
{
  FileHeader * ptr;
  uint32_t     signature;
  uint32_t     length;


  ptr = (FileHeader *)PICO_JAR_START;
  signature = __bswap32(ptr->Signature);
  length    = __bswap32(ptr->Length);
  if (signature != PICO_JAR_SIGNATURE) {
      TTY_TRACE_CR(("Bad wrapped JAR file signature, expected %08X, got %08X", PICO_JAR_SIGNATURE, signature));
      return NULL;
  }

  for (int j=0; j<ARRAY_SIZE(file_handles); j++) {
    if (file_handles[j].data == NULL) {
      file_handles[j].data   = (uint8_t *)PICO_JAR_START + sizeof(FileHeader);
      file_handles[j].length = length;
      file_handles[j].pos    = 0;
      return &file_handles[j];
    }
  }

  return NULL;
}

int OsFile_close(OsFile_Handle handle) 
{
  handle->data = NULL;
  return 0;
}

int OsFile_flush(OsFile_Handle handle) 
{
  // nothing to do
  return 0;
}

size_t OsFile_read(OsFile_Handle handle,void *buffer, size_t size, size_t count) {
  size_t done = 0;
  size_t req = size * count;
  char * p = (char*)buffer;

  while ((handle->pos < handle->length) && (done < req)) {
     *p = handle->data[handle->pos];
     p++;
     handle->pos ++;
     done ++;
  }

  return done;
}

size_t OsFile_write(OsFile_Handle handle, const void *buffer, size_t size, size_t count) 
{
  UNIMPLEMENTED();
  return 0;
}

long OsFile_length(OsFile_Handle handle) 
{
  return handle->length;
}

bool OsFile_exists(const PathChar *fn_filename) 
{
  FileHeader * ptr;
  uint32_t     signature;
  uint32_t     length;


  ptr = (FileHeader *)PICO_JAR_START;
  signature = __bswap32(ptr->Signature);
  length    = __bswap32(ptr->Length);
  if (signature != PICO_JAR_SIGNATURE) {
      TTY_TRACE_CR(("Bad wrapped JAR file signature, expected %08X, got %08X", PICO_JAR_SIGNATURE, signature));
      return false;
  }
  return true;
}

long OsFile_seek(OsFile_Handle handle, long offset, int origin) 
{
  switch (origin) {
  case SEEK_CUR:
    handle->pos += offset;
    break;
  case SEEK_SET:
    handle->pos = offset;
    break;
  case SEEK_END:
    handle->pos = handle->length + offset;
    break;
  default:
    return -1;
  }

  if (handle->pos < 0) {
    handle->pos = 0;
  } else if (handle->pos > handle->length) {
    handle->pos = handle->length;
  }

  return 0;
}

int OsFile_error(OsFile_Handle handle) 
{
  return 0;
}

int OsFile_eof(OsFile_Handle handle) 
{
  return (handle->pos >= handle->length);
}

bool OsFile_rename(const char *from, const char *to) 
{
  UNIMPLEMENTED();
  return 0;
}

int OsFile_remove(const char *filename) 
{
  UNIMPLEMENTED();
  return 0;
}

} // extern "C"
