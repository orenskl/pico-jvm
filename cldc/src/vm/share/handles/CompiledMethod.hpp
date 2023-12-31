/*
 *   
 *
 * Portions Copyright  2000-2007 Sun Microsystems, Inc. All Rights
 * Reserved.  Use is subject to license terms.
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
 *
 *!c<
 * Copyright 2006 Intel Corporation. All rights reserved.
 *!c>
 */

// A CompiledMethod contains the generated native code generated by
// the compiler. It contains two parts
// - the generated instructions (located at the start of the dynamic part)
// - relocation information     (located at the end of the dynamic part)
// The relocation information is PIC making it easy to truncate the object
// after completed code generation.

#ifndef _COMPILEDMETHOD_HPP_
#define _COMPILEDMETHOD_HPP_

#include "CompiledMethodDesc.hpp"
#include "ObjectHeap.hpp"
#include "Universe.hpp"
#include "Oop.hpp"

class CompiledMethod : public Oop {
 public:
  // Offset where compiled code begins
  static int base_offset() {
   return CompiledMethodDesc::header_size();
  }
  static int entry_offset() {
   return CompiledMethodDesc::entry_offset();
  }
  // To avoid endless lists of friends the static offset computation
  // routines are all public.
  static int method_offset() {
    return FIELD_OFFSET(CompiledMethodDesc, _method);
  }
  static int flags_and_size_offset() {
    return FIELD_OFFSET(CompiledMethodDesc, _flags_and_size);
  }

#if ENABLE_JVMPI_PROFILE
  static int jvmpi_code_size_offset() {
    return FIELD_OFFSET(CompiledMethodDesc, _jvmpi_code_size);
  }
#endif


  HANDLE_DEFINITION_CHECK(CompiledMethod, Oop);
 
  // Returns the method causing this code
  // ^ Method
  ReturnOop method() const {
    return obj_field(method_offset());
  }
  void set_method(OopDesc* value) {
    obj_field_put(method_offset(), value);
  }
  void set_method(Oop* value) {
    set_method( value->obj() );
  }

  // Returns the size of the dynamic part
  jint size() const {
    return ((CompiledMethodDesc*)obj())->code_size();
  }

private:
  juint flags_and_size() {
    return uint_field(flags_and_size_offset());
  }
  void set_flags_and_size(const juint value) {
    uint_field_put(flags_and_size_offset(), value);
  }

public:
  void set_has_branch_relocation() {
    juint fs = flags_and_size();
    fs |= CompiledMethodDesc::HAS_BRANCH_RELOCATION_MASK;
    set_flags_and_size(fs);
  }

#if ENABLE_JVMPI_PROFILE
  // Return the compiled code size    
  jint jvmpi_code_size() const {
    return ((CompiledMethodDesc*)obj())->jvmpi_get_code_size();
  }
#endif

  // Returns the end_offset for the object (used by RelocationStream)
  jint end_offset() const {
    return base_offset() + size();
  }

  // Return the address of the entry.
  address entry() const {
    return ((CompiledMethodDesc*)obj())->entry();
  }

  // Shrinks the compiled method to remove wasted padding (used at end
  // of code generation)
  void shrink(jint code_size, jint relocation_size);

  // Expand the compiled methid in-place (by moving the relocation data 
  // to higher address). This works only when this method is being
  // compiled, and compiler_area is enabled.
  // Returns true if successful; false if there's insufficient space
  // in the compiler_area to accomodate the request, in which case
  // the size of this compiled method is not changed.
  bool expand_compiled_code_space(jint delta, jint relocation_size);

#if ENABLE_APPENDED_CALLINFO
  inline bool expand_callinfo_table(const int delta) {
    GUARANTEE(align_allocation_size(delta) == (size_t)delta, "must be aligned");
    if( !ObjectHeap::expand_current_compiled_method(delta) ) {
      return false;
    }
    ((CompiledMethodDesc*) obj())->set_size(size() + delta);
    return true;
  }

  inline void shrink_callinfo_table(const int delta) {
    // Shrink compiled method object
    const size_t new_size = size() - delta;
    const size_t new_allocation_size = CompiledMethodDesc::allocation_size(new_size);
    Universe::shrink_object(this, new_allocation_size);
    ((CompiledMethodDesc*) obj())->set_size(new_size);
    GUARANTEE(object_size() == new_allocation_size, "invalid shrunk size");
  }
#endif // ENABLE_APPENDED_CALLINFO

  // Flushes the icache portion corresponding to the generated code
  void flush_icache();

#if ENABLE_ROM_GENERATOR
  int generate_fieldmap(TypeArray* field_map);
#endif

#if !defined(PRODUCT) || ENABLE_TTY_TRACE || USE_DEBUG_PRINTING
  void print_code_on(Stream* st);
  void print_code_on(Stream* st, jint start, jint end);  // x86
  void print_comment_for(int code_offset, Stream* st);
  void print_relocation_on(Stream* st);
  void iterate(OopVisitor* visitor);
  void print_value_on(Stream* st);
  void print_name_on(Stream* st);
  static void iterate_oopmaps(oopmaps_doer do_map, void* param);
#endif
};

#endif /* _COMPILEDMETHOD_HPP_ */
