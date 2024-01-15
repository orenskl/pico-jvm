#
# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (C) 2024 Oren Sokoler (https://github.com/orenskl) 
#

cmake_minimum_required(VERSION 3.22)

find_package(Java 1.8 EXACT REQUIRED)
include(ExternalProject)

project(jvm)

ExternalProject_Add(romgen SOURCE_DIR ${CMAKE_SOURCE_DIR}/tools/romgen 
                           CMAKE_ARGS -DROMGEN_INSTALL_DIR=${CMAKE_BINARY_DIR} 
                                      -DROMGEN_VERSION=${LIBJVM_VERSION}
                                      -DCMAKE_BUILD_TYPE:STRING=Release
                           USES_TERMINAL_BUILD TRUE
)

if (${TARGET} STREQUAL PICO)
    set(CMAKE_C_COMPILER   arm-none-eabi-gcc)
    set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
endif()
if (${TARGET} STREQUAL PICO)
    set(JVM_DEVICE_CLASSES ${CMAKE_SOURCE_DIR}/src/javaapi/device/pico/hardware/GPIOPin.java)
endif()

set(JVM_CLASSES 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/javax/microedition/io/ConnectionNotFoundException.java
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/javax/microedition/io/ContentConnection.java
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/javax/microedition/io/StreamConnection.java
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/javax/microedition/io/Connector.java
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/javax/microedition/io/Datagram.java
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/javax/microedition/io/InputConnection.java
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/javax/microedition/io/StreamConnectionNotifier.java
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/javax/microedition/io/DatagramConnection.java
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/javax/microedition/io/OutputConnection.java
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/javax/microedition/io/Connection.java
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/com/sun/cldc/io/j2me/socket/Protocol.java
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/com/sun/cldc/io/ConnectionBaseInterface.java
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/com/sun/cldc/i18n/uclc/DefaultCaseConverter.java
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/com/sun/cldc/i18n/j2me/ISO8859_1_Writer.java
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/com/sun/cldc/i18n/j2me/ISO8859_1_Reader.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/com/sun/cldc/i18n/StreamReader.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/com/sun/cldc/i18n/StreamWriter.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/com/sun/cldc/i18n/Helper.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/com/sun/cldc/util/j2me/TimeZoneImpl.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/com/sun/cldc/util/j2me/CalendarImpl.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/io/InputStreamReader.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/io/OutputStreamWriter.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/io/ByteArrayInputStream.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/io/DataOutputStream.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/io/IOException.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/io/UTFDataFormatException.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/io/OutputStream.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/io/InterruptedIOException.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/io/Writer.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/io/InputStream.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/io/DataInputStream.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/io/DataInput.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/io/PrintStream.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/io/Reader.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/io/UnsupportedEncodingException.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/io/EOFException.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/io/DataOutput.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/io/ByteArrayOutputStream.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/Double.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/ref/WeakReference.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/ref/Reference.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/Byte.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/FloatingDecimal.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/String.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/IllegalThreadStateException.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/Boolean.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/NegativeArraySizeException.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/RuntimeException.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/StringBuffer.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/IllegalAccessException.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/Thread.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/ClassNotFoundException.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/ArrayIndexOutOfBoundsException.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/Character.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/Float.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/SecurityException.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/IllegalMonitorStateException.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/Math.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/Runnable.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/OutOfMemoryError.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/InstantiationException.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/IndexOutOfBoundsException.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/Integer.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/NoClassDefFoundError.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/NullPointerException.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/Exception.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/StringIndexOutOfBoundsException.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/ClassCastException.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/System.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/NumberFormatException.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/Short.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/ArrayStoreException.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/Throwable.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/Long.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/IllegalArgumentException.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/InterruptedException.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/Runtime.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/ArithmeticException.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/Object.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/VirtualMachineError.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/Error.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/lang/Class.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/util/TimeZone.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/util/Date.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/util/Enumeration.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/util/Calendar.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/util/Vector.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/util/EmptyStackException.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/util/Random.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/util/NoSuchElementException.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/util/Stack.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/cldc1.1/java/util/Hashtable.java
    ${CMAKE_SOURCE_DIR}/src/javaapi/share/com/sun/cldchi/jvm/FileDescriptor.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/share/com/sun/cldchi/jvm/JVM.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/share/com/sun/cldchi/io/ConsoleOutputStream.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/share/com/sun/cldchi/test/Dummy.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/share/com/sun/cldc/io/ResourceInputStream.java 
    ${CMAKE_SOURCE_DIR}/src/javaapi/share/com/sun/cldc/util/Semaphore.java
    #
    # Device classes
    #
    ${JVM_DEVICE_CLASSES}
)

add_custom_command(OUTPUT ROMImage.cpp
                   DEPENDS ${JVM_CLASSES}
                   COMMAND mkdir ARGS -p ${CMAKE_BINARY_DIR}/classes
                   COMMAND javac ARGS -source 1.4 -target 1.4 -d ${CMAKE_BINARY_DIR}/classes -bootclasspath ${CMAKE_BINARY_DIR}/classes ${JVM_CLASSES}
                   COMMAND ${CMAKE_SOURCE_DIR}/tools/preverify/bin/preverify ARGS -d ${CMAKE_BINARY_DIR}/classes.preverify ${CMAKE_BINARY_DIR}/classes
                   COMMAND cd ARGS ${CMAKE_BINARY_DIR}/classes.preverify
                   COMMAND jar ARGS -cfM0 ${CMAKE_BINARY_DIR}/classes.jar .
                   COMMAND cd ARGS ${CMAKE_BINARY_DIR}
                   COMMAND ${CMAKE_BINARY_DIR}/romgen 
                                    -cp ${CMAKE_BINARY_DIR}/classes.jar
                                    +RewriteROMConstantPool
                                    +EnableAllROMOptimizations
                                    +GenerateGNUCode 
                                    =HeapCapacity16M
                                    -romconfig ${CMAKE_SOURCE_DIR}/src/vm/cldctest_rom.cfg
                                    -romincludepath ${CMAKE_SOURCE_DIR}/src/vm
                                    -romize
)
add_custom_target(classes ALL DEPENDS ROMImage.cpp romgen)

if (${TARGET} STREQUAL PICO)
    add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/doc
                       DEPENDS ${JVM_DEVICE_CLASSES}
                       COMMAND javadoc ARGS -d ${CMAKE_BINARY_DIR}/doc ${JVM_DEVICE_CLASSES}
    )
    add_custom_target(docs ALL DEPENDS ${CMAKE_BINARY_DIR}/doc)
endif()

if (${TARGET} STREQUAL PICO)
    set(JVM_SOURCE_OS_FILES 
        src/vm/os/pico/JVM_pico.cpp
        src/vm/os/pico/OsFile_pico.cpp
        src/vm/os/pico/OsMemory_pico.cpp
        src/vm/os/pico/OsMisc_pico.cpp
    )
endif()
if (${TARGET} STREQUAL LINUX)
    set(JVM_SOURCE_OS_FILES 
        src/vm/os/linux/JVM_linux.cpp
        src/vm/os/linux/MemoryMonitorMd_linux.cpp
        src/vm/os/linux/OsFile_linux.cpp
        src/vm/os/linux/OsMemory_linux.cpp
        src/vm/os/linux/OsMisc_linux.cpp
        src/vm/os/linux/OsSocket_linux.cpp
        src/vm/share/runtime/jvmspi.cpp
    )
endif()

add_library(jvm STATIC
    #------- Library libcldc_vm start
    src/vm/cpu/c/Addressing_c.cpp
    src/vm/cpu/c/Assembler_c.cpp
    src/vm/cpu/c/BinaryAssembler_c.cpp
    src/vm/cpu/c/CodeGenerator_c.cpp
    src/vm/cpu/c/CodeOptimizer_c.cpp
    src/vm/cpu/c/CompilationQueue_c.cpp
    src/vm/cpu/c/CompiledMethod_c.cpp
    src/vm/cpu/c/CompiledMethodDesc_c.cpp
    src/vm/cpu/c/Debug_c.cpp
    src/vm/cpu/c/Disassembler_c.cpp
    src/vm/cpu/c/FloatSupport_c.cpp
    src/vm/cpu/c/FPURegisterMap_c.cpp
    src/vm/cpu/c/Frame_c.cpp
    src/vm/cpu/c/GlobalDefinitions_c.cpp
    src/vm/cpu/c/Instructions_c.cpp
    src/vm/cpu/c/Interpreter_c.cpp
    src/vm/cpu/c/InterpreterRuntime_c.cpp
    src/vm/cpu/c/RegisterAllocator_c.cpp
    src/vm/cpu/c/VirtualStackFrame_c.cpp
    src/vm/share/compiler/Addressing.cpp
    src/vm/share/compiler/BinaryAssemblerCommon.cpp
    src/vm/share/compiler/BinaryLabel.cpp
    src/vm/share/compiler/BytecodeClosure.cpp
    src/vm/share/compiler/BytecodeCompileClosure.cpp
    src/vm/share/compiler/CodeGenerator.cpp
    src/vm/share/compiler/CompilationQueue.cpp
    src/vm/share/compiler/Compiler.cpp
    src/vm/share/compiler/CompilerObject.cpp
    src/vm/share/compiler/CompilerTest.cpp
    src/vm/share/compiler/LiteralPoolElement.cpp
    src/vm/share/compiler/Location.cpp
    src/vm/share/compiler/RegisterAllocator.cpp
    src/vm/share/compiler/Relocation.cpp
    src/vm/share/compiler/Value.cpp
    src/vm/share/compiler/VirtualStackFrame.cpp
    src/vm/share/compiler/VSFMergeTest.cpp
    src/vm/share/debugger/JavaDebugger.cpp
    src/vm/share/debugger/MemoryMonitor.cpp
    src/vm/share/debugger/MemoryMonitorMd.cpp
    src/vm/share/debugger/VMEvent.cpp
    src/vm/share/float/Cosine_kernel.cpp
    src/vm/share/float/FloatNatives.cpp
    src/vm/share/float/IEEE754_fmod.cpp
    src/vm/share/float/IEEE754_rem_pio2.cpp
    src/vm/share/float/IEEE754_sqrt.cpp
    src/vm/share/float/JFP_lib_acos.cpp
    src/vm/share/float/JFP_lib_asin.cpp
    src/vm/share/float/JFP_lib_atan.cpp
    src/vm/share/float/JFP_lib_atan2.cpp
    src/vm/share/float/JFP_lib_ceil.cpp
    src/vm/share/float/JFP_lib_copysign.cpp
    src/vm/share/float/JFP_lib_cos.cpp
    src/vm/share/float/JFP_lib_fabs.cpp
    src/vm/share/float/JFP_lib_floor.cpp
    src/vm/share/float/JFP_lib_scalbn.cpp
    src/vm/share/float/JFP_lib_sin.cpp
    src/vm/share/float/JFP_lib_tan.cpp
    src/vm/share/float/Remainder_pio2_kernel.cpp
    src/vm/share/float/Sine_kernel.cpp
    src/vm/share/float/Tangent_kernel.cpp
    src/vm/share/handles/Array.cpp
    src/vm/share/handles/ArrayClass.cpp
    src/vm/share/handles/Boundary.cpp
    src/vm/share/handles/Buffer.cpp
    src/vm/share/handles/CallInfo.cpp
    src/vm/share/handles/ClassInfo.cpp
    src/vm/share/handles/ClassParserState.cpp
    src/vm/share/handles/CompiledMethod.cpp
    src/vm/share/handles/ConstantPool.cpp
    src/vm/share/handles/EntryActivation.cpp
    src/vm/share/handles/ExecutionStack.cpp
    src/vm/share/handles/FarClass.cpp
    src/vm/share/handles/FieldType.cpp
    src/vm/share/handles/Instance.cpp
    src/vm/share/handles/InstanceClass.cpp
    src/vm/share/handles/JavaClass.cpp
    src/vm/share/handles/JavaClassObj.cpp
    src/vm/share/handles/JavaNear.cpp
    src/vm/share/handles/JavaOop.cpp
    src/vm/share/handles/MetaObjType.cpp
    src/vm/share/handles/Method.cpp
    src/vm/share/handles/MixedOop.cpp
    src/vm/share/handles/Near.cpp
    src/vm/share/handles/NearClass.cpp
    src/vm/share/handles/ObjArray.cpp
    src/vm/share/handles/ObjArrayClass.cpp
    src/vm/share/handles/ObjNear.cpp
    src/vm/share/handles/Oop.cpp
    src/vm/share/handles/OopVisitor.cpp
    src/vm/share/handles/ROMizerHashEntry.cpp
    src/vm/share/handles/Signature.cpp
    src/vm/share/handles/SoftRefArray.cpp
    src/vm/share/handles/StackmapList.cpp
    src/vm/share/handles/String.cpp
    src/vm/share/handles/StringTable.cpp
    src/vm/share/handles/Symbol.cpp
    src/vm/share/handles/Symbols.cpp
    src/vm/share/handles/SymbolTable.cpp
    src/vm/share/handles/ThreadObj.cpp
    src/vm/share/handles/TypeArray.cpp
    src/vm/share/handles/TypeArrayClass.cpp
    src/vm/share/handles/TypeSymbol.cpp
    src/vm/share/handles/Universe.cpp
    src/vm/share/handles/WeakRefArray.cpp
    src/vm/share/handles/WeakReference.cpp
    src/vm/share/interpreter/Bytecodes.cpp
    src/vm/share/interpreter/InterpreterRuntime.cpp
    src/vm/share/memory/Allocation.cpp
    src/vm/share/memory/ArrayClassDesc.cpp
    src/vm/share/memory/ClassInfoDesc.cpp
    src/vm/share/memory/ClassParserStateDesc.cpp
    src/vm/share/memory/CompiledMethodCache.cpp
    src/vm/share/memory/CompiledMethodDesc.cpp
    src/vm/share/memory/ConstantPoolDesc.cpp
    src/vm/share/memory/EntryActivationDesc.cpp
    src/vm/share/memory/ExecutionStackDesc.cpp
    src/vm/share/memory/FarClassDesc.cpp
    src/vm/share/memory/FinalizerConsDesc.cpp
    src/vm/share/memory/InstanceClassDesc.cpp
    src/vm/share/memory/JavaClassDesc.cpp
    src/vm/share/memory/LargeObject.cpp
    src/vm/share/memory/MetaObjTypeDesc.cpp
    src/vm/share/memory/MethodDesc.cpp
    src/vm/share/memory/MixedOopDesc.cpp
    src/vm/share/memory/ObjectHeap.cpp
    src/vm/share/memory/ObjNearDesc.cpp
    src/vm/share/memory/OopDesc.cpp
    src/vm/share/memory/ROMizerHashEntryDesc.cpp
    src/vm/share/memory/StackmapGenerator.cpp
    src/vm/share/memory/StackmapListDesc.cpp
    src/vm/share/memory/SymbolDesc.cpp
    src/vm/share/memoryprofiler/MemoryProfiler.cpp
    src/vm/share/natives/jni.cpp
    src/vm/share/natives/kni.cpp
    src/vm/share/natives/KniUncommon.cpp
    src/vm/share/natives/kvmcompat.cpp
    src/vm/share/natives/Natives.cpp
    src/vm/share/natives/PCSLSocket.cpp
    src/vm/share/natives/sni.cpp
    src/vm/share/ROM/BinaryROM.cpp
    src/vm/share/ROM/BytecodeOptimizer.cpp
    src/vm/share/ROM/ConstantPoolRewriter.cpp
    src/vm/share/ROM/MethodInvocationClosure.cpp
    src/vm/share/ROM/ROM.cpp
    src/vm/share/ROM/ROMBitSet.cpp
    src/vm/share/ROM/ROMClassPatternMatcher.cpp
    src/vm/share/ROM/ROMFieldPatternMatcher.cpp
    src/vm/share/ROM/ROMInliner.cpp
    src/vm/share/ROM/ROMMemberPatternMatcher.cpp
    src/vm/share/ROM/ROMMethodPatternMatcher.cpp
    src/vm/share/ROM/ROMOptimizer.cpp
    src/vm/share/ROM/ROMProfile.cpp
    src/vm/share/ROM/ROMStructsWriter.cpp
    src/vm/share/ROM/ROMTools.cpp
    src/vm/share/ROM/ROMWriter.cpp
    src/vm/share/ROM/SegmentedSourceROMWriter.cpp
    src/vm/share/ROM/SourceObjectWriter.cpp
    src/vm/share/ROM/SourceROMOptimizer.cpp
    src/vm/share/ROM/SourceROMWriter.cpp
    src/vm/share/runtime/BufferedFile.cpp
    src/vm/share/runtime/ClassFileParser.cpp
    src/vm/share/runtime/ClassPathAccess.cpp
    src/vm/share/runtime/DeadlockFinder.cpp
    src/vm/share/runtime/Field.cpp
    src/vm/share/runtime/FileDecoder.cpp
    src/vm/share/runtime/FilePath.cpp
    src/vm/share/runtime/Frame.cpp
    src/vm/share/runtime/HotRoutines0.cpp
    src/vm/share/runtime/HotRoutines1.cpp
    src/vm/share/runtime/Inflate.cpp
    src/vm/share/runtime/JarFileParser.cpp
    src/vm/share/runtime/JarFileUncommon.cpp
    src/vm/share/runtime/JavaVTable.cpp
    src/vm/share/runtime/JVM.cpp
    src/vm/share/runtime/MethodTrap.cpp
    src/vm/share/runtime/OS.cpp
    src/vm/share/runtime/OsFile.cpp
    src/vm/share/runtime/OsMemory.cpp
    src/vm/share/runtime/Profiler.cpp
    src/vm/share/runtime/Scheduler.cpp
    src/vm/share/runtime/Semaphore.cpp
    src/vm/share/runtime/Synchronizer.cpp
    src/vm/share/runtime/SystemDictionary.cpp
    src/vm/share/runtime/Task.cpp
    src/vm/share/runtime/TaskContext.cpp
    src/vm/share/runtime/Thread.cpp
    src/vm/share/runtime/Throw.cpp
    src/vm/share/runtime/Throwable.cpp
    src/vm/share/runtime/WTKProfiler.cpp
    src/vm/share/utilities/AccessFlags.cpp
    src/vm/share/utilities/Arguments.cpp
    src/vm/share/utilities/BytecodeHistogram.cpp
    src/vm/share/utilities/BytecodePrintClosure.cpp
    src/vm/share/utilities/CharacterStream.cpp
    src/vm/share/utilities/ConstantTag.cpp
    src/vm/share/utilities/Debug.cpp
    src/vm/share/utilities/ErrorMessage.cpp
    src/vm/share/utilities/EventLogger.cpp
    src/vm/share/utilities/Generator.cpp
    src/vm/share/utilities/GlobalDefinitions.cpp
    src/vm/share/utilities/Globals.cpp
    src/vm/share/utilities/PairHistogram.cpp
    src/vm/share/utilities/RemoteTracer.cpp
    src/vm/share/utilities/Stream.cpp
    src/vm/share/utilities/Timer.cpp
    src/vm/share/verifier/Verifier.cpp
    src/vm/share/verifier/VerifierFrame.cpp
    src/vm/share/verifier/VerifyMethodCodes.cpp
    src/vm/share/natives/BSDSocket.cpp
    src/vm/share/interpreter/OopMaps.cpp
    src/vm/share/natives/NativesTable.cpp
    ${JVM_SOURCE_OS_FILES}
    ${CMAKE_BINARY_DIR}/ROMImage.cpp
)
add_dependencies(jvm classes)

if (${TARGET} STREQUAL PICO)
    set(JVM_INCLUDE_OS_DIRS src/vm/os/pico ${CMAKE_BINARY_DIR})
endif()
if (${TARGET} STREQUAL LINUX)
    set(JVM_INCLUDE_OS_DIRS src/vm/os/linux)
endif()

target_include_directories(jvm PRIVATE
                    src/include 
                    src/vm/share/runtime
                    src/vm/share/memory
                    src/vm/share/utilities
                    src/vm/share/natives
                    src/vm/share/handles
                    src/vm/cpu/c
                    src/vm/share/ROM
                    src/vm/share/interpreter
                    src/vm/share/debugger
                    src/vm/share/compiler
                    src/vm/share/float
                    src/vm/share/verifier
                    ${JVM_INCLUDE_OS_DIRS}
)

target_compile_definitions(jvm PRIVATE JVM_RELEASE_VERSION="${LIBJVM_VERSION}" ROMIZING=1 PRODUCT)

set(JVM_COMPILE_OPTIONS -Wuninitialized 
                         -Wreturn-type 
                         -Wcomment 
                         -Wparentheses 
                         -Wformat
                         -Wno-narrowing
                         -fstrict-aliasing
                         -fpermissive
                         -fno-gnu-keywords
                         -fno-operator-names
                         -fno-exceptions
                         -fno-optional-diags
                         -fno-rtti
)

if (${TARGET} STREQUAL PICO)
    target_compile_definitions(jvm PRIVATE PICO)
    target_compile_options(jvm PRIVATE -mcpu=cortex-m0plus -mthumb ${JVM_COMPILE_OPTIONS})
endif()

if (${TARGET} STREQUAL LINUX)
    target_compile_options(jvm PRIVATE -m32 ${JVM_COMPILE_OPTIONS})
endif()

install(TARGETS jvm DESTINATION ${LIBJVM_INSTALL_DIR})
install(FILES ${CMAKE_BINARY_DIR}/classes.jar DESTINATION ${LIBJVM_INSTALL_DIR})
if (${TARGET} STREQUAL PICO)
    install(DIRECTORY ${CMAKE_BINARY_DIR}/doc DESTINATION ${LIBJVM_INSTALL_DIR})
endif()

