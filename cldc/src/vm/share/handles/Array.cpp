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
#include "GlobalDefinitions_c.hpp"
#include "GlobalDefinitions_gcc.hpp"
#include "Globals.hpp"

#include "Array.hpp"
#include "Near.hpp"
#include "TypeArray.hpp"
#include "TypeArrayClass.hpp"
#include "ObjArrayClass.hpp"

#if USE_REFLECTION
ReturnOop Array::shrink(int new_length) {
  GUARANTEE(new_length >= 0, "Negative array size prohibited");
  int scale;
  if (is_type_array()) {
    TypeArrayClass::Raw array_class = blueprint();
    scale = array_class().scale();
  } else {
    GUARANTEE(is_obj_array(), "sanity");
    scale = sizeof(OopDesc*);
  }

  Universe::shrink_object(this, ArrayDesc::allocation_size(new_length, scale));
  set_length(new_length);
  return obj();
}
#endif

#if ENABLE_ROM_GENERATOR
// generate a map of all the field types in this object
int Array::generate_fieldmap(TypeArray* field_map) {
  int map_index = Near::generate_fieldmap(field_map);

  //_length
  field_map->byte_at_put(map_index++, T_INT);

  // map size for the elements
  const jint elements_map_size = map_index + length();
  if (elements_map_size > field_map->length()) {    
    return elements_map_size; // need more space
  }

  int type = T_OBJECT;
  if (!is_obj_array()) {
    TypeArrayClass::Raw cls = blueprint();
    type = cls().type();
  }

  for (; map_index < elements_map_size; map_index++) {
    field_map->byte_at_put(map_index, type);
  }

  return map_index;
}
#endif /* #if ENABLE_ROM_GENERATOR */
