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

#ifndef _METAOBJTYPEDESC_HPP_
#define _METAOBJTYPEDESC_HPP_

class MetaObjTypeDesc: public MixedOopDesc { 
 private:
  static size_t allocation_size() { 
    return align_allocation_size(sizeof(MetaObjTypeDesc));
  }
  static int pointer_count() {
    return 1;
  }

 private:
  // pointers
  OopDesc *_objref;

  /* All oops must go before here.  If you change the number of oops, 
   * be sure to change pointer_count()
   */
  // non-pointers
  int _type;
  int _near_type;

  friend class MetaObjType;
};

#endif /* _METAOBJTYPEDESC_HPP_ */
