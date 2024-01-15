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

#ifndef _SYMBOLDESC_HPP_
#define _SYMBOLDESC_HPP_

#include "OopDesc.hpp"

class SymbolDesc: public OopDesc {
 private:
  // NOTE: cannot use sizeof(SymbolDesc) since compiler aligns the size
  static jint header_size( void ) {
    return FIELD_OFFSET(SymbolDesc, _length) + sizeof(jushort);
  }

 public:
  utf8 utf8_data( void ) const {
    return (utf8) this + header_size();
  }
  utf8 utf8_data( void ) {
    return (utf8) this + header_size();
  }

 private:
  // Returns the size of a symbol with the given length.
  static size_t allocation_size(const int length) { 
    GUARANTEE(length >= 0, "Cannot allocate symbol of negative length");
    return align_allocation_size(header_size() + length * sizeof(jbyte));
  }

 public:
  // Returns the object size
  size_t object_size( void ) const { return allocation_size(_length); }

  juint utf8_length( void ) const { 
    return _length;
  };

  bool matches(utf8 s, const int len) const {
    return len == _length &&
           jvm_memcmp(s, utf8_data(), len) == 0;
  }
  bool matches(const SymbolDesc* other_symbol) const {
    return matches(other_symbol->utf8_data(), other_symbol->utf8_length());
  }
  bool matches_pattern(const SymbolDesc* other_symbol) const;

 private:
  // Initializes the object after allocation
  void initialize(OopDesc* klass, int length) {
    OopDesc::initialize(klass);
    GUARANTEE(0 <= length && length < (1 << 16), "invalid symbol size");
    _length = (jushort)length;
  }

  jushort _length;
  
  friend class Symbol;
  friend class SymbolTable;
  friend class Universe;
  friend class ROM;
};

#endif /* _SYMBOLDESC_HPP_ */
