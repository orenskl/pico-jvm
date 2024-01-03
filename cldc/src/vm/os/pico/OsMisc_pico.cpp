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

#ifdef __cplusplus
extern "C" {
#endif

const JvmPathChar *OsMisc_get_classpath() 
{
  /* 
   * Returns a system-wide defined classpath. This function is called only
   * if a classpath is not given in parameters to JVM_Start() or JVM_Start2().
   * On Win32 and Linux this function calls getenv("CLASSPATH"). This function
   * is generally not needed in real devices.
   */
  return NULL;
}


void OsMisc_flush_icache(address start, int size) 
{
  /* 
   * flush_icache is used, for example, to flush any caches used by a
   * code segment that is deoptimized or moved during a garbage
   * collection.
   */
}

#if !defined(PRODUCT) || ENABLE_TTY_TRACE || USE_DEBUG_PRINTING
const char *OsMisc_jlong_format_specifier() 
{
  /* 
   * Return jlong-specifier prefixes are used with type characters in
   * printf functions or wprintf functions to specify interpretation
   * of jlong e.g. for win32 is "%I64d", for linux is "%lld"
   */
  return "%lld";
}

const char *OsMisc_julong_format_specifier() 
{
  /* 
   * Return julong-specifier prefixes are used with type characters in
   * printf functions or wprintf functions to specify interpretation
   * of julong e.g. for win32 is "%I64u", for linux is "%llu"
   */
  return "%llu";
}
#endif // PRODUCT

#if ENABLE_PAGE_PROTECTION
void OsMisc_page_protect() 
{
  UNIMPLEMENTED();
}

void OsMisc_page_unprotect() 
{
  UNIMPLEMENTED();
}
#endif // ENABLE_PAGE_PROTECTION

#ifdef __cplusplus
}
#endif
