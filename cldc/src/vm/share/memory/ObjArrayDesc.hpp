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

#ifndef _OBJARRAYDESC_HPP_
#define _OBJARRAYDESC_HPP_

#include "ArrayDesc.hpp"

class ObjArrayDesc: public ArrayDesc {
 public:
  // GC support.
  void variable_oops_do(void do_oop(OopDesc**)) {
    OopDesc** base = (OopDesc**)((address)this + ArrayDesc::header_size());
    OopDesc** end  = base + _length;
    while (base < end) {
      // Inline null check, fairly common case in object arrays
      if (*base != NULL) {
        do_oop(base);
      }
      base++;
    }
  }

  friend class ObjectHeap;
  friend class JavaDebugger;
};

#endif /* _OBJARRAYDESC_HPP_ */
