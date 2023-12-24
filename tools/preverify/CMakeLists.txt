cmake_minimum_required(VERSION 3.22.1)

project(preverify)

add_executable(preverify
        src/check_class.c
        src/main.c
        src/utf.c
        src/check_code.c
        src/convert_md.c
        src/util.c
        src/jar.c
        src/jar_support.c
        src/classloader.c
        src/file.c
        src/classresolver.c
        src/stubs.c
        src/inlinejsr.c
        src/sys_support.c    
)

target_compile_definitions(preverify PRIVATE UNIX LINUX JAVAVERIFY TRIMMED ARCH=i386)
target_include_directories(preverify PRIVATE src)
target_compile_options(preverify PRIVATE)
