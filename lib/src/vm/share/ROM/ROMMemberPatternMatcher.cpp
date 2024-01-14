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

#include "ROMMemberPatternMatcher.hpp"
#include "TypeSymbol.hpp"
#include "SymbolTable.hpp"

#if ENABLE_ROM_GENERATOR

bool ROMMemberPatternMatcher::initialize(const char* pattern, const int /*len*/
                                         JVM_TRAPS) {
  if (pattern[0] == '*' || pattern[1] == 0) {
    pattern = "*.*";
  }

  const char* delimiter = (const char*) jvm_strrchr(pattern, '.');
  if (!delimiter) {
    invalid_pattern();
    return false;
  }

  int pos = delimiter - pattern;
  const bool ok = 
    ROMClassPatternMatcher::initialize(pattern, pos JVM_NO_CHECK_AT_BOTTOM);
  if (!ok) {
    return false;
  }
  
  // parse the method name after '.'
  pattern += pos + 1;
  if ((delimiter = (const char*)jvm_strchr(pattern, '(')) == NULL) {
    pos = jvm_strlen(pattern);
  } else {
    // delimiter points to the start of method's signature
    pos = delimiter - pattern;
    _signature = TypeSymbol::parse(pattern + pos JVM_CHECK_0);
  }

  if (!ROMClassPatternMatcher::validate(pattern, pos)) {
    return false;
  }

  _name = SymbolTable::symbol_for((utf8) pattern, pos JVM_CHECK_0);
  return true;
}

#endif // ENABLE_ROM_GENERATOR
