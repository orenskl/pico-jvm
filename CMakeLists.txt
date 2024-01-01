cmake_minimum_required(VERSION 3.22)

find_package(Java REQUIRED)
include(ExternalProject)

if (DEFINED PICO)
    # initialize the SDK based on PICO_SDK_PATH
    # note: this must happen before project()
    include(pico_sdk_import.cmake)
endif()

project(pjvm)

include(CTest) 

if (DEFINED PICO)
    # initialize the Raspberry Pi Pico SDK
    pico_sdk_init()
endif()

ExternalProject_Add(romgen SOURCE_DIR ${CMAKE_SOURCE_DIR}/tools/romgen 
                           CMAKE_ARGS -DROMGEN_INSTALL_DIR=${CMAKE_BINARY_DIR} 
                                      -DCMAKE_BUILD_TYPE:STRING=$<CONFIG>
                           USES_TERMINAL_BUILD TRUE
)

set(CMAKE_CXX_FLAGS_RELEASE "-O2")

add_custom_command(OUTPUT ROMImage.cpp
                   COMMAND mkdir ARGS -p ${CMAKE_BINARY_DIR}/classes
                   COMMAND javac ARGS -source 1.4 -target 1.4 -d ${CMAKE_BINARY_DIR}/classes -bootclasspath ${CMAKE_BINARY_DIR}/classes
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/javax/microedition/io/ConnectionNotFoundException.java
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/javax/microedition/io/ContentConnection.java
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/javax/microedition/io/StreamConnection.java
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/javax/microedition/io/Connector.java
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/javax/microedition/io/Datagram.java
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/javax/microedition/io/InputConnection.java
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/javax/microedition/io/StreamConnectionNotifier.java
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/javax/microedition/io/DatagramConnection.java
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/javax/microedition/io/OutputConnection.java
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/javax/microedition/io/Connection.java
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/com/sun/cldc/io/j2me/socket/Protocol.java
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/com/sun/cldc/io/ConnectionBaseInterface.java
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/com/sun/cldc/i18n/uclc/DefaultCaseConverter.java
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/com/sun/cldc/i18n/j2me/ISO8859_1_Writer.java
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/com/sun/cldc/i18n/j2me/ISO8859_1_Reader.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/com/sun/cldc/i18n/StreamReader.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/com/sun/cldc/i18n/StreamWriter.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/com/sun/cldc/i18n/Helper.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/com/sun/cldc/util/j2me/TimeZoneImpl.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/com/sun/cldc/util/j2me/CalendarImpl.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/io/InputStreamReader.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/io/OutputStreamWriter.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/io/ByteArrayInputStream.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/io/DataOutputStream.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/io/IOException.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/io/UTFDataFormatException.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/io/OutputStream.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/io/InterruptedIOException.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/io/Writer.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/io/InputStream.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/io/DataInputStream.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/io/DataInput.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/io/PrintStream.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/io/Reader.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/io/UnsupportedEncodingException.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/io/EOFException.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/io/DataOutput.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/io/ByteArrayOutputStream.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/Double.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/ref/WeakReference.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/ref/Reference.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/Byte.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/FloatingDecimal.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/String.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/IllegalThreadStateException.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/Boolean.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/NegativeArraySizeException.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/RuntimeException.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/StringBuffer.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/IllegalAccessException.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/Thread.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/ClassNotFoundException.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/ArrayIndexOutOfBoundsException.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/Character.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/Float.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/SecurityException.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/IllegalMonitorStateException.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/Math.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/Runnable.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/OutOfMemoryError.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/InstantiationException.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/IndexOutOfBoundsException.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/Integer.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/NoClassDefFoundError.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/NullPointerException.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/Exception.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/StringIndexOutOfBoundsException.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/ClassCastException.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/System.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/NumberFormatException.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/Short.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/ArrayStoreException.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/Throwable.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/Long.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/IllegalArgumentException.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/InterruptedException.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/Runtime.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/ArithmeticException.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/Object.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/VirtualMachineError.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/Error.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/lang/Class.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/util/TimeZone.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/util/Date.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/util/Enumeration.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/util/Calendar.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/util/Vector.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/util/EmptyStackException.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/util/Random.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/util/NoSuchElementException.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/util/Stack.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/cldc1.1/java/util/Hashtable.java
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/share/com/sun/cldchi/jvm/FileDescriptor.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/share/com/sun/cldchi/jvm/JVM.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/share/com/sun/cldchi/io/ConsoleOutputStream.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/share/com/sun/cldchi/test/Dummy.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/share/com/sun/cldc/io/ResourceInputStream.java 
                        ${CMAKE_SOURCE_DIR}/cldc/src/javaapi/share/com/sun/cldc/util/Semaphore.java
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
                                    -romconfig ${CMAKE_SOURCE_DIR}/cldc/src/vm/cldctest_rom.cfg
                                    -romincludepath ${CMAKE_SOURCE_DIR}/cldc/src/vm
                                    -romize
)
add_custom_target(classes ALL DEPENDS ROMImage.cpp romgen)

if (DEFINED PICO)
    set(PJVM_SOURCE_OS_FILES 
        cldc/src/vm/os/pico/JVM_pico.cpp
        cldc/src/vm/os/pico/OS_pico.cpp
        cldc/src/vm/os/pico/OsFile_pico.cpp
        cldc/src/vm/os/pico/OsMemory_pico.cpp
        cldc/src/vm/os/pico/OsMisc_pico.cpp
        cldc/src/vm/os/pico/Main_pico.cpp
    )
endif()
if (DEFINED LINUX)
    set(PJVM_SOURCE_OS_FILES 
        cldc/src/vm/os/linux/JVM_linux.cpp
        cldc/src/vm/os/linux/MemoryMonitorMd_linux.cpp
        cldc/src/vm/os/linux/OS_linux.cpp
        cldc/src/vm/os/linux/OsFile_linux.cpp
        cldc/src/vm/os/linux/OsMemory_linux.cpp
        cldc/src/vm/os/linux/OsMisc_linux.cpp
        cldc/src/vm/os/linux/OsSocket_linux.cpp
        cldc/src/vm/os/linux/Main_linux.cpp
        cldc/src/vm/share/runtime/jvmspi.cpp
    )
endif()

add_executable(pjvm
    #------- Library libcldc_vm start
    cldc/src/vm/cpu/c/Addressing_c.cpp
    cldc/src/vm/cpu/c/Assembler_c.cpp
    cldc/src/vm/cpu/c/BinaryAssembler_c.cpp
    cldc/src/vm/cpu/c/CodeGenerator_c.cpp
    cldc/src/vm/cpu/c/CodeOptimizer_c.cpp
    cldc/src/vm/cpu/c/CompilationQueue_c.cpp
    cldc/src/vm/cpu/c/CompiledMethod_c.cpp
    cldc/src/vm/cpu/c/CompiledMethodDesc_c.cpp
    cldc/src/vm/cpu/c/Debug_c.cpp
    cldc/src/vm/cpu/c/Disassembler_c.cpp
    cldc/src/vm/cpu/c/FloatSupport_c.cpp
    cldc/src/vm/cpu/c/FPURegisterMap_c.cpp
    cldc/src/vm/cpu/c/Frame_c.cpp
    cldc/src/vm/cpu/c/GlobalDefinitions_c.cpp
    cldc/src/vm/cpu/c/Instructions_c.cpp
    cldc/src/vm/cpu/c/Interpreter_c.cpp
    cldc/src/vm/cpu/c/InterpreterRuntime_c.cpp
    cldc/src/vm/cpu/c/RegisterAllocator_c.cpp
    cldc/src/vm/cpu/c/VirtualStackFrame_c.cpp
    cldc/src/vm/share/compiler/Addressing.cpp
    cldc/src/vm/share/compiler/BinaryAssemblerCommon.cpp
    cldc/src/vm/share/compiler/BinaryLabel.cpp
    cldc/src/vm/share/compiler/BytecodeClosure.cpp
    cldc/src/vm/share/compiler/BytecodeCompileClosure.cpp
    cldc/src/vm/share/compiler/CodeGenerator.cpp
    cldc/src/vm/share/compiler/CompilationQueue.cpp
    cldc/src/vm/share/compiler/Compiler.cpp
    cldc/src/vm/share/compiler/CompilerObject.cpp
    cldc/src/vm/share/compiler/CompilerTest.cpp
    cldc/src/vm/share/compiler/LiteralPoolElement.cpp
    cldc/src/vm/share/compiler/Location.cpp
    cldc/src/vm/share/compiler/RegisterAllocator.cpp
    cldc/src/vm/share/compiler/Relocation.cpp
    cldc/src/vm/share/compiler/Value.cpp
    cldc/src/vm/share/compiler/VirtualStackFrame.cpp
    cldc/src/vm/share/compiler/VSFMergeTest.cpp
    cldc/src/vm/share/debugger/JavaDebugger.cpp
    cldc/src/vm/share/debugger/MemoryMonitor.cpp
    cldc/src/vm/share/debugger/MemoryMonitorMd.cpp
    cldc/src/vm/share/debugger/VMEvent.cpp
    cldc/src/vm/share/float/Cosine_kernel.cpp
    cldc/src/vm/share/float/FloatNatives.cpp
    cldc/src/vm/share/float/IEEE754_fmod.cpp
    cldc/src/vm/share/float/IEEE754_rem_pio2.cpp
    cldc/src/vm/share/float/IEEE754_sqrt.cpp
    cldc/src/vm/share/float/JFP_lib_acos.cpp
    cldc/src/vm/share/float/JFP_lib_asin.cpp
    cldc/src/vm/share/float/JFP_lib_atan.cpp
    cldc/src/vm/share/float/JFP_lib_atan2.cpp
    cldc/src/vm/share/float/JFP_lib_ceil.cpp
    cldc/src/vm/share/float/JFP_lib_copysign.cpp
    cldc/src/vm/share/float/JFP_lib_cos.cpp
    cldc/src/vm/share/float/JFP_lib_fabs.cpp
    cldc/src/vm/share/float/JFP_lib_floor.cpp
    cldc/src/vm/share/float/JFP_lib_scalbn.cpp
    cldc/src/vm/share/float/JFP_lib_sin.cpp
    cldc/src/vm/share/float/JFP_lib_tan.cpp
    cldc/src/vm/share/float/Remainder_pio2_kernel.cpp
    cldc/src/vm/share/float/Sine_kernel.cpp
    cldc/src/vm/share/float/Tangent_kernel.cpp
    cldc/src/vm/share/handles/Array.cpp
    cldc/src/vm/share/handles/ArrayClass.cpp
    cldc/src/vm/share/handles/Boundary.cpp
    cldc/src/vm/share/handles/Buffer.cpp
    cldc/src/vm/share/handles/CallInfo.cpp
    cldc/src/vm/share/handles/ClassInfo.cpp
    cldc/src/vm/share/handles/ClassParserState.cpp
    cldc/src/vm/share/handles/CompiledMethod.cpp
    cldc/src/vm/share/handles/ConstantPool.cpp
    cldc/src/vm/share/handles/EntryActivation.cpp
    cldc/src/vm/share/handles/ExecutionStack.cpp
    cldc/src/vm/share/handles/FarClass.cpp
    cldc/src/vm/share/handles/FieldType.cpp
    cldc/src/vm/share/handles/Instance.cpp
    cldc/src/vm/share/handles/InstanceClass.cpp
    cldc/src/vm/share/handles/JavaClass.cpp
    cldc/src/vm/share/handles/JavaClassObj.cpp
    cldc/src/vm/share/handles/JavaNear.cpp
    cldc/src/vm/share/handles/JavaOop.cpp
    cldc/src/vm/share/handles/MetaObjType.cpp
    cldc/src/vm/share/handles/Method.cpp
    cldc/src/vm/share/handles/MixedOop.cpp
    cldc/src/vm/share/handles/Near.cpp
    cldc/src/vm/share/handles/NearClass.cpp
    cldc/src/vm/share/handles/ObjArray.cpp
    cldc/src/vm/share/handles/ObjArrayClass.cpp
    cldc/src/vm/share/handles/ObjNear.cpp
    cldc/src/vm/share/handles/Oop.cpp
    cldc/src/vm/share/handles/OopVisitor.cpp
    cldc/src/vm/share/handles/ROMizerHashEntry.cpp
    cldc/src/vm/share/handles/Signature.cpp
    cldc/src/vm/share/handles/SoftRefArray.cpp
    cldc/src/vm/share/handles/StackmapList.cpp
    cldc/src/vm/share/handles/String.cpp
    cldc/src/vm/share/handles/StringTable.cpp
    cldc/src/vm/share/handles/Symbol.cpp
    cldc/src/vm/share/handles/Symbols.cpp
    cldc/src/vm/share/handles/SymbolTable.cpp
    cldc/src/vm/share/handles/ThreadObj.cpp
    cldc/src/vm/share/handles/TypeArray.cpp
    cldc/src/vm/share/handles/TypeArrayClass.cpp
    cldc/src/vm/share/handles/TypeSymbol.cpp
    cldc/src/vm/share/handles/Universe.cpp
    cldc/src/vm/share/handles/WeakRefArray.cpp
    cldc/src/vm/share/handles/WeakReference.cpp
    cldc/src/vm/share/interpreter/Bytecodes.cpp
    cldc/src/vm/share/interpreter/InterpreterRuntime.cpp
    cldc/src/vm/share/memory/Allocation.cpp
    cldc/src/vm/share/memory/ArrayClassDesc.cpp
    cldc/src/vm/share/memory/ClassInfoDesc.cpp
    cldc/src/vm/share/memory/ClassParserStateDesc.cpp
    cldc/src/vm/share/memory/CompiledMethodCache.cpp
    cldc/src/vm/share/memory/CompiledMethodDesc.cpp
    cldc/src/vm/share/memory/ConstantPoolDesc.cpp
    cldc/src/vm/share/memory/EntryActivationDesc.cpp
    cldc/src/vm/share/memory/ExecutionStackDesc.cpp
    cldc/src/vm/share/memory/FarClassDesc.cpp
    cldc/src/vm/share/memory/FinalizerConsDesc.cpp
    cldc/src/vm/share/memory/InstanceClassDesc.cpp
    cldc/src/vm/share/memory/JavaClassDesc.cpp
    cldc/src/vm/share/memory/LargeObject.cpp
    cldc/src/vm/share/memory/MetaObjTypeDesc.cpp
    cldc/src/vm/share/memory/MethodDesc.cpp
    cldc/src/vm/share/memory/MixedOopDesc.cpp
    cldc/src/vm/share/memory/ObjectHeap.cpp
    cldc/src/vm/share/memory/ObjNearDesc.cpp
    cldc/src/vm/share/memory/OopDesc.cpp
    cldc/src/vm/share/memory/ROMizerHashEntryDesc.cpp
    cldc/src/vm/share/memory/StackmapGenerator.cpp
    cldc/src/vm/share/memory/StackmapListDesc.cpp
    cldc/src/vm/share/memory/SymbolDesc.cpp
    cldc/src/vm/share/memoryprofiler/MemoryProfiler.cpp
    cldc/src/vm/share/natives/jni.cpp
    cldc/src/vm/share/natives/kni.cpp
    cldc/src/vm/share/natives/KniUncommon.cpp
    cldc/src/vm/share/natives/kvmcompat.cpp
    cldc/src/vm/share/natives/Natives.cpp
    cldc/src/vm/share/natives/PCSLSocket.cpp
    cldc/src/vm/share/natives/sni.cpp
    cldc/src/vm/share/ROM/BinaryROM.cpp
#    cldc/src/vm/share/ROM/BinaryObjectWriter.cpp
#    cldc/src/vm/share/ROM/BinaryROMWriter.cpp
    cldc/src/vm/share/ROM/BytecodeOptimizer.cpp
    cldc/src/vm/share/ROM/ConstantPoolRewriter.cpp
    cldc/src/vm/share/ROM/MethodInvocationClosure.cpp
    cldc/src/vm/share/ROM/ROM.cpp
    cldc/src/vm/share/ROM/ROMBitSet.cpp
    cldc/src/vm/share/ROM/ROMClassPatternMatcher.cpp
    cldc/src/vm/share/ROM/ROMFieldPatternMatcher.cpp
    cldc/src/vm/share/ROM/ROMInliner.cpp
    cldc/src/vm/share/ROM/ROMMemberPatternMatcher.cpp
    cldc/src/vm/share/ROM/ROMMethodPatternMatcher.cpp
    cldc/src/vm/share/ROM/ROMOptimizer.cpp
    cldc/src/vm/share/ROM/ROMProfile.cpp
    cldc/src/vm/share/ROM/ROMStructsWriter.cpp
    cldc/src/vm/share/ROM/ROMTools.cpp
    cldc/src/vm/share/ROM/ROMWriter.cpp
    cldc/src/vm/share/ROM/SegmentedSourceROMWriter.cpp
    cldc/src/vm/share/ROM/SourceObjectWriter.cpp
    cldc/src/vm/share/ROM/SourceROMOptimizer.cpp
    cldc/src/vm/share/ROM/SourceROMWriter.cpp
    cldc/src/vm/share/runtime/BufferedFile.cpp
    cldc/src/vm/share/runtime/ClassFileParser.cpp
    cldc/src/vm/share/runtime/ClassPathAccess.cpp
    cldc/src/vm/share/runtime/DeadlockFinder.cpp
    cldc/src/vm/share/runtime/Field.cpp
    cldc/src/vm/share/runtime/FileDecoder.cpp
    cldc/src/vm/share/runtime/FilePath.cpp
    cldc/src/vm/share/runtime/Frame.cpp
    cldc/src/vm/share/runtime/HotRoutines0.cpp
    cldc/src/vm/share/runtime/HotRoutines1.cpp
    cldc/src/vm/share/runtime/Inflate.cpp
    cldc/src/vm/share/runtime/JarFileParser.cpp
    cldc/src/vm/share/runtime/JarFileUncommon.cpp
    cldc/src/vm/share/runtime/JavaVTable.cpp
    cldc/src/vm/share/runtime/JVM.cpp
    cldc/src/vm/share/runtime/MethodTrap.cpp
    cldc/src/vm/share/runtime/OS.cpp
    cldc/src/vm/share/runtime/OsFile.cpp
    cldc/src/vm/share/runtime/OsMemory.cpp
    cldc/src/vm/share/runtime/Profiler.cpp
    cldc/src/vm/share/runtime/Scheduler.cpp
    cldc/src/vm/share/runtime/Semaphore.cpp
    cldc/src/vm/share/runtime/Synchronizer.cpp
    cldc/src/vm/share/runtime/SystemDictionary.cpp
    cldc/src/vm/share/runtime/Task.cpp
    cldc/src/vm/share/runtime/TaskContext.cpp
    cldc/src/vm/share/runtime/Thread.cpp
    cldc/src/vm/share/runtime/Throw.cpp
    cldc/src/vm/share/runtime/Throwable.cpp
    cldc/src/vm/share/runtime/WTKProfiler.cpp
    cldc/src/vm/share/utilities/AccessFlags.cpp
    cldc/src/vm/share/utilities/Arguments.cpp
    cldc/src/vm/share/utilities/BytecodeHistogram.cpp
    cldc/src/vm/share/utilities/BytecodePrintClosure.cpp
    cldc/src/vm/share/utilities/CharacterStream.cpp
    cldc/src/vm/share/utilities/ConstantTag.cpp
    cldc/src/vm/share/utilities/Debug.cpp
    cldc/src/vm/share/utilities/ErrorMessage.cpp
    cldc/src/vm/share/utilities/EventLogger.cpp
    cldc/src/vm/share/utilities/Generator.cpp
    cldc/src/vm/share/utilities/GlobalDefinitions.cpp
    cldc/src/vm/share/utilities/Globals.cpp
    cldc/src/vm/share/utilities/PairHistogram.cpp
    cldc/src/vm/share/utilities/RemoteTracer.cpp
    cldc/src/vm/share/utilities/Stream.cpp
    cldc/src/vm/share/utilities/Timer.cpp
    cldc/src/vm/share/verifier/Verifier.cpp
    cldc/src/vm/share/verifier/VerifierFrame.cpp
    cldc/src/vm/share/verifier/VerifyMethodCodes.cpp
    ${PJVM_SOURCE_OS_FILES}
    #------- Library libcldc_vm end
    cldc/src/vm/share/natives/BSDSocket.cpp
    #------- cldc_vm exec start
    ${CMAKE_BINARY_DIR}/ROMImage.cpp
    cldc/src/vm/share/interpreter/OopMaps.cpp
    cldc/src/vm/share/natives/NativesTable.cpp
)
add_dependencies(pjvm classes)

if (DEFINED PICO)
    set(PJVM_INCLUDE_OS_DIRS cldc/src/vm/os/pico
                             ${CMAKE_BINARY_DIR})
endif()
if (DEFINED LINUX)
    set(PJVM_INCLUDE_OS_DIRS cldc/src/vm/os/linux)
endif()

target_include_directories(pjvm PRIVATE
                    cldc/src/include 
                    cldc/src/vm/share/runtime
                    cldc/src/vm/share/memory
                    cldc/src/vm/share/utilities
                    cldc/src/vm/share/natives
                    cldc/src/vm/share/handles
                    cldc/src/vm/cpu/c
                    cldc/src/vm/share/ROM
                    cldc/src/vm/share/interpreter
                    cldc/src/vm/share/debugger
                    cldc/src/vm/share/compiler
                    cldc/src/vm/share/float
                    cldc/src/vm/share/verifier
                    ${PJVM_INCLUDE_OS_DIRS}
)

if (DEFINED PICO)
    # Add pico_stdlib library which aggregates commonly used features
    target_link_libraries(pjvm pico_stdlib)
    # create map/bin/hex/uf2 file in addition to ELF.
    pico_add_extra_outputs(pjvm)
    pico_enable_stdio_usb(pjvm 1)
    pico_enable_stdio_uart(pjvm 0)
endif()

target_compile_definitions(pjvm PRIVATE
                        SUPPORTS_MONOTONIC_CLOCK=0
                        GCC 
                        REQUIRES_JVMCONFIG_H=1 
                        HARDWARE_LITTLE_ENDIAN=1
                        HOST_LITTLE_ENDIAN=1
                        JVM_RELEASE_VERSION="1.1"
                        JVM_BUILD_VERSION="internal"
                        JVM_NAME="phoneME Feature VM"
                        ROMIZING=1
)

set(PJVM_COMPILE_OPTIONS -Wuninitialized 
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

if (DEFINED PICO)
    target_compile_definitions(pjvm PRIVATE PICO ARM)
    target_compile_options(pjvm PRIVATE ${PJVM_COMPILE_OPTIONS})
endif()

if (DEFINED LINUX)
    target_compile_definitions(pjvm PRIVATE LINUX)
    target_compile_options(pjvm PRIVATE -m32 ${PJVM_COMPILE_OPTIONS})
    target_link_options(pjvm PRIVATE -m32)
    target_link_libraries(pjvm m pthread)
endif()

set(CMAKE_JAVA_COMPILE_FLAGS -source 1.4 -target 1.4)
add_custom_command(OUTPUT main.jar.bin 
                   COMMAND mkdir ARGS -p ${CMAKE_BINARY_DIR}/main.dir
                   COMMAND javac ARGS -source 1.4 -target 1.4 -d ${CMAKE_BINARY_DIR}/main.dir -bootclasspath ${CMAKE_BINARY_DIR}/classes.jar
                        ${CMAKE_SOURCE_DIR}/test/Main.java
                   COMMAND cd ARGS ${CMAKE_BINARY_DIR}/main.dir
                   COMMAND jar ARGS -cfM0 ${CMAKE_BINARY_DIR}/main.jar .
                   COMMAND ${CMAKE_SOURCE_DIR}/tools/wrapjar.sh ARGS ${CMAKE_BINARY_DIR}/main.jar ${CMAKE_BINARY_DIR}/main.jar.bin
)
add_custom_target(main ALL DEPENDS main.jar.bin)

add_test(Sanity ${CMAKE_BINARY_DIR}/pjvm -cp ${CMAKE_BINARY_DIR}/main.jar Main)
