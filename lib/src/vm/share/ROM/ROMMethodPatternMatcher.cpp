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

#include "ROMMethodPatternMatcher.hpp"
#include "Method.hpp"

#if ENABLE_ROM_GENERATOR

inline bool ROMMethodPatternMatcher::match_method(const Method* method) const {
  return name_matches_pattern((SymbolDesc*)method->name(), &_name) &&
         (_signature.is_null() || _signature.obj() == method->signature());
}

void
ROMMethodPatternMatcher::handle_class(const InstanceClass* klass JVM_TRAPS) {
  UsingFastOops fast_oops;
  ObjArray::Fast methods = klass->methods();
  Method::Fast m;
  const int length = methods().length();
  for (int i = 0; i < length; i++) {
    m = methods().obj_at(i);
    if (m.not_null() && match_method(&m)) {
      set_match_found();
      handle_matching_method(&m JVM_CHECK);
    }
  }
}

void ROMMethodPatternMatcher::handle_matching_method(Method* m JVM_TRAPS) {
  // Do nothing
}

#endif // ENABLE_ROM_GENERATOR
