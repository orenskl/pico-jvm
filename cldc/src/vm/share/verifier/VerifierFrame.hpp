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

/**
   \class VerifierFrame
   Defines the stack operations performed on the CLDC Verifier
   stack and the stackmap.
   The stackmap attribute is generated by the preverifier
   and used by the runtime verifier during bytecode verification.
 */ 

#ifndef _VERIFIERFRAME_HPP_
#define _VERIFIERFRAME_HPP_

#include "StackMapDefinitions.hpp"
#include "TypeArray.hpp"
#include "ObjArray.hpp"
#include "Symbol.hpp"

class VerifierFrame: public StackObj {
public:
  VerifierFrame() : _vstackpointer(0) { }
  ~VerifierFrame() {
    // The existence of this destructor helps GCC 2.9 generate
    // smaller code
  }

  void initialize(Method *method, ObjArray *stackmaps JVM_TRAPS);
  void pop_ref(StackMapKind& kind, Symbol* name JVM_TRAPS);
  void pop_ref(JVM_SINGLE_ARG_TRAPS);          // We don't care what it is.
  void pop_object(Symbol* name JVM_TRAPS);
  void pop(StackMapKind kind JVM_TRAPS);

  void pop_category1(StackMapKind& a,                  Symbol* c JVM_TRAPS);
  void pop_category2(StackMapKind& a, StackMapKind& b, Symbol* c, Symbol* d
                     JVM_TRAPS);

  inline void push_object(Symbol *name JVM_TRAPS) {
    push_category1(ITEM_Object, name JVM_NO_CHECK_AT_BOTTOM);
  }

  inline void push(StackMapKind vkind JVM_TRAPS) {
    push_category1(vkind JVM_NO_CHECK_AT_BOTTOM);
  }

  void push_category1(StackMapKind a                             JVM_TRAPS);
  void push_category1(StackMapKind a,                  Symbol* c JVM_TRAPS);
  void push_category2(StackMapKind a, StackMapKind  b, Symbol* c, Symbol* d
                      JVM_TRAPS);

  static void verifier_error(ErrorMsgTag err JVM_TRAPS);
  static void verifier_error(JVM_SINGLE_ARG_TRAPS);
  void save_stack_state();
  void restore_stack_state();

  int pop_invoke_arguments(Signature* method_signature JVM_TRAPS);

  void set_local(int index, StackMapKind kind JVM_TRAPS);
  void set_local2(int index, StackMapKind kind JVM_TRAPS);
  void set_local_ref(int index, StackMapKind kind, Symbol *name JVM_TRAPS);

  void check_local(int index,  StackMapKind type JVM_TRAPS);
  void check_local2(int index, StackMapKind type JVM_TRAPS);
  void check_local_ref(int index, StackMapKind& type, Symbol* name JVM_TRAPS);

  void replace_stack_type_with_real_type(StackMapKind from, StackMapKind to,
                                         Symbol *to_name);

  // Flags used to control how to match two stack maps.
  // One of the stack maps is derived as part of the type checking process.
  // The other stack map is recorded in the class file.
  //
  enum {
      SM_CHECK = 1,
      SM_MERGE = 2,
      SM_EXIST = 4
  };

  void check_current_target(Method *method, int current_bci,
                            bool noControlFlow JVM_TRAPS) {
    check_stackmap_match(method, current_bci,
                           SM_MERGE | (noControlFlow ? SM_EXIST : SM_CHECK),
                           ve_seq_bad_type JVM_NO_CHECK_AT_BOTTOM);
  }

  void check_handler_target(Method *method, int target_bci JVM_TRAPS) {
    check_stackmap_match(method, target_bci, SM_CHECK | SM_EXIST, 
                         ve_target_bad_type JVM_NO_CHECK_AT_BOTTOM);
  }

  void check_branch_target(Method *method, int target_bci
                           JVM_TRAPS) {
    check_stackmap_match(method, target_bci, SM_CHECK | SM_EXIST, 
                         ve_target_bad_type JVM_NO_CHECK_AT_BOTTOM);
  }

  void check_stackmap_match(Method *method, int target_bci, int flags, 
                            ErrorMsgTag err JVM_TRAPS);

private:
  // get virtual private stack pointer
  int vstackpointer() { return _vstackpointer; }

  // get the stack tag type element
  StackMapKind vstack_tags_at(int index) {
    return (StackMapKind)_vstack_tags().int_at(index);
  }

  // get the class reference corresponding to index from stack
  ReturnOop vstack_classes_at(int index) {
    return _vstack_classes().obj_at(index);
  }

  // get the locals tag type element
  StackMapKind vlocals_tags_at(int index ) {
    return (StackMapKind)_vlocals_tags().int_at(index);
  }

  // get the class reference corresponding to index from locals
  ReturnOop vlocals_classes_at(int index) {
    return _vlocals_classes().obj_at(index);
  }

  // set the stack tag type element
  void vstack_tags_at_put(int index, StackMapKind value) {
    _vstack_tags().int_at_put(index, value);
  }

  // set the class reference corresponding to index on stack
  void vstack_classes_at_put(int index, Symbol* value) {
    GUARANTEE(value->is_null() || value->is_valid_class_name(), "sanity");
    _vstack_classes().obj_at_put(index, value);
  }

  void vstack_classes_at_clear(int index) {
    _vstack_classes().obj_at_clear(index);
  }

  // set the locals tag type element
  void vlocals_tags_at_put(int index, StackMapKind value) {
    _vlocals_tags().int_at_put(index, value);
  }

  // set the class reference corresponding to index in locals bit map
  void vlocals_classes_at_put(int index, Symbol* value) {
    _vlocals_classes().obj_at_put(index, value);
  }

  void vlocals_classes_at_clear(int index) {
    _vlocals_classes().obj_at_clear(index);
  }

  int max_stack()       { return _max_stack; }
  int max_locals()      { return _max_locals; }
  ObjArray* stackmaps() { return _stackmaps;  }

  void bogify_surrounding_doublewords(int index);
  bool compute_is_subtype_of(Symbol*, Symbol* JVM_TRAPS);

#ifndef PRODUCT
  void print_frame_internal(const char *name, int index, StackMapKind, Symbol*);
#endif

public:
  bool is_assignable_to(StackMapKind, StackMapKind, Symbol*, Symbol* JVM_TRAPS);
  void print_frame() PRODUCT_RETURN;
  int get_stackmap_index_for_offset(int target_bci);

  int get_stackmap_entry_offset(int index) {
    address stackmap_scalars = (address)stackmaps()->obj_at(index);
    jint *base = (jint*)(stackmap_scalars + TypeArray::base_offset());
    return base[0];
  }

  bool need_initialization() { return _need_initialization; }
  void set_need_initialization(bool value) { _need_initialization = value; }

private:
  FastOopInStackObj    __must_be_first_item__;
  int              _vstackpointer;
  TypeArray::Fast  _vstack_tags;
  ObjArray::Fast   _vstack_classes;
  TypeArray::Fast  _vlocals_tags;
  ObjArray::Fast   _vlocals_classes;
  bool             _need_initialization;

  ObjArray*        _stackmaps;

  int              _saved_stack_pointer;
  StackMapKind     _saved_tags_0;
  Symbol::Fast     _saved_klass_0;

  int _max_stack;
  int _max_locals;
};

#endif /* _VERIFIERFRAME_HPP_ */
