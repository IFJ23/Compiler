cmake_minimum_required(VERSION 3.26)
project(Compiler C)

set(CMAKE_C_STANDARD 11)

include_directories(SRC)

add_executable(Compiler
        SRC/error.c
        SRC/error.h
        SRC/expression.c
        SRC/expression.h
        SRC/generator.c
        SRC/generator.h
        SRC/parser.c
        SRC/parser.h
        SRC/scanner.c
        SRC/scanner.h
        SRC/structures.c
        SRC/structures.h
        SRC/symtable.c
        SRC/symtable.h
        tests/parser/test.c
        tests/scanner/test.c)
