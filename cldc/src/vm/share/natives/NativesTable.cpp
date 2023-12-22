/* This is a generated file.  Do not modify.
 * Generated on Thu Dec 14 15:25:11 IST 2023
 */

/*
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
#if !defined(ROMIZING) || !defined(PRODUCT) || \
    ENABLE_TTY_TRACE
#include "NativesTable.hpp"
#include "kni.h"


extern "C" jint Java_com_sun_cldc_io_ResourceInputStream_bytesRemain();
extern "C" jobject Java_com_sun_cldc_io_ResourceInputStream_clone();
extern "C" jobject Java_com_sun_cldc_io_ResourceInputStream_open();
extern "C" jint Java_com_sun_cldc_io_ResourceInputStream_readByte();
extern "C" jint Java_com_sun_cldc_io_ResourceInputStream_readBytes();
extern "C" jint Java_com_sun_cldc_io_j2me_socket_Protocol_available0();
extern "C" void Java_com_sun_cldc_io_j2me_socket_Protocol_close0();
extern "C" jint Java_com_sun_cldc_io_j2me_socket_Protocol_open0();
extern "C" jint Java_com_sun_cldc_io_j2me_socket_Protocol_readBuf();
extern "C" jint Java_com_sun_cldc_io_j2me_socket_Protocol_readByte();
extern "C" jint Java_com_sun_cldc_io_j2me_socket_Protocol_writeBuf();
extern "C" jint Java_com_sun_cldc_io_j2me_socket_Protocol_writeByte();
extern "C" void Java_com_sun_cldc_util_SemaphoreLock_acquire();
extern "C" void Java_com_sun_cldc_util_SemaphoreLock_release();
extern "C" void Java_com_sun_cldchi_io_ConsoleOutputStream_write();
extern "C" void Java_com_sun_cldchi_jvm_FileDescriptor_finalize();
extern "C" void Java_com_sun_cldchi_jvm_JVM_cancelImageCreation();
extern "C" jboolean Java_com_sun_cldchi_jvm_JVM_createAppImage0();
extern "C" void Java_com_sun_cldchi_jvm_JVM_createSysImage();
extern "C" void Java_com_sun_cldchi_jvm_JVM_flushJarCaches();
extern "C" jint Java_com_sun_cldchi_jvm_JVM_getAppImageProgress();
extern "C" void Java_com_sun_cldchi_jvm_JVM_loadLibrary();
extern "C" jlong Java_com_sun_cldchi_jvm_JVM_monotonicTimeMillis();
extern "C" void Java_com_sun_cldchi_jvm_JVM_startAppImage();
extern "C" void native_jvm_unchecked_byte_arraycopy_entry();
extern "C" void native_jvm_unchecked_char_arraycopy_entry();
extern "C" void native_jvm_unchecked_int_arraycopy_entry();
extern "C" void native_jvm_unchecked_long_arraycopy_entry();
extern "C" void native_jvm_unchecked_obj_arraycopy_entry();
extern "C" jint Java_com_sun_cldchi_jvm_JVM_verifyNextChunk();
extern "C" jobject Java_java_lang_Class_forName();
extern "C" jobject Java_java_lang_Class_getName();
extern "C" jobject Java_java_lang_Class_getSuperclass();
extern "C" void Java_java_lang_Class_init9();
extern "C" void Java_java_lang_Class_invoke_1clinit();
extern "C" void Java_java_lang_Class_invoke_1verify();
extern "C" jboolean Java_java_lang_Class_isArray();
extern "C" jboolean Java_java_lang_Class_isAssignableFrom();
extern "C" jboolean Java_java_lang_Class_isInstance();
extern "C" jboolean Java_java_lang_Class_isInterface();
extern "C" jobject Java_java_lang_Class_newInstance();
extern "C" jlong Java_java_lang_Double_doubleToLongBits();
extern "C" jdouble Java_java_lang_Double_longBitsToDouble();
extern "C" jint Java_java_lang_Float_floatToIntBits();
extern "C" jfloat Java_java_lang_Float_intBitsToFloat();
extern "C" void native_integer_toString_entry();
extern "C" void native_math_ceil_entry();
extern "C" void native_math_cos_entry();
extern "C" void native_math_floor_entry();
extern "C" void native_math_sin_entry();
extern "C" void native_math_sqrt_entry();
extern "C" void native_math_tan_entry();
extern "C" jobject Java_java_lang_Object_getClass();
extern "C" jint Java_java_lang_Object_hashCode();
extern "C" void Java_java_lang_Object_notify();
extern "C" void Java_java_lang_Object_notifyAll();
extern "C" void Java_java_lang_Object_wait();
extern "C" void Java_java_lang_Runtime_exitInternal();
extern "C" jlong Java_java_lang_Runtime_freeMemory();
extern "C" void Java_java_lang_Runtime_gc();
extern "C" jlong Java_java_lang_Runtime_totalMemory();
extern "C" void native_string_init_entry();
extern "C" void native_string_charAt_entry();
extern "C" void native_string_endsWith_entry();
extern "C" void native_string_equals_entry();
extern "C" jint Java_java_lang_String_hashCode();
extern "C" void native_string_indexof0_entry();
extern "C" void native_string_indexof_entry();
extern "C" void native_string_indexof0_string_entry();
extern "C" void native_string_indexof_string_entry();
extern "C" jobject Java_java_lang_String_intern();
extern "C" jint Java_java_lang_String_lastIndexOf__I();
extern "C" jint Java_java_lang_String_lastIndexOf__II();
extern "C" void native_string_startsWith0_entry();
extern "C" void native_string_startsWith_entry();
extern "C" void native_string_substringI_entry();
extern "C" void native_string_substringII_entry();
extern "C" void native_integer_toString_entry();
extern "C" void native_stringbuffer_append_entry();
extern "C" void Java_java_lang_System_arraycopy();
extern "C" void native_system_arraycopy_entry();
extern "C" jlong Java_java_lang_System_currentTimeMillis();
extern "C" jobject Java_java_lang_System_getProperty0();
extern "C" jint Java_java_lang_System_identityHashCode();
extern "C" void Java_java_lang_System_quickNativeThrow();
extern "C" jint Java_java_lang_Thread_activeCount();
extern "C" jobject Java_java_lang_Thread_currentThread();
extern "C" void Java_java_lang_Thread_internalExit();
extern "C" void Java_java_lang_Thread_interrupt0();
extern "C" jboolean Java_java_lang_Thread_isAlive();
extern "C" void Java_java_lang_Thread_setPriority0();
extern "C" void Java_java_lang_Thread_sleep();
extern "C" void Java_java_lang_Thread_start0();
extern "C" void Java_java_lang_Thread_yield();
extern "C" void Java_java_lang_Throwable_fillInStackTrace();
extern "C" void Java_java_lang_Throwable_printStackTrace();
extern "C" void Java_java_lang_ref_WeakReference_clear();
extern "C" void Java_java_lang_ref_WeakReference_finalize();
extern "C" jobject Java_java_lang_ref_WeakReference_get();
extern "C" void Java_java_lang_ref_WeakReference_initializeWeakReference();
extern "C" void native_vector_addElement_entry();
extern "C" void native_vector_elementAt_entry();


static const JvmNativeFunction com_sun_cldc_io_ResourceInputStream_natives[] = {
  JVM_NATIVE("bytesRemain",   "(Ljava/lang/Object;)I", Java_com_sun_cldc_io_ResourceInputStream_bytesRemain),
  JVM_NATIVE("clone",         "(Ljava/lang/Object;)Ljava/lang/Object;", Java_com_sun_cldc_io_ResourceInputStream_clone),
  JVM_NATIVE("open",          "(Ljava/lang/String;)Ljava/lang/Object;", Java_com_sun_cldc_io_ResourceInputStream_open),
  JVM_NATIVE("readByte",      "(Ljava/lang/Object;)I", Java_com_sun_cldc_io_ResourceInputStream_readByte),
  JVM_NATIVE("readBytes",     "(Ljava/lang/Object;[BII)I", Java_com_sun_cldc_io_ResourceInputStream_readBytes),
  {(char*)0, (char*)0, (void*)0}
};

static const JvmNativeFunction com_sun_cldc_io_j2me_socket_Protocol_natives[] = {
  JVM_NATIVE("available0",    "(I)I",                  Java_com_sun_cldc_io_j2me_socket_Protocol_available0),
  JVM_NATIVE("close0",        "(I)V",                  Java_com_sun_cldc_io_j2me_socket_Protocol_close0),
  JVM_NATIVE("open0",         "([BII)I",               Java_com_sun_cldc_io_j2me_socket_Protocol_open0),
  JVM_NATIVE("readBuf",       "(I[BII)I",              Java_com_sun_cldc_io_j2me_socket_Protocol_readBuf),
  JVM_NATIVE("readByte",      "(I)I",                  Java_com_sun_cldc_io_j2me_socket_Protocol_readByte),
  JVM_NATIVE("writeBuf",      "(I[BII)I",              Java_com_sun_cldc_io_j2me_socket_Protocol_writeBuf),
  JVM_NATIVE("writeByte",     "(II)I",                 Java_com_sun_cldc_io_j2me_socket_Protocol_writeByte),
  {(char*)0, (char*)0, (void*)0}
};

static const JvmNativeFunction com_sun_cldc_util_SemaphoreLock_natives[] = {
  JVM_NATIVE("acquire",       "()V",                   Java_com_sun_cldc_util_SemaphoreLock_acquire),
  JVM_NATIVE("release",       "()V",                   Java_com_sun_cldc_util_SemaphoreLock_release),
  {(char*)0, (char*)0, (void*)0}
};

static const JvmNativeFunction com_sun_cldchi_io_ConsoleOutputStream_natives[] = {
  JVM_NATIVE("write",         "(I)V",                  Java_com_sun_cldchi_io_ConsoleOutputStream_write),
  {(char*)0, (char*)0, (void*)0}
};

static const JvmNativeFunction com_sun_cldchi_jvm_FileDescriptor_natives[] = {
  JVM_NATIVE("finalize",      "()V",                   Java_com_sun_cldchi_jvm_FileDescriptor_finalize),
  {(char*)0, (char*)0, (void*)0}
};

static const JvmNativeFunction com_sun_cldchi_jvm_JVM_natives[] = {
  JVM_NATIVE("cancelImageCreation","()V",                   Java_com_sun_cldchi_jvm_JVM_cancelImageCreation),
  JVM_NATIVE("createAppImage0","()Z",                   Java_com_sun_cldchi_jvm_JVM_createAppImage0),
  JVM_NATIVE("createSysImage","()V",                   Java_com_sun_cldchi_jvm_JVM_createSysImage),
  JVM_NATIVE("flushJarCaches","()V",                   Java_com_sun_cldchi_jvm_JVM_flushJarCaches),
  JVM_NATIVE("getAppImageProgress","()I",                   Java_com_sun_cldchi_jvm_JVM_getAppImageProgress),
  JVM_NATIVE("loadLibrary",   "(Ljava/lang/String;)V", Java_com_sun_cldchi_jvm_JVM_loadLibrary),
  JVM_NATIVE("monotonicTimeMillis","()J",                   Java_com_sun_cldchi_jvm_JVM_monotonicTimeMillis),
  JVM_NATIVE("startAppImage", "([C[CI)V",              Java_com_sun_cldchi_jvm_JVM_startAppImage),
  JVM_NATIVE("verifyNextChunk","(Ljava/lang/String;II)I", Java_com_sun_cldchi_jvm_JVM_verifyNextChunk),
  {(char*)0, (char*)0, (void*)0}
};

static const JvmNativeFunction com_sun_cldchi_jvm_JVM_entries[] = {
  JVM_ENTRY("unchecked_byte_arraycopy", "([BI[BII)V", native_jvm_unchecked_byte_arraycopy_entry),
  JVM_ENTRY("unchecked_char_arraycopy", "([CI[CII)V", native_jvm_unchecked_char_arraycopy_entry),
  JVM_ENTRY("unchecked_int_arraycopy", "([II[III)V", native_jvm_unchecked_int_arraycopy_entry),
  JVM_ENTRY("unchecked_long_arraycopy", "([JI[JII)V", native_jvm_unchecked_long_arraycopy_entry),
  JVM_ENTRY("unchecked_obj_arraycopy", "([Ljava/lang/Object;I[Ljava/lang/Object;II)V", native_jvm_unchecked_obj_arraycopy_entry),
  {(char*)0, (char*)0, (void*)0}
};

static const JvmNativeFunction java_lang_Class_natives[] = {
  JVM_NATIVE("forName",       "(Ljava/lang/String;)Ljava/lang/Class;", Java_java_lang_Class_forName),
  JVM_NATIVE("getName",       "()Ljava/lang/String;",  Java_java_lang_Class_getName),
  JVM_NATIVE("getSuperclass", "()Ljava/lang/Class;",   Java_java_lang_Class_getSuperclass),
  JVM_NATIVE("init9",         "()V",                   Java_java_lang_Class_init9),
  JVM_NATIVE("invoke_clinit", "()V",                   Java_java_lang_Class_invoke_1clinit),
  JVM_NATIVE("invoke_verify", "()V",                   Java_java_lang_Class_invoke_1verify),
  JVM_NATIVE("isArray",       "()Z",                   Java_java_lang_Class_isArray),
  JVM_NATIVE("isAssignableFrom","(Ljava/lang/Class;)Z",  Java_java_lang_Class_isAssignableFrom),
  JVM_NATIVE("isInstance",    "(Ljava/lang/Object;)Z", Java_java_lang_Class_isInstance),
  JVM_NATIVE("isInterface",   "()Z",                   Java_java_lang_Class_isInterface),
  JVM_NATIVE("newInstance",   "()Ljava/lang/Object;",  Java_java_lang_Class_newInstance),
  {(char*)0, (char*)0, (void*)0}
};

static const JvmNativeFunction java_lang_Double_natives[] = {
  JVM_NATIVE("doubleToLongBits","(D)J",                  Java_java_lang_Double_doubleToLongBits),
  JVM_NATIVE("longBitsToDouble","(J)D",                  Java_java_lang_Double_longBitsToDouble),
  {(char*)0, (char*)0, (void*)0}
};

static const JvmNativeFunction java_lang_Float_natives[] = {
  JVM_NATIVE("floatToIntBits","(F)I",                  Java_java_lang_Float_floatToIntBits),
  JVM_NATIVE("intBitsToFloat","(I)F",                  Java_java_lang_Float_intBitsToFloat),
  {(char*)0, (char*)0, (void*)0}
};

static const JvmNativeFunction java_lang_Integer_entries[] = {
  JVM_ENTRY("toString",            "(I)Ljava/lang/String;", native_integer_toString_entry),
  {(char*)0, (char*)0, (void*)0}
};

static const JvmNativeFunction java_lang_Math_entries[] = {
  JVM_ENTRY("ceil",                "(D)D", native_math_ceil_entry),
  JVM_ENTRY("cos",                 "(D)D", native_math_cos_entry),
  JVM_ENTRY("floor",               "(D)D", native_math_floor_entry),
  JVM_ENTRY("sin",                 "(D)D", native_math_sin_entry),
  JVM_ENTRY("sqrt",                "(D)D", native_math_sqrt_entry),
  JVM_ENTRY("tan",                 "(D)D", native_math_tan_entry),
  {(char*)0, (char*)0, (void*)0}
};

static const JvmNativeFunction java_lang_Object_natives[] = {
  JVM_NATIVE("getClass",      "()Ljava/lang/Class;",   Java_java_lang_Object_getClass),
  JVM_NATIVE("hashCode",      "()I",                   Java_java_lang_Object_hashCode),
  JVM_NATIVE("notify",        "()V",                   Java_java_lang_Object_notify),
  JVM_NATIVE("notifyAll",     "()V",                   Java_java_lang_Object_notifyAll),
  JVM_NATIVE("wait",          "(J)V",                  Java_java_lang_Object_wait),
  {(char*)0, (char*)0, (void*)0}
};

static const JvmNativeFunction java_lang_Runtime_natives[] = {
  JVM_NATIVE("exitInternal",  "(I)V",                  Java_java_lang_Runtime_exitInternal),
  JVM_NATIVE("freeMemory",    "()J",                   Java_java_lang_Runtime_freeMemory),
  JVM_NATIVE("gc",            "()V",                   Java_java_lang_Runtime_gc),
  JVM_NATIVE("totalMemory",   "()J",                   Java_java_lang_Runtime_totalMemory),
  {(char*)0, (char*)0, (void*)0}
};

static const JvmNativeFunction java_lang_String_natives[] = {
  JVM_NATIVE("hashCode",      "()I",                   Java_java_lang_String_hashCode),
  JVM_NATIVE("intern",        "()Ljava/lang/String;",  Java_java_lang_String_intern),
  JVM_NATIVE("lastIndexOf",   "(I)I",                  Java_java_lang_String_lastIndexOf__I),
  JVM_NATIVE("lastIndexOf",   "(II)I",                 Java_java_lang_String_lastIndexOf__II),
  {(char*)0, (char*)0, (void*)0}
};

static const JvmNativeFunction java_lang_String_entries[] = {
  JVM_ENTRY("<init>",              "(Ljava/lang/StringBuffer;)V", native_string_init_entry),
  JVM_ENTRY("charAt",              "(I)C", native_string_charAt_entry),
  JVM_ENTRY("endsWith",            "(Ljava/lang/String;)Z", native_string_endsWith_entry),
  JVM_ENTRY("equals",              "(Ljava/lang/Object;)Z", native_string_equals_entry),
  JVM_ENTRY("indexOf",             "(I)I", native_string_indexof0_entry),
  JVM_ENTRY("indexOf",             "(II)I", native_string_indexof_entry),
  JVM_ENTRY("indexOf",             "(Ljava/lang/String;)I", native_string_indexof0_string_entry),
  JVM_ENTRY("indexOf",             "(Ljava/lang/String;I)I", native_string_indexof_string_entry),
  JVM_ENTRY("startsWith",          "(Ljava/lang/String;)Z", native_string_startsWith0_entry),
  JVM_ENTRY("startsWith",          "(Ljava/lang/String;I)Z", native_string_startsWith_entry),
  JVM_ENTRY("substring",           "(I)Ljava/lang/String;", native_string_substringI_entry),
  JVM_ENTRY("substring",           "(II)Ljava/lang/String;", native_string_substringII_entry),
  JVM_ENTRY("valueOf",             "(I)Ljava/lang/String;", native_integer_toString_entry),
  {(char*)0, (char*)0, (void*)0}
};

static const JvmNativeFunction java_lang_StringBuffer_entries[] = {
  JVM_ENTRY("append",              "(C)Ljava/lang/StringBuffer;", native_stringbuffer_append_entry),
  {(char*)0, (char*)0, (void*)0}
};

static const JvmNativeFunction java_lang_System_natives[] = {
  JVM_NATIVE("arraycopy",     "(Ljava/lang/Object;ILjava/lang/Object;II)V", Java_java_lang_System_arraycopy),
  JVM_NATIVE("currentTimeMillis","()J",                   Java_java_lang_System_currentTimeMillis),
  JVM_NATIVE("getProperty0",  "(Ljava/lang/String;)Ljava/lang/String;", Java_java_lang_System_getProperty0),
  JVM_NATIVE("identityHashCode","(Ljava/lang/Object;)I", Java_java_lang_System_identityHashCode),
  JVM_NATIVE("quickNativeThrow","()V",                   Java_java_lang_System_quickNativeThrow),
  {(char*)0, (char*)0, (void*)0}
};

static const JvmNativeFunction java_lang_System_entries[] = {
  JVM_ENTRY("arraycopy",           "(Ljava/lang/Object;ILjava/lang/Object;II)V", native_system_arraycopy_entry),
  {(char*)0, (char*)0, (void*)0}
};

static const JvmNativeFunction java_lang_Thread_natives[] = {
  JVM_NATIVE("activeCount",   "()I",                   Java_java_lang_Thread_activeCount),
  JVM_NATIVE("currentThread", "()Ljava/lang/Thread;",  Java_java_lang_Thread_currentThread),
  JVM_NATIVE("internalExit",  "()V",                   Java_java_lang_Thread_internalExit),
  JVM_NATIVE("interrupt0",    "()V",                   Java_java_lang_Thread_interrupt0),
  JVM_NATIVE("isAlive",       "()Z",                   Java_java_lang_Thread_isAlive),
  JVM_NATIVE("setPriority0",  "(II)V",                 Java_java_lang_Thread_setPriority0),
  JVM_NATIVE("sleep",         "(J)V",                  Java_java_lang_Thread_sleep),
  JVM_NATIVE("start0",        "()V",                   Java_java_lang_Thread_start0),
  JVM_NATIVE("yield",         "()V",                   Java_java_lang_Thread_yield),
  {(char*)0, (char*)0, (void*)0}
};

static const JvmNativeFunction java_lang_Throwable_natives[] = {
  JVM_NATIVE("fillInStackTrace","()V",                   Java_java_lang_Throwable_fillInStackTrace),
  JVM_NATIVE("printStackTrace","()V",                   Java_java_lang_Throwable_printStackTrace),
  {(char*)0, (char*)0, (void*)0}
};

static const JvmNativeFunction java_lang_ref_WeakReference_natives[] = {
  JVM_NATIVE("clear",         "()V",                   Java_java_lang_ref_WeakReference_clear),
  JVM_NATIVE("finalize",      "()V",                   Java_java_lang_ref_WeakReference_finalize),
  JVM_NATIVE("get",           "()Ljava/lang/Object;",  Java_java_lang_ref_WeakReference_get),
  JVM_NATIVE("initializeWeakReference","(Ljava/lang/Object;)V", Java_java_lang_ref_WeakReference_initializeWeakReference),
  {(char*)0, (char*)0, (void*)0}
};

static const JvmNativeFunction java_util_Vector_entries[] = {
  JVM_ENTRY("addElement",          "(Ljava/lang/Object;)V", native_vector_addElement_entry),
  JVM_ENTRY("elementAt",           "(I)Ljava/lang/Object;", native_vector_elementAt_entry),
  {(char*)0, (char*)0, (void*)0}
};

const JvmNativesTable jvm_natives_table[] = {
  JVM_TABLE("com/sun/cldc/io/ResourceInputStream",
                                        com_sun_cldc_io_ResourceInputStream_natives,
                                        (JvmNativeFunction*)0),
  JVM_TABLE("com/sun/cldc/io/j2me/socket/Protocol",
                                        com_sun_cldc_io_j2me_socket_Protocol_natives,
                                        (JvmNativeFunction*)0),
  JVM_TABLE("com/sun/cldc/util/SemaphoreLock",
                                        com_sun_cldc_util_SemaphoreLock_natives,
                                        (JvmNativeFunction*)0),
  JVM_TABLE("com/sun/cldchi/io/ConsoleOutputStream",
                                        com_sun_cldchi_io_ConsoleOutputStream_natives,
                                        (JvmNativeFunction*)0),
  JVM_TABLE("com/sun/cldchi/jvm/FileDescriptor",
                                        com_sun_cldchi_jvm_FileDescriptor_natives,
                                        (JvmNativeFunction*)0),
  JVM_TABLE("com/sun/cldchi/jvm/JVM",
                                        com_sun_cldchi_jvm_JVM_natives,
                                        com_sun_cldchi_jvm_JVM_entries),
  JVM_TABLE("java/lang/Class",
                                        java_lang_Class_natives,
                                        (JvmNativeFunction*)0),
  JVM_TABLE("java/lang/Double",
                                        java_lang_Double_natives,
                                        (JvmNativeFunction*)0),
  JVM_TABLE("java/lang/Float",
                                        java_lang_Float_natives,
                                        (JvmNativeFunction*)0),
  JVM_TABLE("java/lang/Integer",
                                        (JvmNativeFunction*)0,
                                        java_lang_Integer_entries),
  JVM_TABLE("java/lang/Math",
                                        (JvmNativeFunction*)0,
                                        java_lang_Math_entries),
  JVM_TABLE("java/lang/Object",
                                        java_lang_Object_natives,
                                        (JvmNativeFunction*)0),
  JVM_TABLE("java/lang/Runtime",
                                        java_lang_Runtime_natives,
                                        (JvmNativeFunction*)0),
  JVM_TABLE("java/lang/String",
                                        java_lang_String_natives,
                                        java_lang_String_entries),
  JVM_TABLE("java/lang/StringBuffer",
                                        (JvmNativeFunction*)0,
                                        java_lang_StringBuffer_entries),
  JVM_TABLE("java/lang/System",
                                        java_lang_System_natives,
                                        java_lang_System_entries),
  JVM_TABLE("java/lang/Thread",
                                        java_lang_Thread_natives,
                                        (JvmNativeFunction*)0),
  JVM_TABLE("java/lang/Throwable",
                                        java_lang_Throwable_natives,
                                        (JvmNativeFunction*)0),
  JVM_TABLE("java/lang/ref/WeakReference",
                                        java_lang_ref_WeakReference_natives,
                                        (JvmNativeFunction*)0),
  JVM_TABLE("java/util/Vector",
                                        (JvmNativeFunction*)0,
                                        java_util_Vector_entries),
  JVM_TABLE((char*)0, (JvmNativeFunction*)0, (JvmNativeFunction*)0)
};
const JvmExecutionEntry jvm_api_entries[] = {
{(unsigned char*)&native_jvm_unchecked_byte_arraycopy_entry,
(char*)"native_jvm_unchecked_byte_arraycopy_entry"},
{(unsigned char*)&native_jvm_unchecked_char_arraycopy_entry,
(char*)"native_jvm_unchecked_char_arraycopy_entry"},
{(unsigned char*)&native_jvm_unchecked_int_arraycopy_entry,
(char*)"native_jvm_unchecked_int_arraycopy_entry"},
{(unsigned char*)&native_jvm_unchecked_long_arraycopy_entry,
(char*)"native_jvm_unchecked_long_arraycopy_entry"},
{(unsigned char*)&native_jvm_unchecked_obj_arraycopy_entry,
(char*)"native_jvm_unchecked_obj_arraycopy_entry"},
{(unsigned char*)&native_integer_toString_entry,
(char*)"native_integer_toString_entry"},
{(unsigned char*)&native_math_ceil_entry,
(char*)"native_math_ceil_entry"},
{(unsigned char*)&native_math_cos_entry,
(char*)"native_math_cos_entry"},
{(unsigned char*)&native_math_floor_entry,
(char*)"native_math_floor_entry"},
{(unsigned char*)&native_math_sin_entry,
(char*)"native_math_sin_entry"},
{(unsigned char*)&native_math_sqrt_entry,
(char*)"native_math_sqrt_entry"},
{(unsigned char*)&native_math_tan_entry,
(char*)"native_math_tan_entry"},
{(unsigned char*)&native_string_init_entry,
(char*)"native_string_init_entry"},
{(unsigned char*)&native_string_charAt_entry,
(char*)"native_string_charAt_entry"},
{(unsigned char*)&native_string_endsWith_entry,
(char*)"native_string_endsWith_entry"},
{(unsigned char*)&native_string_equals_entry,
(char*)"native_string_equals_entry"},
{(unsigned char*)&native_string_indexof0_entry,
(char*)"native_string_indexof0_entry"},
{(unsigned char*)&native_string_indexof_entry,
(char*)"native_string_indexof_entry"},
{(unsigned char*)&native_string_indexof0_string_entry,
(char*)"native_string_indexof0_string_entry"},
{(unsigned char*)&native_string_indexof_string_entry,
(char*)"native_string_indexof_string_entry"},
{(unsigned char*)&native_string_startsWith0_entry,
(char*)"native_string_startsWith0_entry"},
{(unsigned char*)&native_string_startsWith_entry,
(char*)"native_string_startsWith_entry"},
{(unsigned char*)&native_string_substringI_entry,
(char*)"native_string_substringI_entry"},
{(unsigned char*)&native_string_substringII_entry,
(char*)"native_string_substringII_entry"},
{(unsigned char*)&native_integer_toString_entry,
(char*)"native_integer_toString_entry"},
{(unsigned char*)&native_stringbuffer_append_entry,
(char*)"native_stringbuffer_append_entry"},
{(unsigned char*)&native_system_arraycopy_entry,
(char*)"native_system_arraycopy_entry"},
{(unsigned char*)&native_vector_addElement_entry,
(char*)"native_vector_addElement_entry"},
{(unsigned char*)&native_vector_elementAt_entry,
(char*)"native_vector_elementAt_entry"},
{(unsigned char*)0, (char*)0}};

#endif
