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

#ifndef _SYMBOL_HPP_
#define _SYMBOL_HPP_

#include "SymbolDesc.hpp"
#include "Oop.hpp"

class Symbol: public Oop {
 private:
  SymbolDesc* symbol() const        { return (SymbolDesc*) obj(); }

 public:
  HANDLE_DEFINITION_CHECK(Symbol, Oop);
  ~Symbol() {}
  static int length_offset() { return FIELD_OFFSET(SymbolDesc, _length); }
  jushort length(void) const { return ushort_field(length_offset()); }

 public:
  // To avoid endless lists of friends the static offset computation
  // routines are all public.
  static int base_offset(void) { return SymbolDesc::header_size(); }

  utf8 utf8_data(void) const {
    return symbol()->utf8_data();
  }

  // The address of the first byte of the string held in this Symbol
  const char* base_address(void) const {
    return ((char*)obj()) + base_offset();
  }

 private:
  bool is_within_bounds(int index) const {
    return (index >= 0 && index < length());
  }
  int offset_from_index(int index) const {
    GUARANTEE(is_within_bounds(index), "Symbol contents index out of bounds");
    return base_offset() + (index * sizeof(jbyte));
  }
  static void put_unicode_char(utf8 &p, jchar ch);

 public:

  jbyte byte_at(int index) const {
    return byte_field(offset_from_index(index));
  }

  void byte_at_put(int index, jbyte value) {
    byte_field_put(offset_from_index(index), value);
  }
  static ReturnOop copy_string_to_byte_array(OopDesc* string, bool slashify JVM_TRAPS);

  // Returns unbounded equality hash value
  juint hash(void) const;

  bool matches(const SymbolDesc* other_symbol) const {
    return symbol()->matches(other_symbol);
  }
  bool matches(const Symbol* other_symbol) const {
    return matches(other_symbol->symbol());
  }

  bool matches_pattern(const SymbolDesc* other_symbol) const {
    return symbol()->matches_pattern(other_symbol);
  }
  bool matches_pattern(const Symbol* other_symbol) const {
    return matches_pattern(other_symbol->symbol());
  }

  int strrchr(jbyte c) const;
  bool is_same_class_package(const Symbol* other) const;

  void string_copy(char* buffer, int buffer_size);

  void print_symbol_on(Stream* st, bool dottified = false);

  bool is_valid_class_name(void);

  bool is_valid_field_name(void);
  bool is_valid_field_type(void);

  bool is_valid_method_name(void);
  bool is_valid_method_signature(const Symbol* name=NULL) const;

#ifndef PRODUCT
  void iterate(OopVisitor* visitor);
#endif
#if !defined(PRODUCT) || ENABLE_TTY_TRACE
  void print_value_on(Stream* st);
#endif

#if ENABLE_ROM_GENERATOR
  void print_as_c_array_on(Stream* st);
  void print_as_c_source_on(Stream* st);
  int generate_fieldmap(TypeArray* field_map);
#endif

#if ENABLE_ROM_GENERATOR
  bool eq(const char str[]) const;
#endif

#if ENABLE_ROM_GENERATOR || ENABLE_PERFORMANCE_COUNTERS
  // 1 if s1 > s2; 0 if the same; -1 if s1 < s2
  static jint compare_to(const Symbol* s1, const Symbol* s2);
#endif

  friend class SymbolTable;
  friend class Universe;
};

#endif /* _SYMBOL_HPP_ */
