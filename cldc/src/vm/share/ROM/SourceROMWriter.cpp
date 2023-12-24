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

#include "jvmconfig.h"

#include "BuildFlags.hpp"
#include "GlobalDefinitions.hpp"
#include "GlobalDefinitions_c.hpp"
#include "GlobalDefinitions_gcc.hpp"
#include "Globals.hpp"

#include "SourceROMWriter.hpp"
#include "OS.hpp"
#include "Throwable.hpp"
#include "ROMStructsWriter.hpp"
#include "CompiledMethod.hpp"
#include "String.hpp"
#include "Field.hpp"
#include "JarFileParser.hpp"
#include "ClassPathAccess.hpp"
#include "Inflate.hpp"
#include "SymbolTable.hpp"
#include "StringTable.hpp"
#include "SystemDictionary.hpp"
#include "Task.hpp"
#include "Generator.hpp"

#if USE_SOURCE_IMAGE_GENERATOR

void SourceROMWriter::save_file_streams(void) {
  ROMWriter::save_file_streams();

  _declare_stream.save(&_declare_stream_state);
  _main_stream.save(&_main_stream_state);
  _reloc_stream.save(&_reloc_stream_state);
  _jni_stream.save(&_jni_stream_state);
  _kvm_stream.save(&_kvm_stream_state);
}

inline void SourceROMWriter::restore_file_streams(void) {
  ROMWriter::restore_file_streams();

  _declare_stream.restore(&_declare_stream_state);
  _main_stream.restore(&_main_stream_state);
  _reloc_stream.restore(&_reloc_stream_state);
  _jni_stream.restore(&_jni_stream_state);
  _kvm_stream.restore(&_kvm_stream_state);

  _comment_stream = &_main_stream;
}

// This method is called for 1 time after JVM_Start() is called.
void SourceROMWriter::initialize() {
  ROMWriter::initialize();
  set_state(STATE_VIRGIN);
}

SourceROMWriter::SourceROMWriter() : ROMWriter() {
  // This constructor is NOT just called to initialize the romizer.
  // It's called every time when the romizer is resumed.
  restore_file_streams();
}

void SourceROMWriter::start(JVM_SINGLE_ARG_TRAPS) {
  // All of these handles must be cleared before we set the state to be
  // active.
  jvm_memset(_romwriter_oops, 0, sizeof(_romwriter_oops));
  _optimizer.init_handles();

  //tty->print_cr("My sizes = %d, %d", sizeof(*this), sizeof(ROMOptimizer));

  GUARANTEE(state() == STATE_VIRGIN, "sanity");
  set_state(STATE_START);
  ROMWriter::start(JVM_SINGLE_ARG_CHECK);
  _optimizer.initialize(&_optimizer_log_stream JVM_CHECK);
  constant_string_table()->initialize(256, 1 JVM_CHECK);
  set_constant_string_count(0);
  set_next_state();
  save_file_streams();
}

// Returns true iff romization is suspended and needs to be resumed later..
bool SourceROMWriter::execute(JVM_SINGLE_ARG_TRAPS) {
#if ENABLE_PERFORMANCE_COUNTERS
  jlong start_time = Os::elapsed_counter();
#endif

  bool suspended = execute0(JVM_SINGLE_ARG_NO_CHECK);

  if (CURRENT_HAS_PENDING_EXCEPTION) {
    // The exception is not cleared here, so it will be thrown
    // and will cause the loop in System.createRomImage() to terminate.
    tty->print_cr("ROMizing failed with Exception: ");
    Throwable exception = Thread::current_pending_exception();
    exception.print_stack_trace();
    tty->cr();
    set_state(STATE_FAILED);
  } else if (!suspended) {
    // Romization has completed. Reset romizer to inactive
    // to disable ROMWritet::oops_do().
    set_state(STATE_SUCCEEDED);
  }

#if ENABLE_PERFORMANCE_COUNTERS
  jlong elapsed = Os::elapsed_counter() - start_time;
  jvm_perf_count.num_of_romizer_steps ++;
  jvm_perf_count.total_romizer_hrticks += elapsed;
  if (jvm_perf_count.max_romizer_hrticks < elapsed) {
      jvm_perf_count.max_romizer_hrticks = elapsed;
  }
#endif

  return suspended;
}

void SourceROMWriter::load_all_classes(JVM_SINGLE_ARG_TRAPS) {
  sort_and_load_all_in_classpath(JVM_SINGLE_ARG_NO_CHECK_AT_BOTTOM);
}

// Returns true iff romization is suspended and needs to be resumed later..
bool SourceROMWriter::execute0(JVM_SINGLE_ARG_TRAPS) {
#if 0
  static int count = 0;
  tty->print_cr("doing work %d: %d", ++count, state());
#endif
  start_work_timer();

  GUARANTEE(state() > STATE_START, "romizer must have been started");

  do {
    switch (state()) {
    case STATE_OPTIMIZE:
      _optimizer.optimize(&_optimizer_log_stream JVM_CHECK_0);
      if (_optimizer.is_done()) {
        set_next_state();
      }
      break;
    case STATE_FIXUP_IMAGE:
      // Fix up persistent handles, etc
      fixup_image(JVM_SINGLE_ARG_CHECK_0);
      set_next_state();
      break;
    case STATE_WRITE_IMAGE:
      // Write the ROM image
      write_image(JVM_SINGLE_ARG_CHECK_0);
      set_next_state();
      break;
    case STATE_WRITE_STRUCT:
      if (GenerateROMStructs || GenerateROMStructsStatics) {
        ROMStructsWriter rom_structs_writer;
        rom_structs_writer.write(this JVM_CHECK_0);
      }
      set_next_state();
      break;
    case STATE_WRITE_REPORTS:
      write_reports(JVM_SINGLE_ARG_CHECK_0);
      set_next_state();
      break;
    case STATE_COMBINE_OUTPUT_FILES:
      combine_output_files();
      set_next_state();
      break;
    default:
      SHOULD_NOT_REACH_HERE();
    }
  } while (!work_timer_has_expired() && state() < STATE_DONE);

  save_file_streams();
  return (state() < STATE_DONE);
}


// This is main routine in the romizer that deals with writing
// the ROM image to 3 C++ output files. These 3 files are combined
// later by combine_output_files() to create ROMImage.cpp
void SourceROMWriter::write_image(JVM_SINGLE_ARG_TRAPS) {
  *Universe::system_dictionary() = Universe::current_dictionary();
  
  // Find out where all the objects should live (TEXT, DATA or HEAP,
  // and at what offset). Then write them to the output files.
  find_types(JVM_SINGLE_ARG_CHECK);
  find_offsets(JVM_SINGLE_ARG_CHECK);
  _optimizer.set_classes_as_romized();
  write_objects(JVM_SINGLE_ARG_CHECK);

#if ENABLE_COMPILER
  if (EnableAOTSymbolTable) {
    write_aot_symbol_table(JVM_SINGLE_ARG_CHECK);
  }
#endif

  _ending_free_heap = ObjectHeap::free_memory();
}

void SourceROMWriter::write_text_defines(FileStream* stream) {
  stream->print_cr("#define TEXT(x)  (int)&_rom_text_block[x]");
  stream->print_cr("#define TEXTb(x) (int)&(((char*)_rom_text_block)[x])");
}

void SourceROMWriter::write_text_undefines(FileStream* stream) {
  stream->print_cr("#undef TEXT");
  stream->print_cr("#undef TEXTb");
}

void SourceROMWriter::init_declare_stream() {
  _declare_stream.print_cr("#include \"jvmconfig.h\"");
  _declare_stream.print_cr("#ifdef ROMIZING");
  _declare_stream.print_cr("#include \"ROMImage.hpp\"");
  _declare_stream.print_cr("#include \"kni.h\"");

  // Write the macros needed by the contents in _stream
  write_text_undefines(&_declare_stream);

  _declare_stream.print_cr("#undef DATA");
  _declare_stream.print_cr("#undef DATAb");
  _declare_stream.print_cr("#undef HEAP");
  _declare_stream.cr();

  write_text_defines(&_declare_stream);

  _declare_stream.print_cr("#define DATA(x)  (int)&_rom_data_block[x]");
  _declare_stream.print_cr("#define DATAb(x) (int)&(((char*)_rom_data_block)[x])");
  _declare_stream.print_cr("#define HEAP(x)  (int)&_rom_heap_block[x]");
  _declare_stream.cr();
}

/*
 * Create the streams used for generating ROMImage.cpp and ROMLog.txt.
 * Note we use several streams for ROMImage.cpp. That's because we
 * sometimes need to generate two blocks of code at the same time.
 *
 * We also use two streams for ROMLog.txt so that we can put the summary at
 * the top of the file.
 */
void SourceROMWriter::init_streams() {
  _declare_stream.open(FilePath::rom_declare_file);
  _main_stream.open(FilePath::rom_main_file);
  _reloc_stream.open(FilePath::rom_reloc_file);

  _summary_log_stream.open(FilePath::rom_summary_file);
  _optimizer_log_stream.open(FilePath::rom_optimizer_file);
  _jni_stream.open(FilePath::rom_jni_adapters_file);
  _kvm_stream.open(FilePath::rom_kvm_natives_file);
  
  write_copyright(&_summary_log_stream, false);

  write_copyright(&_jni_stream, true);
  _jni_stream.cr();
  _jni_stream.print_cr("#include \"jvmconfig.h\"");
  _jni_stream.print_cr("#include \"ROMImage.hpp\"");
  _jni_stream.print_cr("#include \"kni.h\"");

#if ENABLE_JNI
  _jni_stream.print_cr("#include \"jni.h\"");
  _jni_stream.cr();
  _jni_stream.print_cr("extern \"C\" JNIEnv _jni_env;");
  _jni_stream.cr();
  _jni_stream.print_cr("extern \"C\" void * decode_handle(void*);");
#endif

  _jni_stream.cr();

  _kvm_stream.print_cr("#include \"jvmconfig.h\"");
  _kvm_stream.print_cr("#include \"ROMImage.hpp\"");
  _kvm_stream.print_cr("#include \"kni.h\"");
  _kvm_stream.cr();

  _reloc_stream.cr();
  _reloc_stream.print_cr("const int _rom_has_linked_image = 1;");
  _reloc_stream.print   ("const int _rom_flags = 0");
  if (CompactROMMethodTables) {
    _reloc_stream.print(" | ROM_HAS_COMPACT_METHOD_TABLE");
  }
  _reloc_stream.print_cr(";");
  _reloc_stream.cr();

  write_copyright(&_declare_stream, true);

  init_declare_stream();
}

void SourceROMWriter::write_copyright(Stream *stream, bool c_style_comments) {
  const char * header;

  if (c_style_comments) {
    header =
      "/*\n"
      " * Copyright  1990-2007 Sun Microsystems, Inc. All Rights Reserved.\n"
      " * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER\n"
      " * \n"
      " * This program is free software; you can redistribute it and/or\n"
      " * modify it under the terms of the GNU General Public License version\n"
      " * 2 only, as published by the Free Software Foundation.\n"
      " * \n"
      " * This program is distributed in the hope that it will be useful, but\n"
      " * WITHOUT ANY WARRANTY; without even the implied warranty of\n"
      " * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU\n"
      " * General Public License version 2 for more details (a copy is\n"
      " * included at /legal/license.txt).\n"
      " * \n"
      " * You should have received a copy of the GNU General Public License\n"
      " * version 2 along with this work; if not, write to the Free Software\n"
      " * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA\n"
      " * 02110-1301 USA\n"
      " * \n"
      " * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa\n"
      " * Clara, CA 95054 or visit www.sun.com if you need additional\n"
      " * information or have any questions.\n"
      " */\n"
      "\n"
      "/* This file is auto-generated. Do not edit*/\n";
  } else {
    header =
      "Copyright  1990-2007 Sun Microsystems, Inc. All Rights Reserved.\n"
      "DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER\n"
      "\n"
      "This program is free software; you can redistribute it and/or\n"
      "modify it under the terms of the GNU General Public License version\n"
      "2 only, as published by the Free Software Foundation.\n"
      "\n"
      "This program is distributed in the hope that it will be useful, but\n"
      "WITHOUT ANY WARRANTY; without even the implied warranty of\n"
      "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU\n"
      "General Public License version 2 for more details (a copy is\n"
      "included at /legal/license.txt).\n"
      "\n"
      "You should have received a copy of the GNU General Public License\n"
      "version 2 along with this work; if not, write to the Free Software\n"
      "Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA\n"
      "02110-1301 USA\n"
      "\n"
      "Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa\n"
      "Clara, CA 95054 or visit www.sun.com if you need additional\n"
      "information or have any questions.\n";
  }

  stream->print_cr(header);
  stream->cr();
}

class TextKlassLookupTable : public ROMLookupTable {
public:
  HANDLE_DEFINITION(TextKlassLookupTable, ROMLookupTable);

  void initialize(int num_buckets, int num_attributes JVM_TRAPS) {
    ROMLookupTable::initialize(num_buckets, num_attributes JVM_CHECK);
    set_hashcode_func((hashcode_func_type)&TextKlassLookupTable::_hashcode);
  }

  juint _hashcode(Oop *obj) {
    SETUP_ERROR_CHECKER_ARG;
    ROMWriter* romwriter = ROMWriter::singleton();
#if USE_SEGMENTED_TEXT_BLOCK_WRITER
    juint byte_offset = romwriter->loc_offset_of(obj JVM_NO_CHECK);
#else
    juint byte_offset = romwriter->offset_of(obj JVM_NO_CHECK);
#endif
    juint code = byte_offset / 4;
    GUARANTEE(!CURRENT_HAS_PENDING_EXCEPTION, "sanity");
    return code;
  }
};

/**
 * Writes _rom_text_klass_table, which is used by non-PRODUCT modes
 * to get the "klass" field of objects in the TEXT block. Most
 * of these objects have their "klass" field skipped to save
 * footprint.
 */
void SourceROMWriter::write_text_klass_table(JVM_SINGLE_ARG_TRAPS) {
  tty->print_cr("Writing text_klass_table ...");

  int num_buckets = NUM_TEXT_KLASS_BUCKETS, i;

  UsingFastOops fast_oops;  
  ROMizerHashEntry::Fast info;
  Oop::Fast oop;
  Oop::Fast klass;
  Oop::Fast record;
  int count = 0;
  TextKlassLookupTable::Fast table;
  table().initialize(num_buckets, 0 JVM_CHECK);

  //
  // (1) Iterate over all objects in the _info_table, and add them into
  //     TextKlassLookupTable
  //
  for (int bucket=0; bucket<INFO_TABLE_SIZE; bucket++) {
    for (info = info_table()->obj_at(bucket); !info.is_null(); ) {
      oop = info().referent(); // get the object
      if (oop.not_null()) {
        // Why would it be NULL?
        if (is_text_subtype(info().type())) 
        {
          table().put(&oop JVM_CHECK);
          count++;
        }
      }
      info = info().next(); // move onto the next link
    }
  }

  main_stream()->print_cr("#ifndef PRODUCT");
  OffsetVector sorter;
  sorter.initialize(JVM_SINGLE_ARG_CHECK);

  //
  // (2) Print out the individual buckets
  //

  for (i=0; i<num_buckets; i++) {
    int num_written = 0;
    sorter.flush();
    main_stream()->print("static const int klass_table_%d[] = {\n\t", i);
    record = table().get_record_at(i);

    // Sort the content of each bucket, so that we have the same output
    // when romizing on different hosts.
    while (!record.is_null()) {
      oop = table().get_key_from_record(&record);
      record = table().get_next_record(&record);
      sorter.add_element(&oop JVM_CHECK);
    }

    sorter.sort();

    for (int n=0; n<sorter.size(); n++) {
      oop = sorter.element_at(n);
      klass = oop.klass();
      if (GenerateROMComments && VerbosePointers) {
        main_stream()->print("/* (0x%x)->klass = 0x%x*/ ", 
                      (int)(oop.obj()), (int)(klass.obj()));
      }
      num_written ++;
      write_reference(&oop, TEXT_BLOCK, main_stream() JVM_CHECK);
      main_stream()->print(", ");
      write_reference(&klass, TEXT_BLOCK, main_stream() JVM_CHECK);
      if (GenerateROMComments || ((num_written % 2) == 0)) {
        main_stream()->print(",\n\t");
      } else {
        main_stream()->print(", ");
      }
    }
    main_stream()->print_cr("0, 0 };");
  }

  // Print the table, which points to all the buckets.
  main_stream()->print_cr("const int  _rom_text_klass_table_size = %d;", num_buckets);
  main_stream()->print_cr("const int* const _rom_text_klass_table[] = {");
  for (i=0; i<num_buckets; i++) {
    main_stream()->print_cr("\t(const int*)klass_table_%d, ", i);
  }
  main_stream()->print_cr("};");

  main_stream()->print_cr("#endif /*  PRODUCT */");
}
bool SourceROMWriter::is_text_subtype(int type) {
#if ENABLE_HEAP_NEARS_IN_HEAP
  if (type == TEXT_AND_HEAP_BLOCK) return true;
#endif
  return type == TEXT_BLOCK;
}
// IMPL_NOTE: move code inside #if #endif to SegmentedSourceROMWriter::find_offsets
void SourceROMWriter::find_offsets(JVM_SINGLE_ARG_TRAPS) {
  OffsetFinder offset_finder;
  int text_subtype_0_end;
  int text_subtype_1_end;
  int text_subtype_2_end;
  int text_subtype_3_end;
  int text_subtype_4_end;
  int text_subtype_5_end;
  int text_subtype_6_end;
#if USE_SEGMENTED_TEXT_BLOCK_WRITER
  int text_subtype_7_end;
  int text_subtype_8_end;
  int text_subtype_9_end;
#endif

  TTY_TRACE_CR(("Calculating offsets ..."));

  visit_all_objects(&offset_finder, 0 JVM_CHECK);
  text_subtype_0_end = offset_finder.get_text_count();
  _data_block_scanned_count = offset_finder.get_data_count();
  _heap_block_permanent_count = offset_finder.get_heap_count();
#if USE_SEGMENTED_TEXT_BLOCK_WRITER
  _pass_sizes[0] = text_subtype_0_end * sizeof(jint);
#endif

  visit_all_objects(&offset_finder, 1 JVM_CHECK);
  text_subtype_1_end = offset_finder.get_text_count();
#if USE_SEGMENTED_TEXT_BLOCK_WRITER
  _pass_sizes[1] = (text_subtype_1_end - text_subtype_0_end) * sizeof(jint);
#endif

  visit_all_objects(&offset_finder, 2 JVM_CHECK);
  text_subtype_2_end = offset_finder.get_text_count();
#if USE_SEGMENTED_TEXT_BLOCK_WRITER
  _pass_sizes[2] = (text_subtype_2_end - text_subtype_1_end) * sizeof(jint);
#endif

  visit_all_objects(&offset_finder, 3 JVM_CHECK);
  text_subtype_3_end = offset_finder.get_text_count();
#if USE_SEGMENTED_TEXT_BLOCK_WRITER
  _pass_sizes[3] = (text_subtype_3_end - text_subtype_2_end) * sizeof(jint);
#endif

  visit_all_objects(&offset_finder, 4 JVM_CHECK);
  text_subtype_4_end = offset_finder.get_text_count();
#if USE_SEGMENTED_TEXT_BLOCK_WRITER
  _pass_sizes[4] = (text_subtype_4_end - text_subtype_3_end) * sizeof(jint);
#endif

  visit_all_objects(&offset_finder, 5 JVM_CHECK);
  text_subtype_5_end = offset_finder.get_text_count();
#if USE_SEGMENTED_TEXT_BLOCK_WRITER
  _pass_sizes[5] = (text_subtype_5_end - text_subtype_4_end) * sizeof(jint);
#endif

  visit_all_objects(&offset_finder, 6 JVM_CHECK);
  text_subtype_6_end = offset_finder.get_text_count();
#if USE_SEGMENTED_TEXT_BLOCK_WRITER
  _pass_sizes[6] = (text_subtype_6_end - text_subtype_5_end) * sizeof(jint);
#endif

  visit_all_objects(&offset_finder, 7 JVM_CHECK);
#if USE_SEGMENTED_TEXT_BLOCK_WRITER
  text_subtype_7_end = offset_finder.get_text_count();
  _pass_sizes[7] = (text_subtype_7_end - text_subtype_6_end) * sizeof(jint);
#endif

  visit_all_objects(&offset_finder, 8 JVM_CHECK);
#if USE_SEGMENTED_TEXT_BLOCK_WRITER
  text_subtype_8_end = offset_finder.get_text_count();
  _pass_sizes[8] = (text_subtype_8_end - text_subtype_7_end) * sizeof(jint);
#endif

  visit_all_objects(&offset_finder, 9 JVM_CHECK);
#if USE_SEGMENTED_TEXT_BLOCK_WRITER
  text_subtype_9_end = offset_finder.get_text_count();
  _pass_sizes[9] = (text_subtype_9_end - text_subtype_8_end) * sizeof(jint);
#endif

  _text_block_count = offset_finder.get_text_count();
  _data_block_count = offset_finder.get_data_count();
  _heap_block_count = offset_finder.get_heap_count();
#if ENABLE_PREINITED_TASK_MIRRORS && ENABLE_ISOLATES 
  _tm_block_count =   offset_finder.get_task_mirrors_count(); 
  int rom_tm_bitmap_size = _tm_block_count / BitsPerWord + 1; 
  *rom_tm_bitmap() = Universe::new_int_array(rom_tm_bitmap_size  JVM_CHECK); 
#endif 
  _symbols_start = text_subtype_1_end - 1;
  
#if USE_SEGMENTED_TEXT_BLOCK_WRITER
  int max_skip_words = 0;
#else
  int max_skip_words = sizeof(OopDesc)/BytesPerWord;
#endif

  write_subtype_range("methods", _method_start_skip, text_subtype_0_end,
                      text_subtype_1_end);
#if USE_SEGMENTED_TEXT_BLOCK_WRITER
  write_subtype_range("symbols", max_skip_words, text_subtype_1_end, 
                      text_subtype_2_end);
#else
  write_subtype_range("symbols", max_skip_words, text_subtype_1_end, 
                      text_subtype_4_end);
#endif
  write_subtype_range("fieldtype_symbols", max_skip_words, text_subtype_2_end,
                      text_subtype_3_end);
  write_subtype_range("signature_symbols", max_skip_words, text_subtype_3_end,
                      text_subtype_4_end);
  write_subtype_range("stackmap_entries", max_skip_words, text_subtype_4_end,
                      text_subtype_5_end);
  write_subtype_range("string_entries", max_skip_words, text_subtype_5_end,
                      text_subtype_6_end);
}


void SourceROMWriter::write_subtype_range(const char *name, 
                                          int max_skip_header_words,
                                          int start, int end)
{
  // Note the (end-1) so that we can do start <= p <= end. See
  // comments in OopDesc::is_rom_symbol().
  int start_offset = start     * sizeof(int);
  int end_offset   = (end - 1) * sizeof(int);

  if (start_offset == 0) {
    max_skip_header_words = 0;
  }
  start_offset -= max_skip_header_words * sizeof(int);
  end_offset   -= max_skip_header_words * sizeof(int);

  _reloc_stream.print("const int* _rom_%s_start = (const int*)(", name);
  write_reference(TEXT_BLOCK, start_offset, TEXT_BLOCK, &_reloc_stream);
  _reloc_stream.print_cr(");");

  _reloc_stream.print("const int* _rom_%s_end =   (const int*)(", name);
  write_reference(TEXT_BLOCK, end_offset,   TEXT_BLOCK, &_reloc_stream);
  _reloc_stream.print_cr(");");

  _reloc_stream.cr();
}

void SourceROMWriter::write_text_block(SourceObjectWriter* obj_writer
                                       JVM_TRAPS) {

  tty->print_cr("Writing TEXT block ...");
  obj_writer->start_block(TEXT_BLOCK, _text_block_count JVM_CHECK);
  print_separator("TEXT pass 0");
  visit_all_objects(obj_writer, 0 JVM_CHECK);
  print_separator("TEXT pass 1");
  visit_all_objects(obj_writer, 1 JVM_CHECK);
  print_separator("TEXT pass 2");
  visit_all_objects(obj_writer, 2 JVM_CHECK);
  print_separator("TEXT pass 3");
  visit_all_objects(obj_writer, 3 JVM_CHECK);
  print_separator("TEXT pass 4");
  visit_all_objects(obj_writer, 4 JVM_CHECK);
  print_separator("TEXT pass 5");
  visit_all_objects(obj_writer, 5 JVM_CHECK);
  print_separator("TEXT pass 6");
  visit_all_objects(obj_writer, 6 JVM_CHECK);
  print_separator("TEXT pass 7");
  visit_all_objects(obj_writer, 7 JVM_CHECK);
  print_separator("TEXT pass 8");
  visit_all_objects(obj_writer, 8 JVM_CHECK);
  print_separator("TEXT pass 9");
  visit_all_objects(obj_writer, 9 JVM_CHECK);
  obj_writer->end_block(JVM_SINGLE_ARG_CHECK);
}

void SourceROMWriter::write_data_body(SourceObjectWriter* obj_writer 
                                      JVM_TRAPS) {
  tty->print_cr("Writing DATA block ...");
  obj_writer->start_block(DATA_BLOCK, _data_block_count JVM_CHECK);
  print_separator("DATA pass 0");
  visit_all_objects(obj_writer, 0 JVM_CHECK);
  print_separator("DATA pass 1");
  visit_all_objects(obj_writer, 1 JVM_CHECK);
  obj_writer->end_block(JVM_SINGLE_ARG_CHECK);
}

void SourceROMWriter::write_data_block(SourceObjectWriter* obj_writer 
                                       JVM_TRAPS) {
  write_segment_header();
  write_data_body(obj_writer JVM_CHECK);
  write_segment_footer();
}
#if ENABLE_PREINITED_TASK_MIRRORS && ENABLE_ISOLATES 
void SourceROMWriter::write_tm_body(SourceObjectWriter* obj_writer 
                                      JVM_TRAPS) {
  tty->print_cr("Writing TASK MIRRORS block ...");
  obj_writer->start_block(TASK_MIRRORS_BLOCK, _tm_block_count JVM_CHECK);
  visit_all_objects(obj_writer, 0 JVM_CHECK);
  obj_writer->end_block(JVM_SINGLE_ARG_CHECK);
}

void SourceROMWriter::write_tm_block(SourceObjectWriter* obj_writer 
                                       JVM_TRAPS) {
  write_segment_header();
  write_tm_body(obj_writer JVM_CHECK);
  write_segment_footer();
}
#endif
void SourceROMWriter::write_heap_body(SourceObjectWriter* obj_writer 
                                      JVM_TRAPS) {
  tty->print_cr("Writing HEAP block ...");
  obj_writer->start_block(HEAP_BLOCK, _heap_block_count JVM_CHECK);
  print_separator("HEAP pass 0");
  visit_all_objects(obj_writer, 0 JVM_CHECK);
  print_separator("HEAP pass 1");
  visit_all_objects(obj_writer, 1 JVM_CHECK);
  obj_writer->end_block(JVM_SINGLE_ARG_CHECK);
}

void SourceROMWriter::write_heap_block(SourceObjectWriter* obj_writer 
                                       JVM_TRAPS) {
  write_segment_header();
  write_heap_body(obj_writer JVM_CHECK);
  write_segment_footer();
}

void SourceROMWriter::write_stuff_body(SourceObjectWriter* obj_writer 
                                       JVM_TRAPS) {
#if ENABLE_PREINITED_TASK_MIRRORS && ENABLE_ISOLATES 
  write_tm_block(obj_writer JVM_CHECK);
#endif
  // (2) Write the persistent handles and system symbols
  write_persistent_handles(obj_writer JVM_CHECK);  

  write_system_symbols(obj_writer JVM_CHECK);

  // (3) Write the ROM symbol and string table
  print_separator("Symbols");
  write_symbol_table(JVM_SINGLE_ARG_CHECK);

  print_separator("Strings");
  write_string_table(JVM_SINGLE_ARG_CHECK);

  // (4) Write original_class_info table that described renamed methods.
  print_separator("Original class info");
  write_original_class_info_table(JVM_SINGLE_ARG_CHECK);

  // (5) Restricted packages.
  print_separator("Restricted packages info");
  write_restricted_packages();

  // (6) Global singletons
  print_separator("Global singletons");
  write_global_singletons(JVM_SINGLE_ARG_CHECK);

  // (7) Check if we're linking the right ROMImage.o
  write_link_checks();

  // (8) klass table -- type info for objects in TEXT
  print_separator("Text Klass Table");
  write_text_klass_table(JVM_SINGLE_ARG_CHECK);

  // (9) Method variable parts
  print_separator("Variable parts");
  main_stream()->print_cr("const int _rom_number_of_java_classes = %d;",
                   number_of_romized_java_classes());

  // (11) romgen_oopmap_check()
  Generator::generate_oopmap_checks("romgen", main_stream());

#if ENABLE_COMPILER && ENABLE_APPENDED_CALLINFO
  // (12) Compiled method table
  print_separator("Compiled Method Table");
  write_compiled_method_table(JVM_SINGLE_ARG_CHECK);
#endif

#if ENABLE_MEMBER_HIDING
  print_separator("Member hiding tables");
  main_stream()->print_cr("#if ENABLE_MEMBER_HIDING");
  write_hidden_members(JVM_SINGLE_ARG_CHECK);
  main_stream()->print_cr("#endif // ENABLE_MEMBER_HIDING");
#endif

#if ENABLE_MULTIPLE_PROFILES_SUPPORT
  print_separator("Profiles information");
  main_stream()->print_cr("#if ENABLE_MULTIPLE_PROFILES_SUPPORT");

  // (13) Hidden classes within profiles
  write_hidden_classes();

  // (14) Restricted packages within profiles
  write_restricted_in_profiles();

  main_stream()->print("#endif // ENABLE_MULTIPLE_PROFILES_SUPPORT\n\n");
#endif // ENABLE_MULTIPLE_PROFILES_SUPPORT

#if ENABLE_ROM_JAVA_DEBUGGER
  // Write out state of MakeROMDebuggable flag so we know whether or not
  // to access line/var tables in methods.  If -MakeROMDebuggable then we
  // skip the line/var header entry when streaming the method
  main_stream()->print_cr("const int _rom_is_debuggable = %d;", 
                   MakeROMDebuggable);
#endif
}

void SourceROMWriter::write_stuff_block(SourceObjectWriter* obj_writer
                                        JVM_TRAPS) {
  write_segment_header();
  write_stuff_body(obj_writer JVM_CHECK);
  write_consistency_checks();
  write_segment_footer();
}

void SourceROMWriter::finalize_streams() {
  _reloc_stream.print_cr("#endif // ROMIZING");
}

void SourceROMWriter::write_objects(JVM_SINGLE_ARG_TRAPS) {
  SourceObjectWriter obj_writer(&_declare_stream, main_stream(), &_reloc_stream,
                          &_optimizer);
  obj_writer.set_writer(this);
  obj_writer.print_entry_declarations();
  obj_writer.print_oopmap_declarations();

  write_includes();

  write_text_block(&obj_writer JVM_CHECK);
  write_data_block(&obj_writer JVM_CHECK);
  write_heap_block(&obj_writer JVM_CHECK);
  write_stuff_block(&obj_writer JVM_CHECK);

  finalize_streams();
}

void SourceROMWriter::visit_persistent_handles(JVM_SINGLE_ARG_TRAPS) {
  // Visit all persistent handlers, except the last NUM_HANDLES_SKIP handles.
  int count = Universe::__number_of_persistent_handles -
    Universe::NUM_HANDLES_SKIP;

  Oop visiting_object;
  for (int index = 0; index < count; index++) {
    visiting_object = (persistent_handles[index]);
    visit_object(&visiting_object, NULL JVM_CHECK);
  }
  while (has_pending_object()) {
    visiting_object = remove_pending_object();
    visit_object(&visiting_object, NULL JVM_CHECK);
  }
}

void SourceROMWriter::print_separator(const char * section) {
  main_stream()->print_cr("\n\n/* ==== %s starting ==== */", section);
}


void SourceROMWriter::write_reference(Oop* oop, BlockType current_type,
                                      FileStream *stream JVM_TRAPS) {
  int oop_offset = offset_of(oop JVM_CHECK);
  BlockType type = block_type_of(oop JVM_CHECK);
#if ENABLE_HEAP_NEARS_IN_HEAP
  /*
   *in these function we are converting oop to it's final position. In case it is 
   *object which is cloned we should determine what copy to use, HEAP or another.
   *ROM_DUPLICATE_HANDLES_BLOCK contains references to NON-HEAP copies.
   *HEAP and PERSISTENT_HANDLES_BLOCK blocks contain references to HEAP copies, while TEXT and DATA
   *points to another copy
  */
  if (current_type == ROMWriter::ROM_DUPLICATE_HANDLES_BLOCK) {
    GUARANTEE(oop->obj()->is_near(), "only nears shall be in this block!");
    if (type == ROMWriter::TEXT_AND_HEAP_BLOCK) {
      type = ROMWriter::TEXT_BLOCK;
    } else if (type == ROMWriter::DATA_AND_HEAP_BLOCK) {
      type = ROMWriter::DATA_BLOCK;
    } else {
      GUARANTEE(type == ROMWriter::HEAP_BLOCK, "nears should be in HEAP!")
    }
  } else if (type == ROMWriter::TEXT_AND_HEAP_BLOCK || type == ROMWriter::DATA_AND_HEAP_BLOCK) {
    GUARANTEE(oop->obj()->is_near(), "only nears shall be in this block!");
    if (current_type == ROMWriter::HEAP_BLOCK || current_type == ROMWriter::PERSISTENT_HANDLES_BLOCK) {
      type = ROMWriter::HEAP_BLOCK;
      oop_offset = heap_offset_of(oop JVM_CHECK);
    } else {
      type = (type == ROMWriter::TEXT_AND_HEAP_BLOCK) ? ROMWriter::TEXT_BLOCK : ROMWriter::DATA_BLOCK;
    }
  }
#endif
  write_reference(type, oop_offset, current_type, stream);
}

void SourceROMWriter::write_general_reference(FileStream* stream, 
                                              int offset, 
                                              int delta, 
                                              const char* block_name) {
  if (delta & 0x03) {
    stream->print("%sb(0x%08x * 4 + %d)", block_name, 
      (unsigned int)(offset/sizeof(int)), delta);
  } else {
    stream->print("%s(0x%08x + %d)", block_name,
      (unsigned int)(offset/sizeof(int)), delta/4);
  }
}

void SourceROMWriter::write_compiled_text_reference(FileStream* stream,
                                                    int offset, 
                                                    int delta) {
  write_general_reference(stream, offset, delta, "TEXT");
}

void SourceROMWriter::write_method_comments(FileStream* stream,
                                            CompiledMethod* cm) {
  if (GenerateROMComments) {
    stream->print(" /* ");
    cm->print_value_on(stream);
    stream->print_cr("*/");
  }
}

void SourceROMWriter::write_compiled_code_reference(CompiledMethod* cm, 
                                                    FileStream *stream,
                                                    bool as_execution_entry
                                                    JVM_TRAPS)
{
  BlockType type = block_type_of(cm JVM_CHECK);
  int offset = offset_of(cm JVM_CHECK);
  int delta = CompiledMethod::entry_offset();

  if (as_execution_entry) {
#if ENABLE_THUMB_COMPILER
    // The low bit is set to 0x1 so that BX will automatically switch into
    // THUMB mode.
    delta += 1;
#endif
  }
  
  switch (type) {
  case ROMWriter::TEXT_BLOCK:
    write_compiled_text_reference(stream, offset, delta);
    break;
  case ROMWriter::DATA_BLOCK:
    write_general_reference(stream, offset, delta, "DATA");
    break;
  default:
    // AOT methods cannot live in heap
    SHOULD_NOT_REACH_HERE();
  }
  write_method_comments(stream, cm);
}

void SourceROMWriter::write_compiled_method_reference(CompiledMethod* cm, 
                                                      BlockType type,
                                                      FileStream *stream
                                                      JVM_TRAPS)
{
  const int offset = offset_of(cm JVM_CHECK);
  write_reference(type, offset, type, stream);
  write_method_comments(stream, cm);
}

void SourceROMWriter::write_text_reference(FileStream* stream, int offset) {
  stream->print("TEXT(0x%08x)", (unsigned int)(offset/sizeof(int)));
}

void SourceROMWriter::write_reference(BlockType type, int offset,
                                BlockType current_type, FileStream *stream) {
  GUARANTEE((offset % sizeof(int)) == 0, "sanity");
  switch (type) {
  case ROMWriter::TEXT_BLOCK:
    write_text_reference(stream, offset);
    break;
  case ROMWriter::DATA_BLOCK:
    stream->print("DATA(0x%08x)", (unsigned int)(offset/sizeof(int)));
    break;
  case ROMWriter::HEAP_BLOCK:
    GUARANTEE(current_type != ROMWriter::TEXT_BLOCK,
              "TEXT block may not contain HEAP pointers");
    stream->print("HEAP(0x%08x)", (unsigned int)(offset/sizeof(int)));
    break;
#if ENABLE_PREINITED_TASK_MIRRORS && ENABLE_ISOLATES
   case ROMWriter::TASK_MIRRORS_BLOCK:
    if (current_type == ROMWriter::PERSISTENT_HANDLES_BLOCK) {
      //this is mirror list. shall not be referenced via persistent handles
      //it will be loaded separately
      write_plain_int(0, stream);
    } else {
      //such references must be written via write_tm_reference
      SHOULD_NOT_REACH_HERE();
    }
    break;     
#endif
  default:
    SHOULD_NOT_REACH_HERE();
  }
}

void SourceROMWriter::write_plain_int(jint value, FileStream *stream) {
  char buff[20];
  jvm_sprintf(buff, "0x%08x", value);
  GUARANTEE(jvm_strlen(buff) == 10, "sprintf check");
  stream->print(buff);
}

void SourceROMWriter::write_int(jint value, FileStream *stream) {
  char buff[20];
  jvm_sprintf(buff, "     0x%08x", value);
  GUARANTEE(jvm_strlen(buff) == 15, "sprintf check");
  stream->print(buff);
}

void SourceROMWriter::write_double(jint msw, jint lsw, FileStream *stream) {
  stream->print("ROM_DOUBLE(0x%08x, 0x%08x)", msw, lsw);
}

void SourceROMWriter::write_long(jint msw, jint lsw, FileStream *stream) {
  stream->print("ROM_LONG(0x%08x, 0x%08x)", msw, lsw);
}

void SourceROMWriter::write_null(FileStream *stream) {
  const char *buff = "   /* NULL */ 0" ;
  GUARANTEE(jvm_strlen(buff) == 15, "sprintf check");
  stream->print(buff);
}

// Return the number of bytes written
int SourceROMWriter::write_rom_hashtable(const char *table_name,
                                         const char *element_name,
                                         ObjArray *table, 
                                         ConstantPool *embedded_holder,
                                         int embedded_offset JVM_TRAPS)
{
  int written_bytes;
  written_bytes = print_rom_hashtable_header(table_name, element_name, table, 
                                             embedded_holder, embedded_offset
                                             JVM_CHECK_0);
  
  if (embedded_holder->is_null()) {
    written_bytes = print_rom_hashtable_content(element_name, table JVM_CHECK_0);
  } else if (GenerateROMComments) {
    // print the table in a commented block for debugging purpose.
    main_stream()->cr();
    main_stream()->print_cr("#if 0");
    main_stream()->print_cr("/* Unused */");
    main_stream()->print_cr("/* Actual table is embedded inside constant pool */");
    print_rom_hashtable_content(element_name, table JVM_CHECK_0);
    main_stream()->cr();
    main_stream()->print_cr("#endif");
  }


  main_stream()->cr();
  main_stream()->print_cr("};");
  main_stream()->cr();

#if USE_ROM_LOGGING
  if (strcmp(element_name, "STRG") == 0) {
    int num_buckets = table->length();
    _optimizer_log_stream.print_cr("\n[String Table]\n");

    for (int b = 0; b<num_buckets; b++) {
      ObjArray::Raw bucket = table->obj_at(b);
      const int bucket_size = bucket().length();
      if (bucket_size == 0) {
        continue;
      }
      for( int index = 0; index < bucket_size; index++ ) {
        String::Raw string = bucket().obj_at(index);
        _optimizer_log_stream.print("romstring = \"");
        string().print_string_on(&_optimizer_log_stream);
        _optimizer_log_stream.print_cr("\"");
      }
    }
  }
#endif

  return written_bytes;
}

int SourceROMWriter::print_rom_hashtable_header(const char *table_name,
                                                const char *element_name,
                                                ObjArray *table, 
                                                ConstantPool *embedded_holder,
                                                int embedded_offset JVM_TRAPS)
{
  int num_buckets = table->length();
  main_stream()->cr();
  main_stream()->print_cr("#undef BUCKET");

  if (embedded_holder->not_null()) {
    // The table is embedded inside a constant pool
    int text_offset = offset_of(embedded_holder JVM_CHECK_0);
    GUARANTEE(text_offset >= 0, "embedded_holder must be included in ROM");
#if USE_SEGMENTED_TEXT_BLOCK_WRITER
    int obj_pass = pass_of(embedded_holder JVM_CHECK_0);
    for(int i=0; i<obj_pass; i++) {
      text_offset -= _pass_sizes[i];
    }
#endif
    text_offset  /= BytesPerWord;
    text_offset += embedded_holder->base_offset() / BytesPerWord;
    text_offset += embedded_offset;
#if USE_SEGMENTED_TEXT_BLOCK_WRITER
    {
    main_stream()->print_cr("#define BUCKET(x) TEXT%d(%d + (x))", obj_pass, 
                      text_offset);
    }
#else
    main_stream()->print_cr("#define BUCKET(x) TEXT(%d + (x))", text_offset);
#endif
  } else {
    main_stream()->print_cr("#define BUCKET(x) (const int)&_rom_%s[x]", table_name);
  }
  write_text_undefines(main_stream());
  main_stream()->print_cr("#undef DATA");
  
  write_text_defines(main_stream());
  main_stream()->print_cr("#define DATA(x) (const int)&_rom_data_block[x]");

  main_stream()->print_cr("const int _rom_%s_num_buckets = %d;", table_name,
                   num_buckets);
  main_stream()->print   ("const int _rom_%s[] = {\n\t", table_name);

  // Print the indices to the buckets
  int bytes_written = 0;
  int bucket_start;

  if (embedded_holder->not_null()) {
    bucket_start = 0;
  } else {
    bucket_start = num_buckets + 1; // plus marker for end of the last bucket
  }

  ObjArray bucket;
  for (int b=0; b<num_buckets; b++) {
    if (b > 0 && (b % 4) == 0) {
      main_stream()->print("\n\t");
    }
    bucket = table->obj_at(b);
    main_stream()->print("BUCKET(%d), ", bucket_start);

    bucket_start += bucket.length();
    bytes_written += BytesPerWord;
  }

  // marker for the end of the last bucket
  //
  // Note that you'd see one more "BUCKET()" than the value of
  // _rom_{string,symbol}_table_num_buckets.  See comments in
  // SymbolTable.cpp for how the BUCKET() items are used in searching
  // for ROMized symbols/strings.
  main_stream()->print("\n/*END*/ ");
  main_stream()->print("BUCKET(%d), ", bucket_start);

  bytes_written += BytesPerWord;

  return bytes_written;
}

int
SourceROMWriter::print_rom_hashtable_content(const char *element_name,
                                             ObjArray *table
                                             JVM_TRAPS)
{
  int num_buckets = table->length();
  int bytes_written = 0;
  main_stream()->cr();

  UsingFastOops level1;
  ObjArray::Fast bucket;
  Oop::Fast oop;

  for (int b = 0; b<num_buckets; b++) {
    bucket = table->obj_at(b);
    if (bucket().length() == 0) {
      continue;
    }
    main_stream()->print("\n/* %d [%d] */\n\t", b, bytes_written/4);

    const int bucket_size = bucket().length();
    for (int index = 0; index < bucket_size; index++) {
      oop = bucket().obj_at(index);
      GUARANTEE(!oop.is_null(), "sanity");
      write_reference(&oop, TEXT_BLOCK, main_stream() JVM_CHECK_0);
      main_stream()->print(", ");
      if (GenerateROMComments) {
        main_stream()->print("/%s ", "*");
        oop().print_value_on(main_stream());
        if (oop().is_symbol()) {
          Symbol::Raw s = oop.obj();
          main_stream()->print(" hash=0x%x", s().hash());
        } else if (oop.is_string()) {
          String::Raw s = oop.obj();
          main_stream()->print(" hash=0x%x", s().hash());
        }
        main_stream()->print(" */\n\t");
      } else {
        if ((index % 4) == 3) {
          main_stream()->print("\n\t");
        }
      }
       bytes_written += 4;
    }
  }

  return bytes_written;
}

void SourceROMWriter::write_original_info_strings(JVM_SINGLE_ARG_TRAPS) {
  // This function is necessary because Java method.field names, as well
  // as their signatures, may contain arbitrary characters.
  // if all C++ compilers likes the syntax "foo\x12" for including
  // the ASCII value 0x12 into a literal string. So, we write all these
  // constant strings as char arrays. E.g.:
  //
  // const char _rom_str123[] = {3,'f','o','o',(char)0x12,0};
  UsingFastOops level1;
  ObjArray::Fast minfo_list = _optimizer.romizer_original_method_info()->obj();
  ObjArray::Fast finfo_list = _optimizer.romizer_original_fields_list()->obj();
  ConstantPool::Fast orig_cp = _optimizer.romizer_alternate_constant_pool()->obj();
  int i;
  int class_count = number_of_romized_java_classes();

  // (1) The constant strings for the renamed fields  
  Symbol::Fast symbol;
  for (i=0; ; i++) {
    const ConstantTag tag = orig_cp().tag_at(i);
    if (tag.is_invalid()) {
      break;
    }
    symbol = orig_cp().symbol_at(i);
    write_constant_string(&symbol JVM_CHECK);
  }

  // (2) The constant strings for the method info
  ObjArray::Fast info;
  Method::Fast method;
  Symbol::Fast name;
  for (i = 0; i < class_count; i++) {
    info = minfo_list().obj_at(i);
    while (!info.is_null()) {
      method = info().obj_at(ROM::INFO_OFFSET_METHOD);
      name   = info().obj_at(ROM::INFO_OFFSET_NAME);

      const int offset = offset_of(&method JVM_CHECK);
      if (offset == -1) {
        // The method has been removed from the system entirely. No
        // need to put it inside the original info table.
      } else {
        write_constant_string(&name JVM_CHECK);
      }
      info = info().obj_at(ROM::INFO_OFFSET_NEXT);
    }
  }

  // (3) The constant strings for the original class names
  Symbol::Fast orig_name;
  for (i=0; i<class_count; i++) {
    JavaClass::Raw klass = Universe::class_from_id(i);
    if (klass.not_null() && klass().is_instance_class()) {
      const InstanceClass::Raw ic = klass.obj();
      name = ic().name();
      if (name().equals(Symbols::unknown())) {
        orig_name = ic().original_name();
        write_constant_string(&orig_name JVM_CHECK);
      }
    }
  }
}

void SourceROMWriter::write_original_class_info_table(JVM_SINGLE_ARG_TRAPS) {
  UsingFastOops level1;
  ObjArray::Fast minfo_list = _optimizer.romizer_original_method_info()->obj();
  ObjArray::Fast finfo_list = _optimizer.romizer_original_fields_list()->obj();
  ConstantPool::Fast orig_cp = _optimizer.romizer_alternate_constant_pool()->obj();
  int i;
  int class_count = number_of_romized_java_classes();

  // Enable the original info output in Romized files.
  main_stream()->print_cr(
   "#if ENABLE_ROM_DEBUG_SYMBOLS || ENABLE_JVMPI_PROFILE || ENABLE_TTY_TRACE");

  write_original_info_strings(JVM_SINGLE_ARG_CHECK);

  // (1) Print ROM::_alternate_constant_pool
  main_stream()->print_cr("const char* const _rom_alternate_constant_pool_src[] = {");
  for (i=0; ; i++) {
    const ConstantTag tag = orig_cp().tag_at(i);
    if (tag.is_invalid()) {
      break;
    }
    main_stream()->print("\t");
    Symbol::Raw symbol = orig_cp().symbol_at(i);
    write_constant_string_ref(&symbol);
    main_stream()->print_cr(",");
  }
  main_stream()->print_cr("0");
  main_stream()->print_cr("};");
  main_stream()->print_cr("const int _rom_alternate_constant_pool_count = %d;", i);
  main_stream()->cr();

  // (2) Print the individual OriginalMethodInfo's
  ObjArray::Fast info;
  Method::Fast method;
  Symbol::Fast name;
  for (i=0; i<class_count; i++) {
    info = minfo_list().obj_at(i);
    if (info.is_null()) {
      // no renamed methods
      continue;
    }

    if (GenerateROMComments) {
      main_stream()->print("\n/* ");
      InstanceClass::Raw ic = Universe::class_from_id(i);
      ic().print_name_on(main_stream());
      main_stream()->print_cr("*/");
    }

    main_stream()->print_cr("static const OriginalMethodInfo method_info_%d[] = {",i);
    int count = 0;
    while (!info.is_null()) {
      method = info().obj_at(ROM::INFO_OFFSET_METHOD);
      name   = info().obj_at(ROM::INFO_OFFSET_NAME);

      int offset = offset_of(&method JVM_CHECK);
      if (offset == -1) {
        // The method has been removed from the system entirely. No
        // need to put it inside the original info table.
      } else {
        main_stream()->print("\t{");
        write_reference(&method, TEXT_BLOCK, main_stream() JVM_CHECK);
        main_stream()->print_cr(",");

        main_stream()->print("\t ");
        write_constant_string_ref(&name);
        main_stream()->print_cr("},");
        count ++;
      }
      info = info().obj_at(ROM::INFO_OFFSET_NEXT);
    }
    if (count == 0) {
      main_stream()->print_cr("{0, 0}");
    }
    main_stream()->print_cr("};");
  }

  // (3) Print the individual OriginalFieldInfo's
  for (i=0; i<finfo_list().length(); i++) {
    TypeArray::Raw fields = finfo_list().obj_at(i);

    if (fields.is_null() || fields().length() == 0) {
      // no renamed fields
      continue;
    }

    if (GenerateROMComments) {
      InstanceClass::Raw ic = Universe::class_from_id(i);
      main_stream()->print("\n/* ");
      ic().print_name_on(main_stream());
      main_stream()->print_cr("*/");
    }

    main_stream()->print_cr("static const OriginalFieldInfo field_info_%d[] = {",i);
    for (int idx=0; idx<fields().length(); idx+=5) {
      jushort flags     = fields().ushort_at(idx + Field::ACCESS_FLAGS_OFFSET);
      jushort offset    = fields().ushort_at(idx + Field::OFFSET_OFFSET);
      jushort name_index= fields().ushort_at(idx + Field::NAME_OFFSET);
      jushort sig_index = fields().ushort_at(idx + Field::SIGNATURE_OFFSET);

      main_stream()->print("\t{%d, %d, %d, %d},", 
                       flags, offset, name_index, sig_index);

      if (GenerateROMComments) {
        Symbol::Raw name = orig_cp().symbol_at(name_index);
        Symbol::Raw sig  = orig_cp().symbol_at(sig_index);

        main_stream()->print("\t/* ");
        name().print_symbol_on(main_stream());
        main_stream()->print(": ");
        sig().print_symbol_on(main_stream());
        main_stream()->print(" */");
      }
      main_stream()->cr();
    }
    main_stream()->print_cr("};");
  }

  // (4) Print the combined OriginalClassInfo array
  main_stream()->print_cr("const OriginalClassInfo _rom_original_class_info[] = {");
  JavaClass::Fast klass;
  Symbol::Fast orig_name;
  ObjArray::Fast minfo;
  for (i=0; i<class_count; i++) {
    main_stream()->print(" {");

    // (a) original name
    klass = Universe::class_from_id(i);
    bool class_renamed = false;
    if (klass.not_null() && klass().is_instance_class()) {
      InstanceClass::Raw ic = klass.obj();
      name = ic().name();
      if (name.equals(Symbols::unknown())) {
        orig_name = ic().original_name();
        write_constant_string_ref(&orig_name);
        class_renamed = true;
      }
    }
    if (!class_renamed) {
      main_stream()->print("(const char*)0");
    }
    main_stream()->print(", ");

    // (b) original methods
    minfo = minfo_list().obj_at(i);
    if (minfo.is_null()) {
      // no renamed methods
      main_stream()->print(" 0, (const OriginalMethodInfo *)0x0, ");
    } else {
      int num = 0;
      while (!minfo.is_null()) {
        method = minfo().obj_at(ROM::INFO_OFFSET_METHOD);
        int offset = offset_of(&method JVM_CHECK);
        if (offset != -1) {
          num ++;
        }
        minfo = minfo().obj_at(ROM::INFO_OFFSET_NEXT);
      }

      main_stream()->print(" %d, method_info_%d, ", num, i);
    }

    // (c) original fields
    TypeArray::Raw fields = finfo_list().obj_at(i);
    if (fields.is_null() || fields().length() == 0) {
      // no renamed fields
      main_stream()->print("0, (const OriginalFieldInfo *)0x0");
    } else {
      main_stream()->print("%d, field_info_%d", fields().length(), i);
    }
    main_stream()->print("},\n\t");
  }

  main_stream()->print_cr("};");

  main_stream()->print_cr("const int _rom_original_class_info_count = %d;",
                   class_count);

  main_stream()->print_cr("#endif /* ENABLE_ROM_DEBUG_SYMBOLS */");
}


void SourceROMWriter::write_constant_string(Symbol* s JVM_TRAPS) {
  if (constant_string_table()->exists(s)) {
    return;
  }
  int n = _constant_string_count ++;
  constant_string_table()->put(s JVM_CHECK);
  constant_string_table()->set_int_attribute(s, 0, n JVM_CHECK);

  int c;
  if (s->is_valid_method_signature(NULL)) {
    c = '1';
  } else if (s->is_valid_field_type()) {
    c = '2';
  } else {
    c = '3';
  }
  main_stream()->print("static const char _rom_str%d[] = {(char)0x%c,", n, c);
  s->print_as_c_array_on(main_stream());
  main_stream()->print_cr("0};");
}

void SourceROMWriter::write_constant_string_ref(Symbol* s) {
  const int n = constant_string_table()->get_int_attribute(s, 0);
  main_stream()->print("(const char*)_rom_str%d", n);
  if (GenerateROMComments) {
    main_stream()->print(" /* ");
    s->print_symbol_on(main_stream());
    main_stream()->print(" */");
  }
}

// Write the restricted packages in a packed string table. Each string
// be shorter than 256 bytes. We first write the length, then the string
// body. The end of the table is delimited by a length of 0.
void SourceROMWriter::print_packages_list(ROMVector* patterns) {
  GUARANTEE(patterns->not_null(), "Sanity");

  int num_bytes = 1;  // Terminating 0
  int num_pkgs = 0;

  const int patterns_count = patterns->size();  
  for( int pat = 0; pat < patterns_count; pat++ ) {  
    Symbol::Raw package( patterns->element_at(pat) );
    const int len = package().length();
    if( len ) {
      GUARANTEE(len < 256, "sanity");
      num_bytes += len+1; // Length + characters
      num_pkgs ++;
      main_stream()->print("\n  0x%02x,", len);
      for( int i = 0; i < len; i++ ) {
        main_stream()->print("'%c',", package().byte_at(i)); 
      }
    }
  } 
  main_stream()->print_cr("\n  0");

  MemCounter::restricted_pkgs().text_bytes   += num_bytes;
  MemCounter::restricted_pkgs().text_objects += num_pkgs;
  MemCounter::total().add_text(num_bytes);
}


void SourceROMWriter::write_restricted_packages( void ) {
  main_stream()->print("const char _rom_restricted_packages[] = {");
  print_packages_list(_optimizer.restricted_packages());
  main_stream()->print_cr("}; // _rom_restricted_packages");
}

#if ENABLE_MULTIPLE_PROFILES_SUPPORT
void SourceROMWriter::print_profile_name( const int profile_id ) {
  ROMProfile::Raw rom_profile = _optimizer.profiles_vector()->element_at(profile_id);
  Symbol::Raw profile_name = rom_profile().profile_name();
  profile_name().print_symbol_on(main_stream());
}

void SourceROMWriter::write_restricted_in_profiles() {
  ROMVector* const rom_profiles_table = _optimizer.profiles_vector();
  GUARANTEE(rom_profiles_table != NULL, "Sanity");
  const int profiles_count = rom_profiles_table->size();
  int p;

  for (p = 0; p < profiles_count; p++ ) {
    ROMProfile::Raw profile = rom_profiles_table->element_at(p);
    Symbol::Raw profile_name = profile().profile_name();
    GUARANTEE(profile_name.not_null(), "Sanity");

    main_stream()->print(
      "const char _rom_restricted_packages_%d[] = { // ", p);
    profile_name().print_symbol_on(main_stream());

    // Writing restricted packages list.
    {
      ROMVector::Raw patterns = profile().restricted_packages();
      print_packages_list(&patterns);
    }

    main_stream()->print("}; // ");
    profile_name().print_symbol_on(main_stream());
    main_stream()->cr();
    main_stream()->cr();
  }

  main_stream()->print_cr("const char* const _rom_profiles_restricted_packages[] = {");
  for (p = 0; p < profiles_count; p++ ) {
    main_stream()->print_cr("  _rom_restricted_packages_%d,", p);    
  }
  main_stream()->print_cr("}; // _rom_profiles_restricted_packages");

  main_stream()->cr();
}

// Writes hidden classes for specified profiles information.
void SourceROMWriter::write_hidden_classes(void) {
  ROMVector* rom_profiles_table = _optimizer.profiles_vector();
  GUARANTEE(rom_profiles_table != NULL, "Sanity");

  const int profiles_count = rom_profiles_table->size();
  main_stream()->print_cr("const int _rom_profiles_count = %d;\n", profiles_count);

  // Writing profiles names...
  {
    main_stream()->print_cr("const char* const _rom_profiles_names[] = {");
    for (int p = 0; p < profiles_count; p++) {
      main_stream()->print("  \"");
      print_profile_name(p);
      main_stream()->print_cr("\",");
    }
    main_stream()->print_cr("  0\n}; // _rom_profiles_names\n");
  }

  const int bitmap_row_base = ROMBitSet::range_start();
  const int bitmap_row_size = ROMBitSet::range_length();

  // Writing _rom_profile_base and _profile_bitmap_row_size
  main_stream()->print_cr(
    "const int _rom_profile_bitmap_row_base = %d;\n"
    "const int _rom_profile_bitmap_row_size = %d;\n",
    bitmap_row_base, bitmap_row_size);

  // Writing profiles bitmaps  
  main_stream()->print_cr(
      "const unsigned char _rom_hidden_classes_bitmaps[] = {");
  int total_size;
  if( bitmap_row_size != 0 ) {
    for (int profile_id = 0;; main_stream()->cr()) {
      main_stream()->print( "  // Profile " );
      print_profile_name(profile_id);
      main_stream()->cr();

      ROMProfile::Raw profile = rom_profiles_table->element_at(profile_id);
      ROMBitSet::Raw hidden_set = profile().hidden_set();
      hidden_set().print_class_names( main_stream(), "  // Hidden " );
      main_stream()->print("  ");
      hidden_set().print_bytes(main_stream(), bitmap_row_base, bitmap_row_size);
      main_stream()->cr();
      if( ++profile_id >= profiles_count ) {
        break;
      }
    }
    total_size = bitmap_row_size * profiles_count;
  } else {
    main_stream()->print_cr("  0 // No profile-local hidden classes" );
    total_size = 1;
  }
  main_stream()->print_cr("}; // _rom_hidden_classes_bitmaps\n");   

  MemCounter::hidden_classes().text_bytes   = total_size;
  MemCounter::hidden_classes().text_objects = profiles_count;
  MemCounter::total().add_text(total_size);
}
#endif // ENABLE_MULTIPLE_PROFILES_SUPPORT

#if ENABLE_MEMBER_HIDING
bool SourceROMWriter::is_hidden_field (const InstanceClass* ic,
                                              const OopDesc* field) {
  return !Symbols::unknown()->equals(field) &&
         ROMOptimizer::is_hidden_field(ic, field);
}

bool SourceROMWriter::is_hidden_method(const InstanceClass* ic,
                                              const Method* method) {
  return !Symbols::unknown()->equals(method->name()) &&
         ROMOptimizer::is_hidden_method(ic, method);
}

inline int SourceROMWriter::field_count (const InstanceClass* ic) {
  return ic->field_count();
}
inline int SourceROMWriter::method_count(const InstanceClass* ic) {
  return ic->method_count();
}

#if ENABLE_MULTIPLE_PROFILES_SUPPORT
inline ROMProfile* SourceROMWriter::global_profile(void) {
  return ROMOptimizer::global_profile();
}
#endif

bool SourceROMWriter::has_hidden_fields (const InstanceClass* ic) {
#if ENABLE_MULTIPLE_PROFILES_SUPPORT
  if (global_profile()->has_hidden_fields(ic)) {
    return true;
  }

  const ROMVector* rom_profiles = ROMOptimizer::profiles_vector();
  GUARANTEE(rom_profiles != NULL, "Sanity");

  const int profiles_count = rom_profiles->size();
  for (int i = 0; i < profiles_count; i++) {
    const ROMProfile::Raw profile = rom_profiles->element_at(i);
    if (profile().has_hidden_fields(ic)) {
      return true;
    }
  }
  return false;
#else // ENABLE_MULTIPLE_PROFILES_SUPPORT
  ConstantPool::Raw cp = ic->constants();
  const TypeArray::Raw fields = ic->fields();
  const int fields_length = fields().length();

  for (int i = 0; i < fields_length; i += Field::NUMBER_OF_SLOTS) {
    const jushort name_index = fields().ushort_at(i + Field::NAME_OFFSET);
    const OopDesc* field_name = cp().symbol_at(name_index);
    if (is_hidden_field(ic, field_name)) {
      return true;
    }
  }
  return false;
#endif // ENABLE_MULTIPLE_PROFILES_SUPPORT
}

bool SourceROMWriter::has_hidden_methods (const InstanceClass* ic) {
#if ENABLE_MULTIPLE_PROFILES_SUPPORT
  if (global_profile()->has_hidden_methods(ic)) {
    return true;
  }

  const ROMVector* rom_profiles = ROMOptimizer::profiles_vector();
  GUARANTEE(rom_profiles != NULL, "Sanity");

  const int profiles_count = rom_profiles->size();
  for (int i = 0; i < profiles_count; i++) {
    const ROMProfile::Raw profile = rom_profiles->element_at(i);
    if (profile().has_hidden_methods(ic)) {
      return true;
    }
  }
  return false;
#else // ENABLE_MULTIPLE_PROFILES_SUPPORT
  {
    const ObjArray::Raw methods = ic->methods();
    const int methods_length = methods().length();

    for (int i = 0; i < methods_length; i++) {
      const Method::Raw method = methods().obj_at(i);
      if (method.not_null() && is_hidden_method(ic, &method)) {
        return true;
      }
    }
  }
  {
    const jushort holder_id = ic->class_id();
    const ClassInfo::Raw info = ic->class_info();
    const int vtable_length = info().vtable_length();
    for (int i = 0; i < vtable_length; i++) {
      const Method::Raw method = info().vtable_method_at(i);
      if (method.not_null() && method().holder_id() == holder_id
                            && is_hidden_method(ic, &method)) {
        return true;
      }
    }
  }
  return false;
#endif // ENABLE_MULTIPLE_PROFILES_SUPPORT
}

inline void
SourceROMWriter::write_modified_class_index(const int min, const int max) {
  int modified_class_index = 0;
  for (int i = min; i <= max; i++) {
    int n = -1;
    JavaClass::Raw klass = Universe::class_from_id(i);
    if (klass.is_instance_class()) {
      const InstanceClass::Raw ic = klass.obj();
      if (!klass().access_flags().is_hidden()) {
        if (has_hidden_fields(&ic) || has_hidden_methods(&ic)) {
          n = modified_class_index++;
        }
      }
    }

    main_stream()->print("  %5d, // ", n);
    klass().print_name_on(main_stream());
    main_stream()->cr();
  }
}

inline int
SourceROMWriter::write_modified_class_attributes(const int min, const int max) {
  int bit_count = 0;
  for (SystemClassStream st; st.has_next();) {
    InstanceClass::Raw klass = st.next();
    {
      const int class_id = klass().class_id();
      if (class_id > max) break;
      if (class_id < min || klass().access_flags().is_hidden()) {
        continue;
      }
    }
    const bool hidden_fields = has_hidden_fields(&klass);
    const bool hidden_methods = has_hidden_methods(&klass);
    if (hidden_fields || hidden_methods) {
      int attr = bit_count;
      if (hidden_fields) {
        attr |= ROM::CLASS_HAS_HIDDEN_FIELDS;
        bit_count += field_count(&klass);
      }
      if (hidden_methods) {
        attr |= ROM::CLASS_HAS_HIDDEN_METHODS;
        bit_count += method_count(&klass);
      }
      main_stream()->print("  0x%08x, // ", attr);
      klass().print_name_on(main_stream());
      main_stream()->cr();
    }
  }
  return bit_count;
}

#if ENABLE_MULTIPLE_PROFILES_SUPPORT
inline void
SourceROMWriter::write_modified_class_bitmap(const int min, const int max,
                                             jubyte bitmap[],
                                             const ROMProfile* profile) {
  int bit_count = 0;
  for (SystemClassStream st; st.has_next();) {
    InstanceClass::Raw klass = st.next();
    const int class_id = klass().class_id();
    if (class_id > max) break;
    if (class_id < min || klass().access_flags().is_hidden()) {
      continue;
    }
    
    const bool has_hidden_fields = this->has_hidden_fields(&klass);
    const bool has_hidden_methods = this->has_hidden_methods(&klass);
    if (!has_hidden_fields && !has_hidden_methods) {
      continue;
    }

    const bool hidden_fields =
      global_profile()->has_hidden_fields(&klass) ||
      profile->has_hidden_fields(&klass);
    const bool hidden_methods =
      global_profile()->has_hidden_methods(&klass) ||
      profile->has_hidden_methods(&klass);
    if (hidden_fields || hidden_methods) {
      main_stream()->print("/*\n  ");
      klass().print_name_on(main_stream());
      main_stream()->cr();
    };
    if (hidden_fields) {
      main_stream()->print_cr("    Hidden fields:");
      ConstantPool::Raw cp = klass().constants();
      const TypeArray::Raw fields = klass().fields();
      const int fields_length = fields().length();
      for (int i = 0; i < fields_length; i += Field::NUMBER_OF_SLOTS) {
        const jushort name_index = fields().ushort_at(i + Field::NAME_OFFSET);
        OopDesc* field_name = cp().symbol_at(name_index);
        if (global_profile()->is_hidden_field(&klass, field_name) ||
            profile->is_hidden_field(&klass, field_name)) {
          main_stream()->print("      ");
          Symbol::Raw name = field_name;
          name().print_symbol_on(main_stream());
          main_stream()->cr();

          bitmap[bit_count >> LogBitsPerByte] |=
           1 << (bit_count & (BitsPerByte-1));
        }
        bit_count++;
      }
    } else {
      if (hidden_methods) {
        main_stream()->print_cr("    No hidden fields");
      }
      if (has_hidden_fields) { // In other profile(s)
        bit_count += field_count(&klass);
      }
    }
    if (hidden_methods) {
      main_stream()->print_cr("    Hidden methods:");
      { // Static methods first
        const ObjArray::Raw methods = klass().methods();
        const int methods_length = methods().length();

        for (int i = 0; i < methods_length; i++) {
          const Method::Raw method = methods().obj_at(i);
          if (method.not_null()) {
            if (global_profile()->is_hidden_method(&klass, &method) ||
                profile->is_hidden_method(&klass, &method)) {
              main_stream()->print("      ");
              Symbol::Raw name = method().name();
              name().print_symbol_on(main_stream());
              main_stream()->cr();

              bitmap[bit_count >> LogBitsPerByte] |=
               1 << (bit_count & (BitsPerByte-1));
            }
            bit_count++;
          }
        }
      }
      { // Virtual methods next
        const ClassInfo::Raw info = klass().class_info();
        const int vtable_length = info().vtable_length();
        for (int i = 0; i < vtable_length; i++) {
          const Method::Raw method = info().vtable_method_at(i);
          if (method.not_null() && method().holder_id() == class_id) {
            if (global_profile()->is_hidden_method(&klass, &method) ||
                profile->is_hidden_method(&klass, &method)) {
              main_stream()->print("      ");
              Symbol::Raw name = method().name();
              name().print_symbol_on(main_stream());
              main_stream()->cr();

              bitmap[bit_count >> LogBitsPerByte] |=
               1 << (bit_count & (BitsPerByte-1));
            }
            bit_count++;
          }
        }
      }
    } else {
      if (hidden_fields) {
        main_stream()->print_cr("    No hidden methods");
      }
      if (has_hidden_methods) { // In other profile(s)
        bit_count += method_count(&klass);
      }
    }
    if (hidden_fields || hidden_methods) {
      main_stream()->print("*/\n");
    }
  }
  main_stream()->sp();
}
#else //  ENABLE_MULTIPLE_PROFILES_SUPPORT
inline void
SourceROMWriter::write_modified_class_bitmap(const int min, const int max,
                                             jubyte bitmap[]) {
  int bit_count = 0;
  for (SystemClassStream st; st.has_next();) {
    InstanceClass::Raw klass = st.next();
    const int class_id = klass().class_id();
    if (class_id > max) break;
    if (class_id < min || klass().access_flags().is_hidden()) {
      continue;
    }
    const bool hidden_fields = has_hidden_fields(&klass);
    const bool hidden_methods = has_hidden_methods(&klass);
    if (hidden_fields || hidden_methods) {
      main_stream()->print("/*\n  ");
      klass().print_name_on(main_stream());
      main_stream()->cr();

      if (hidden_fields) {
        main_stream()->print_cr("    Hidden fields:");
        ConstantPool::Raw cp = klass().constants();
        const TypeArray::Raw fields = klass().fields();
        const int fields_length = fields().length();
        for (int i = 0; i < fields_length; i += Field::NUMBER_OF_SLOTS) {
          const jushort name_index = fields().ushort_at(i + Field::NAME_OFFSET);
          OopDesc* field_name = cp().symbol_at(name_index);
          if (is_hidden_field(&klass, field_name)) {
            main_stream()->print("      ");
            Symbol::Raw name = field_name;
            name().print_symbol_on(main_stream());
            main_stream()->cr();

            bitmap[bit_count >> LogBitsPerByte] |=
             1 << (bit_count & (BitsPerByte-1));
          }
          bit_count++;
        }
      } else {
        main_stream()->print_cr("    No hidden fields");
      }

      if (hidden_methods) {
        main_stream()->print_cr("    Hidden methods:");
        { // Static methods first
          const ObjArray::Raw methods = klass().methods();
          const int methods_length = methods().length();

          for (int i = 0; i < methods_length; i++) {
            const Method::Raw method = methods().obj_at(i);
            if (method.not_null()) {
              if (is_hidden_method(&klass, &method)) {
                main_stream()->print("      ");
                Symbol::Raw name = method().name();
                name().print_symbol_on(main_stream());
                main_stream()->cr();

                bitmap[bit_count >> LogBitsPerByte] |=
                 1 << (bit_count & (BitsPerByte-1));
              }
              bit_count++;
            }
          }
        }
        { // Virtual methods next
          const ClassInfo::Raw info = klass().class_info();
          const int vtable_length = info().vtable_length();
          for (int i = 0; i < vtable_length; i++) {
            const Method::Raw method = info().vtable_method_at(i);
            if (method.not_null() && method().holder_id() == class_id) {
              if (is_hidden_method(&klass, &method)) {
                main_stream()->print("      ");
                Symbol::Raw name = method().name();
                name().print_symbol_on(main_stream());
                main_stream()->cr();

                bitmap[bit_count >> LogBitsPerByte] |=
                 1 << (bit_count & (BitsPerByte-1));
              }
              bit_count++;
            }
          }
        }
      } else {
        main_stream()->print_cr("    No hidden methods");
      }
      main_stream()->print("*/\n");
    }
  }
  main_stream()->sp();
}
#endif // ENABLE_MULTIPLE_PROFILES_SUPPORT

void SourceROMWriter::write_hidden_members(JVM_SINGLE_ARG_TRAPS) {
  int min = -1;
  int max = -2;
  {
    for (SystemClassStream st; st.has_next();) {
      const InstanceClass::Raw klass = st.next();
      if (klass().access_flags().is_hidden()) {
        continue;
      }
      if (has_hidden_fields(&klass) || has_hidden_methods(&klass)) {
        max = klass().class_id();
        if (min < 0) {
          min = max;
        }
      }
    }
  }
  const int size = max - min + 1;
  main_stream()->print_cr(
    "const unsigned short _rom_modified_class_index_base = %d;\n"
    "const unsigned short _rom_modified_class_index_size = %d;\n",
    min < 0 ? 0 : min, size);

  main_stream()->print_cr("const short _rom_modified_class_index[] = {" );
  if (size) {
    write_modified_class_index(min, max);
  } else {
    main_stream()->print_cr( "  -1 // No modified classes" );
  }
  main_stream()->print_cr( "}; // _rom_modified_class_index\n" );   

  int bit_count;
  main_stream()->print_cr( "const int _rom_modified_class_attributes[] = {" );
  if (size) {
    bit_count = write_modified_class_attributes(min, max);
  } else {
    main_stream()->print_cr( "  0 // No modified classes" );
    bit_count = 0;
  }
  main_stream()->print_cr( "}; // _rom_modified_class_attributes\n" );   

  main_stream()->print_cr( "const unsigned char _rom_modified_class_bitmap[] = {" );
  const int bit_scale_byte_size = (bit_count+(BitsPerByte-1)) >> LogBitsPerByte;
  if (bit_scale_byte_size) {
    TypeArray::Raw bit_scale =
      Universe::new_byte_array(bit_scale_byte_size JVM_OZCHECK(bit_scale));
    jubyte* bitmap = bit_scale().ubyte_base_address();
#if ENABLE_MULTIPLE_PROFILES_SUPPORT
    const ROMVector* rom_profiles = ROMOptimizer::profiles_vector();
    GUARANTEE(rom_profiles != NULL, "Sanity");

    const int profiles_count = rom_profiles->size();
    for (int profile_id = 0; profile_id < profiles_count; profile_id++) {
      if (profile_id != 0) {
        main_stream()->cr();
        main_stream()->cr();
      }
      main_stream()->print( "// Profile " );
      print_profile_name(profile_id);
      main_stream()->cr();

      const ROMProfile::Raw profile = rom_profiles->element_at(profile_id);
      write_modified_class_bitmap(min, max, bitmap, &profile);

      for (int i = 0; i < bit_scale_byte_size; i++) {
        main_stream()->print (" 0x%02x,", bitmap[i]);
        bitmap[i] = 0;
      }
    }
#else // ENABLE_MULTIPLE_PROFILES_SUPPORT
    write_modified_class_bitmap(min, max, bitmap);
    for (int i = 0; i < bit_scale_byte_size; i++) {
      main_stream()->print (" 0x%02x,", bitmap[i]);
    }
#endif // ENABLE_MULTIPLE_PROFILES_SUPPORT
  } else {
    main_stream()->print( "  0 // No modified classes" );
  }
  main_stream()->print_cr( "\n}; // _rom_modified_class_bitmap\n" );   

  main_stream()->print_cr("#if ENABLE_MULTIPLE_PROFILES_SUPPORT");
  main_stream()->print_cr(
    "const int _rom_modified_class_bitmap_row_size = %d;", bit_scale_byte_size);
  main_stream()->print_cr("#endif // ENABLE_MULTIPLE_PROFILES_SUPPORT\n");
}
#endif // ENABLE_MEMBER_HIDING

// write references to global singletons
void SourceROMWriter::write_global_singletons(JVM_SINGLE_ARG_TRAPS) {
  // write pointer to ROM constant pool
  main_stream()->print("const int* const _rom_constant_pool = (const int*)");
  if (skipped_constant_pool()->not_null()) {
    write_reference(skipped_constant_pool(), TEXT_BLOCK, main_stream() JVM_CHECK);
  } else {
    main_stream()->print("0");
  }
  main_stream()->print_cr(";");
  main_stream()->print("const int _gc_stackmap_size = ");
  main_stream()->print("%d", _gc_stackmap_size);
  main_stream()->print_cr(";");
  
}

// write special symbols to make sure we're linking the right ROMImage.o.
// See ROMImage.hpp for details.
void SourceROMWriter::write_link_checks() {
  main_stream()->print_cr("const int _ROM_LINKCHECK_HLE  = 0;");
  main_stream()->print_cr("const int _ROM_LINKCHECK_MFFL = 0;");
  main_stream()->print_cr("const int _ROM_LINKCHECK_MFFD = 0;");
}

void SourceROMWriter::link_output_rom_image() {
  OsFile_Handle dst = OsFile_open(Arguments::rom_output_file(), "w");
  append_file_to(dst, FilePath::rom_declare_file);
  append_file_to(dst, FilePath::rom_main_file);
  append_file_to(dst, FilePath::rom_reloc_file);
  OsFile_close(dst);

  dst = OsFile_open(FilePath::rom_log_file, "w");
  append_file_to(dst, FilePath::rom_summary_file);
  append_file_to(dst, FilePath::rom_optimizer_file);
  OsFile_close(dst);
}

void SourceROMWriter::combine_output_files() {
  _declare_stream.close();
  _main_stream.close();
  _reloc_stream.close();
  _jni_stream.close();
  _kvm_stream.close();

  _summary_log_stream.close();
  _optimizer_log_stream.close();

  link_output_rom_image();

  OsFile_remove(FilePath::rom_declare_file);
  OsFile_remove(FilePath::rom_main_file);
  OsFile_remove(FilePath::rom_reloc_file);

  OsFile_remove(FilePath::rom_summary_file);
  OsFile_remove(FilePath::rom_optimizer_file);
}

void SourceROMWriter::handle_jar_entry(const char* name, int length, 
                                       JarFileParser * /*jf*/
                                       JVM_TRAPS) {
  TypeArray byte_array = Universe::new_byte_array(length JVM_CHECK);
  char *base = (char*)byte_array.base_address();
  memcpy(base, name, length);
  ((SourceROMWriter*)_singleton)->
                _sorted_class_names->add_element(&byte_array JVM_CHECK);
}

void SourceROMWriter::get_all_names_in_classpath(ObjArray* classpath,
                                                 const bool classes JVM_TRAPS) {
  const int length = classpath->length();
  for(int index = 0; index < length; index++) {
    FilePath::Raw path = classpath->obj_at(index);
    JarFileParser::do_next_class_entries(&path, classes, 
                        (JarFileParser::do_entry_proc)&handle_jar_entry,
                        0, max_jint JVM_CHECK);
  }
}

// By sorting the classes in the classpath by name, we can ensure that
// the classes in ROMImage.cpp appear at the same order, regardless of the
// order of the entries in the JAR file. This help make the ROM image
// more patchable -- if the contents of the classes are changed slightly,
// the differences in the new ROM image will be small.
//
// IMPL_NOTE: this is not sufficient to make the ROM patchable -- we also need to
// make sure that the methods, fields and constant pools are padded and
// sorted in certain ways .... TBD.
void SourceROMWriter::sort_and_load_all_in_classpath(JVM_SINGLE_ARG_TRAPS) {
  UsingFastOops level1;
  const int suffix_length = STATIC_STRLEN(".class");
  ROMVector::Fast sorted_names;
  sorted_names().initialize(JVM_SINGLE_ARG_CHECK);
  int index;

  _sorted_class_names = &sorted_names;

  //loading classes
  ObjArray::Fast classpath = Task::current()->app_classpath();
  get_all_names_in_classpath(&classpath, true JVM_CHECK);

  sorted_names().sort();

  TypeArray::Fast byte_array;
  Symbol::Fast class_name;
  InstanceClass::Fast instance_class;
  for (index = 0; index < sorted_names().size(); index++) {
    byte_array = sorted_names().element_at(index);
    int len = byte_array().length() - suffix_length;
    class_name = SymbolTable::symbol_for(&byte_array,
                                (utf8)(byte_array().base_address()),
                                len JVM_CHECK);
    instance_class =
      SystemDictionary::resolve(&class_name, ErrorOnFailure JVM_NO_CHECK);
    if( instance_class.is_null() ) {
      tty->print( "Error romizing " );
      class_name().print_value_on(tty);
      tty->cr();
      return;
    }
    instance_class().verify(JVM_SINGLE_ARG_CHECK);
  }

  //loading resources
  sorted_names.set_null();
  sorted_names().initialize(JVM_SINGLE_ARG_CHECK);
  _sorted_class_names = &sorted_names;

  get_all_names_in_classpath(&classpath, false JVM_CHECK);

  sorted_names().sort();
  const int size = sorted_names().size();
  *Universe::resource_names() = Universe::new_obj_array(size JVM_CHECK);
  *Universe::resource_data()  = Universe::new_obj_array(size JVM_CHECK);
  *Universe::resource_size()  = Universe::new_int_array(size JVM_CHECK);
  
  Symbol::Fast resource_name;  
  Buffer::Fast resource_data;
  FileDecoder::Fast file_decoder;
  for (index = 0; index < size; index++) {
    byte_array = sorted_names().element_at(index);
    resource_name = SymbolTable::symbol_for(&byte_array,
                                            (utf8) byte_array().data(),
                                            byte_array().length() JVM_CHECK);
    Universe::resource_names()->obj_at_put(index, resource_name);
    file_decoder = ClassPathAccess::open_entry(&resource_name, false JVM_CHECK);
    resource_data = file_decoder().read_completely(JVM_SINGLE_ARG_CHECK);
    if (file_decoder().is_inflater()) {
      Universe::resource_size()->int_at_put(index, file_decoder().file_size());
      Universe::resource_data()->obj_at_put(index,
                                     ((Inflater*)&file_decoder)->in_buffer());
    } else {
      Universe::resource_size()->int_at_put(index, -1);
      Universe::resource_data()->obj_at_put(index, resource_data);
    }
  }
}


#define WRITE_FRAME_OFFSETS_CHECKER(x) \
  main_stream()->print_cr("const int _rom_check_%s = %d;", STR(x), x());

void SourceROMWriter::write_consistency_checks() {
#if ENABLE_COMPILER
   Compiler dummy;
#endif
  EnforceCompilerJavaStackDirection enfore_java_stack_direction;

  main_stream()->print_cr("#ifndef PRODUCT");
  main_stream()->print_cr("/* Info for checking AOT compiler consistency */");
  main_stream()->print_cr("const int _rom_compilation_enabled = %d;", 
                   ENABLE_COMPILER && EnableROMCompilation);
  main_stream()->print_cr("const int _rom_check_JavaFrame__arg_offset_from_sp_0 =%d;",
                   JavaFrame__arg_offset_from_sp(0));
  FRAME_OFFSETS_DO(WRITE_FRAME_OFFSETS_CHECKER);

  main_stream()->print_cr("const int _rom_generator_soft_float_enabled = %d;", 
                   (ENABLE_SOFT_FLOAT && ENABLE_FLOAT));
  main_stream()->print_cr("const int _rom_generator_target_msw_first_for_double = %d;", 
                   TARGET_MSW_FIRST_FOR_DOUBLE);

  main_stream()->print_cr("#endif");
}

bool SourceROMWriter::may_skip_constant_pool(Method *method) {
  // Effectively, we allow CP skipping for all methods whose CP is the 
  // first CP we see. In normal cases (unless you turn off CP merging), we
  // will have exactly 1 CP, so this function should return true.
  //
  // The reasons of having this check at all are to (1) test romization
  // with CP merging turned off; (2) allow ROM image to run even if CP merging
  // forgets to replace the CP of certain methods.
  ConstantPool::Raw cp = method->constants();
  if (skipped_constant_pool()->not_null()) {
    return skipped_constant_pool()->equals(&cp);
  } else {
    *skipped_constant_pool() = cp.obj();
    return true;
  }
}

void SourceROMWriter::fixup_image(JVM_SINGLE_ARG_TRAPS) {
  JarFileParser::flush_caches();
  SymbolTable::current()                        ->set_null();
  StringTable::current()                        ->set_null();
  Universe::gc_block_stackmap()                 ->set_null();
  Universe::verifier_stackmap_cache()           ->set_null();
  Universe::verifier_instruction_starts_cache() ->set_null();
  Universe::verifier_vstack_tags_cache()        ->set_null();
  Universe::verifier_vstack_classes_cache()     ->set_null();
  Universe::verifier_vlocals_tags_cache()       ->set_null();
  Universe::verifier_vlocals_classes_cache()    ->set_null();

  ROMWriter::fixup_image(JVM_SINGLE_ARG_CHECK);
}

#if ENABLE_COMPILER
void SourceROMWriter::write_aot_symbol_table(JVM_SINGLE_ARG_TRAPS) {
  main_stream()->print_cr("extern \"C\" {\nconst int __aot_symbol_table[] = {");

  UsingFastOops fast_oops;
  ObjArray::Fast raw_objects = visited_objects()->raw_array();
  ObjArray::Fast raw_infos   = visited_object_infos()->raw_array();
  Method::Fast method;
  CompiledMethod::Fast cm;
  Symbol::Fast class_name;
  Symbol::Fast method_name;
  Symbol::Fast signature;
  TypeArray::Fast native_name;

  const int size = visited_objects()->size();
  const OopDesc* const end = (OopDesc*)jvm_fast_globals.compiler_area_top;

  {
    int cm_count = 0;
    {
      for( const OopDesc* obj = (OopDesc*)jvm_fast_globals.compiler_area_start;
           obj < end;
           obj = DERIVED(OopDesc*, obj, obj->object_size()) ) {
        cm_count ++;
      }
    }

    // determine total number of methods
    int method_count = 0;
    {
      for( int i = 0; i < size; i++ ) {
        const Oop::Raw object = raw_objects().obj_at(i);
        if (object.is_method()) {
          method_count ++;
        }
      }
    }

    main_stream()->print_cr("  %d,", cm_count + method_count);
  }


  // Dump names of all compiled methods (in JNI style so that it's easier
  // to set break points inside debugger).
  {
    GCDisabler raw_pointers_used_in_this_block;

    OopDesc* obj = (OopDesc*)jvm_fast_globals.compiler_area_start;
    while (obj < end) {
      cm = obj;
      method = cm().method();

      const InstanceClass::Raw ic = method().holder();
      class_name = ic().original_name();

      bool dummy;
      method_name = method().get_original_name(dummy);

      if (method().is_overloaded()) {
        signature = method().signature();
      }
      native_name = Natives::convert_to_jni_name(&class_name,
           &method_name, &signature JVM_CHECK);

      main_stream()->print("  ");
      write_compiled_code_reference(&cm, main_stream(), false JVM_CHECK); // addr
      main_stream()->print(", %d", cm().size()); // code size
      main_stream()->print_cr(", (int)\"%s\",", (char*)native_name().data()); // name
      obj = DERIVED(OopDesc*, obj, obj->object_size());
    }
  }

  // Dump names of all regular methods (in Java style so that it's easier
  // to distinguish them from compiled methods).
  {
    for (int i = 0; i < size; i++) {
      const Oop::Raw object = raw_objects().obj_at(i);
      if (object().is_method()) {
        method = object().obj();
        main_stream()->print("  ");
        write_reference(&method, TEXT_BLOCK, main_stream() JVM_CHECK);
        main_stream()->print(", %d", method.object_size());
        main_stream()->print(", (int)\"");
        method().print_name_on(main_stream());
        main_stream()->print_cr("\",");
      }
    }
  }

  main_stream()->print_cr("0, 0, 0};");
  main_stream()->print_cr("}");
}

#if ENABLE_APPENDED_CALLINFO
void SourceROMWriter::write_compiled_method_table(JVM_SINGLE_ARG_TRAPS) {
  ROMVector * const compiled_methods = compiled_method_list();
  FileStream * const stream = main_stream();

  const int compiled_methods_count = compiled_methods->size();

  stream->print_cr("const unsigned int _rom_compiled_methods[] = {");

  if (compiled_methods_count <= 0) {
    stream->print_cr("0");
  } else {
    compiled_methods->sort();

    CompiledMethod cm;
    for (int i = 0; i < compiled_methods_count; i++) {
      cm = compiled_methods->element_at(i);
      stream->print("/*%4d*/", i);

      write_compiled_method_reference(&cm, ROMWriter::TEXT_BLOCK, 
                                      stream JVM_CHECK);
      stream->print_cr(",");
    }
  }
  stream->print_cr("};");

  stream->print_cr("const unsigned int _rom_compiled_methods_count = %d;",
                   compiled_methods_count);
}
#endif // ENABLE_APPENDED_CALLINFO

#endif
#if ENABLE_PREINITED_TASK_MIRRORS && ENABLE_ISOLATES         
void SourceROMWriter::write_tm_reference(Oop* owner, int inside_owner_offset, Oop* oop, FileStream* _stream ) { 
  SETUP_ERROR_CHECKER_ARG; 
  BlockType owner_type = block_type_of(owner JVM_NO_CHECK); 
  GUARANTEE(owner_type == ROMWriter::TASK_MIRRORS_BLOCK, "should be called only for references from this block!"); 
  BlockType oop_type = block_type_of(oop JVM_NO_CHECK); 
  int reference_offset = offset_of(owner JVM_NO_CHECK); 
  reference_offset = (reference_offset + inside_owner_offset) / sizeof(int); 
  int ref_off_word = reference_offset / BitsPerWord; 
  int ref_off_bit  = reference_offset % BitsPerWord; 
  int oop_offset = offset_of(oop JVM_NO_CHECK); 
  if (oop_type == ROMWriter::TEXT_BLOCK) { 
    write_text_reference(_stream, oop_offset); 
    return; 
  } else if (oop_type == ROMWriter::TASK_MIRRORS_BLOCK) { 
    const int block_type_mask = ((1 << ROM::block_type_width) - 1) << ROM::block_type_start; 
    GUARANTEE(!(oop_offset & block_type_mask), "offset too large!"); 
    write_plain_int(oop_offset, _stream);          
  } else { 
    write_plain_int(ROM::encode_heap_reference(oop), _stream); 
  } 
  int bitmap = rom_tm_bitmap()->int_at(ref_off_word); 
  GUARANTEE(!(bitmap & (1 << ref_off_bit)), "we are writing second time the same place!"); 
  bitmap |= 1 << ref_off_bit; 
  rom_tm_bitmap()->int_at_put(ref_off_word, bitmap); 
} 
#endif 
void OffsetFinder::begin_object(Oop *object JVM_TRAPS) {
  int offset = 0xdeadbeef;
  _current_type = writer()->block_type_of(object JVM_CHECK);
  int pass = writer()->pass_of(object JVM_CHECK);
  int skip_words = writer()->skip_words_of(object JVM_CHECK);
  const bool by_ref = ROMWriter::write_by_reference(object);

  switch (_current_type) {
  case ROMWriter::TEXT_BLOCK:
#if ENABLE_HEAP_NEARS_IN_HEAP  
  case ROMWriter::TEXT_AND_HEAP_BLOCK:
#endif
    offset = by_ref ? (int)object->obj() : _text_offset;
#if !USE_SEGMENTED_TEXT_BLOCK_WRITER
    if (offset == 0) {
      // if the first text object is a method we can't skip any header info
      if(object->is_method())  {
        writer()->set_method_start_skip(0);
      }
      writer()->set_skip_words_of(object, 0 JVM_CHECK);
      skip_words = 0;
    }
#else
    {
      int pass = writer()->pass_of(object JVM_CHECK);

      // We can't skip header info for any of the text block boundaries.
      int i;
      int boundary = 0;
      bool on_boundary = false;      
      for (i = 0; i < ROM::TEXT_BLOCK_SEGMENTS_COUNT; i++) {
        if (offset == boundary) {
          on_boundary = true;
          break;
        }
        boundary += writer()->_pass_sizes[i];
      }

      if (on_boundary) {
        writer()->set_skip_words_of(object, 0 JVM_CHECK);
        if(object->is_method()) {
          writer()->set_method_start_skip(0);
        }
        skip_words = 0;
      }
 
      int loc_offset = offset;
      for(i = 0; i < pass; i++) {
        loc_offset -= writer()->_pass_sizes[i]; 
      }

      loc_offset -= skip_words * sizeof(jobject);
      writer()->set_loc_offset_of(object, loc_offset JVM_CHECK);
    }
#endif
    break;
  case ROMWriter::DATA_BLOCK:
#if ENABLE_HEAP_NEARS_IN_HEAP  
  case ROMWriter::DATA_AND_HEAP_BLOCK:
#endif
    offset = by_ref ? (int)object->obj() : _data_offset;
    break;
  case ROMWriter::HEAP_BLOCK:
    offset = _heap_offset;
    break;
#if ENABLE_PREINITED_TASK_MIRRORS && ENABLE_ISOLATES 
  case ROMWriter::TASK_MIRRORS_BLOCK:
    offset = _task_mirrors_offset;
    break;
#endif
  default:
    SHOULD_NOT_REACH_HERE();
  }

  offset -= skip_words * sizeof(jobject);
#if ENABLE_HEAP_NEARS_IN_HEAP  
  if (_current_type == ROMWriter::TEXT_AND_HEAP_BLOCK) {
    //we must clone it into the heap!    
    writer()->set_heap_offset_of(object, _heap_offset JVM_CHECK);
  } else if (_current_type == ROMWriter::DATA_AND_HEAP_BLOCK) {
    //we must clone it into the heap!    
    writer()->set_heap_offset_of(object, _heap_offset JVM_CHECK);
  }
#endif

  if (!by_ref && ( _current_type == ROMWriter::TEXT_BLOCK
#if ENABLE_HEAP_NEARS_IN_HEAP  
        || _current_type == ROMWriter::TEXT_AND_HEAP_BLOCK
#endif
    )) {
    int last_offset = _last_text_offset;

    if (offset < 0 || offset <= last_offset) {
      // At this point, <offset> is the address of the first word of the
      // object, and <last_offset> is the address of the first word of the
      // last object in the same block, as if no words have been skipped
      // from either objects.
      //
      // If <skip_words> is too big, <offset> may be smaller than, or
      // equal to, <last_offset>. This would cause many things to
      // fail. For example, ROM::raw_text_klass_of() may get confused
      // when two text objects have the same offset.
      //
      // To make sure that <offset> is at least <last_offset>+4 bytes,
      // we place all the one-word TEXT objects in pass #7. We also don't
      // skip headers for the first method in TEXT.
      //
      // If you see the error below, change BlockTypeFinder::find_type()
      // to reduce skip words or change the pass number of the offending
      // object.

      tty->print_cr("ROMWriter error: illegal overlapping objects in TEXT");
      JVM_FATAL(empty_message);
    }
    _last_text_offset = offset;
  }

  writer()->set_offset_of(object, offset JVM_CHECK);
}

void OffsetFinder::put_reference(Oop *owner, int offset, Oop *object JVM_TRAPS)
{
  if (ROMWriter::write_by_value(owner)) {
    put_int(owner, 0 JVM_CHECK);
  }
}

void OffsetFinder::put_symbolic(Oop *owner, int offset JVM_TRAPS) {
  if (ROMWriter::write_by_value(owner)) {
    put_int(owner, 0 JVM_CHECK);
  }
}

void OffsetFinder::put_long(Oop *owner, jint msw, jint lsw JVM_TRAPS) {
  // the value passed to put_int doesn't matter -- we just want to find 
  // offset.
  if (ROMWriter::write_by_value(owner)) {
    put_int(owner, 0 JVM_CHECK);
    put_int(owner, 0 JVM_CHECK);
  }
}

void OffsetFinder::put_double(Oop *owner, jint msw, jint lsw JVM_TRAPS) {
  // the value passed to put_int doesn't matter -- we just want to find 
  // offset.
  if (ROMWriter::write_by_value(owner)) {
    put_int(owner, 0 JVM_CHECK);
    put_int(owner, 0 JVM_CHECK);
  }
}

void OffsetFinder::put_int(Oop *owner, jint value JVM_TRAPS) {
  switch (_current_type) {
  case ROMWriter::TEXT_BLOCK:
    _text_offset += sizeof(jint);
    break;
  case ROMWriter::DATA_BLOCK:
    _data_offset += sizeof(jint);
    break;
  case ROMWriter::HEAP_BLOCK:
    _heap_offset += sizeof(jint);
    break;
#if ENABLE_HEAP_NEARS_IN_HEAP  
  case ROMWriter::TEXT_AND_HEAP_BLOCK:    
    _text_offset += sizeof(jint);
    _heap_offset += sizeof(jint);
    break;
  case ROMWriter::DATA_AND_HEAP_BLOCK:    
    _data_offset += sizeof(jint);
    _heap_offset += sizeof(jint);
    break;
#endif
#if ENABLE_PREINITED_TASK_MIRRORS && USE_SOURCE_IMAGE_GENERATOR && ENABLE_ISOLATES  
  case ROMWriter::TASK_MIRRORS_BLOCK: 
    _task_mirrors_offset += sizeof(jint); 
    break; 
#endif 
  default:
    SHOULD_NOT_REACH_HERE();
  }
}

#endif //USE_SOURCE_IMAGE_GENERATOR
