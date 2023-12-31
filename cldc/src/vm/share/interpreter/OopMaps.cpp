/*
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

/* This file is auto-generated. Do not edit */

extern "C" {
extern const unsigned char oopmap_Empty[];
const unsigned char oopmap_Empty[] = { 0 };
extern const unsigned char oopmap_ConstantPool[];
const unsigned char oopmap_ConstantPool[] = { 2, 0 };
extern const unsigned char oopmap_Method[];
const unsigned char oopmap_Method[] = { 1, 1, 1, 0 };
extern const unsigned char oopmap_InstanceClass[];
const unsigned char oopmap_InstanceClass[] = { 3, 1, 1, 1, 1, 1, 1, 1, 0 };
extern const unsigned char oopmap_ArrayClass[];
const unsigned char oopmap_ArrayClass[] = { 3, 1, 1, 1, 1, 1, 1, 1, 0 };
extern const unsigned char oopmap_ObjNear[];
const unsigned char oopmap_ObjNear[] = { 1, 0 };
extern const unsigned char oopmap_FarClass[];
const unsigned char oopmap_FarClass[] = { 3, 0 };
extern const unsigned char oopmap_EntryActivation[];
const unsigned char oopmap_EntryActivation[] = { 2, 1, 0 };
extern const unsigned char oopmap_ClassInfo[];
const unsigned char oopmap_ClassInfo[] = { 3, 0 };
extern const unsigned char oopmap_StackmapList[];
const unsigned char oopmap_StackmapList[] = { 0 };
extern const unsigned char oopmap_CompiledMethod[];
const unsigned char oopmap_CompiledMethod[] = { 2, 0 };
}

#ifndef PRODUCT

#ifdef __cplusplus
extern "C" {
#endif
extern void jvm_check_oopmap(const char *generator_name, const char* class_name, void *data);
static const char * loopgen_ConstantPool_oopmap_data[] = {
	(const char*)   4, /*size=4*/ "length",
	(const char*)   8, /*size=4*/ "tags",
	(const char*)   0, (const char*)0
};
static const char * loopgen_Method_oopmap_data[] = {
	(const char*)   4, /*size=4*/ "constants",
	(const char*)   8, /*size=4*/ "exception_table",
	(const char*)  12, /*size=4*/ "stackmaps",
	(const char*)  16, /*size=4*/ "heap_execution_entry",
	(const char*)  20, /*size=4*/ "variable_part",
	(const char*)  24, /*size=2*/ "access_flags",
	(const char*)  26, /*size=2*/ "holder_id",
	(const char*)  28, /*size=2*/ "max_execution_stack_count",
	(const char*)  30, /*size=2*/ "max_locals",
	(const char*)  32, /*size=2*/ "method_attributes",
	(const char*)  34, /*size=2*/ "name_index",
	(const char*)  36, /*size=2*/ "signature_index",
	(const char*)  38, /*size=2*/ "code_size",
	(const char*)   0, (const char*)0
};
static const char * loopgen_InstanceClass_oopmap_data[] = {
	(const char*)   4, /*size=2*/ "object_size",
	(const char*)   6, /*size=2*/ "instance_size",
	(const char*)   8, /*size=4*/ "oop_map",
	(const char*)  12, /*size=4*/ "prototypical_near",
	(const char*)  16, /*size=4*/ "class_info",
	(const char*)  20, /*size=4*/ "subtype_cache_1",
	(const char*)  24, /*size=4*/ "subtype_cache_2",
	(const char*)  28, /*size=4*/ "array_class",
	(const char*)  32, /*size=4*/ "java_mirror",
	(const char*)  36, /*size=4*/ "super",
	(const char*)  40, /*size=4*/ "next",
	(const char*)   0, (const char*)0
};
static const char * loopgen_ArrayClass_oopmap_data[] = {
	(const char*)   4, /*size=2*/ "object_size",
	(const char*)   6, /*size=2*/ "instance_size",
	(const char*)   8, /*size=4*/ "oop_map",
	(const char*)  12, /*size=4*/ "prototypical_near",
	(const char*)  16, /*size=4*/ "class_info",
	(const char*)  20, /*size=4*/ "subtype_cache_1",
	(const char*)  24, /*size=4*/ "subtype_cache_2",
	(const char*)  28, /*size=4*/ "array_class",
	(const char*)  32, /*size=4*/ "java_mirror",
	(const char*)  36, /*size=4*/ "super",
	(const char*)  40, /*size=4*/ "element_class",
	(const char*)   0, (const char*)0
};
static const char * loopgen_ObjNear_oopmap_data[] = {
	(const char*)   4, /*size=4*/ "object",
	(const char*)   0, (const char*)0
};
static const char * loopgen_FarClass_oopmap_data[] = {
	(const char*)   4, /*size=2*/ "object_size",
	(const char*)   6, /*size=2*/ "instance_size",
	(const char*)   8, /*size=4*/ "oop_map",
	(const char*)  12, /*size=4*/ "prototypical_near",
	(const char*)   0, (const char*)0
};
static const char * loopgen_EntryActivation_oopmap_data[] = {
	(const char*)   4, /*size=4*/ "length",
	(const char*)   8, /*size=4*/ "method",
	(const char*)  12, /*size=4*/ "next",
	(const char*)   0, (const char*)0
};
static const char * loopgen_ClassInfo_oopmap_data[] = {
	(const char*)   4, /*size=2*/ "object_size",
	(const char*)   6, /*size=2*/ "vtable_length",
	(const char*)   8, /*size=2*/ "itable_length",
	(const char*)  10, /*size=2*/ "class_id",
	(const char*)  12, /*size=4*/ "name",
	(const char*)  16, /*size=4*/ "access_flags",
	(const char*)  20, /*size=4*/ "methods",
	(const char*)  24, /*size=4*/ "fields",
	(const char*)  28, /*size=4*/ "local_interfaces",
	(const char*)  32, /*size=4*/ "constants",
	(const char*)   0, (const char*)0
};
static const char * loopgen_StackmapList_oopmap_data[] = {
	(const char*)   4, /*size=4*/ "entry_count",
	(const char*)   0, (const char*)0
};
static const char * loopgen_CompiledMethod_oopmap_data[] = {
	(const char*)   4, /*size=4*/ "flags_and_size",
	(const char*)   8, /*size=4*/ "method",
	(const char*)   0, (const char*)0
};
void loopgen_check_oopmaps() {
  jvm_check_oopmap("loopgen", "ConstantPool", &loopgen_ConstantPool_oopmap_data);
  jvm_check_oopmap("loopgen", "Method", &loopgen_Method_oopmap_data);
  jvm_check_oopmap("loopgen", "InstanceClass", &loopgen_InstanceClass_oopmap_data);
  jvm_check_oopmap("loopgen", "ArrayClass", &loopgen_ArrayClass_oopmap_data);
  jvm_check_oopmap("loopgen", "ObjNear", &loopgen_ObjNear_oopmap_data);
  jvm_check_oopmap("loopgen", "FarClass", &loopgen_FarClass_oopmap_data);
  jvm_check_oopmap("loopgen", "EntryActivation", &loopgen_EntryActivation_oopmap_data);
  jvm_check_oopmap("loopgen", "ClassInfo", &loopgen_ClassInfo_oopmap_data);
  jvm_check_oopmap("loopgen", "StackmapList", &loopgen_StackmapList_oopmap_data);
  jvm_check_oopmap("loopgen", "CompiledMethod", &loopgen_CompiledMethod_oopmap_data);
}
#ifdef __cplusplus
}
#endif
#endif /* PRODUCT*/
