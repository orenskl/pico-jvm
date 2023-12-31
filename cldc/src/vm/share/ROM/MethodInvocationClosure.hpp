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

#ifndef _METHODINVOCATIONCLOSURE_HPP_
#define _METHODINVOCATIONCLOSURE_HPP_

#if ENABLE_ROM_GENERATOR

class MethodInvocationClosure {
public:
  void initialize(JVM_SINGLE_ARG_TRAPS);
  
  bool add_method           (Method* method);
  void add_interface_method (Method* method);
  void add_virtual_method   (Method* method);

  bool contains(const Method* method) const {
    const juint len = juint(_methods.length());
    const juint start = juint(hashcode_for_method(method)) % len;

    for (juint i=start; ;) {
      const Method::Raw m = _methods.obj_at(i);
      if (m.is_null()) {
        break;
      }
      if (m.equals(method)) {
        return true;
      }

      if (++i >= len) {
         i = 0;
      }
      GUARANTEE(i != start, "Sanity");
      // _old_methods's length is 3 times the number of methods,
      // so we will always have space.
    }

    return false;
  }

private:
  static int hashcode_for_method(const Method* method);
  static int hashcode_for_symbol(const Symbol* symbol);
  
  ObjArray _methods;
};

#endif

#endif /* _METHODINVOCATIONCLOSURE_HPP_ */
