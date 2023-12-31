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

#ifndef _CONSTANTPOOLDESC_HPP_
#define _CONSTANTPOOLDESC_HPP_

#include "OopDesc.hpp"

class ConstantPoolDesc: public OopDesc { 

 private:
  static jint header_size() { return sizeof(ConstantPoolDesc); }

  // Computes the allocation size
  static size_t allocation_size(int length) { 
    return align_allocation_size(header_size() + length * sizeof(OopDesc*));
  }

 public:
  // Returns the object size
  size_t object_size() { return allocation_size(_length); }
  // GC support.
  void variable_oops_do(void do_oop(OopDesc**));

 private:
  // Initializes the object after allocation
  void initialize(OopDesc* klass, jint length) {
    OopDesc::initialize(klass);
    _length = (jushort)length; 
  }

  jushort            _length;
  jushort            _pad;         // Force 4-byte alignment of _tags.
  TypeArrayDesc*     _tags;        // describes the constant pool's contents
  friend class ConstantPool;
  friend class Universe;
  friend class MethodDesc;
};

#endif /* _CONSTANTPOOLDESC_HPP_ */
