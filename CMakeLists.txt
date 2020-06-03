cmake_minimum_required(VERSION 3.16)
project(lambda)

set(CMAKE_CXX_STANDARD 20)

include_directories(src, lib)

add_executable(
        lambda
        main.cpp
        src/parse.cpp
        src/parse.h
        src/eval.cpp
        src/eval.h
        src/lex.cpp
        src/lex.h
        lib/lang_tools/lexer/lexer.hpp
        lib/lang_tools/repl/REPL.hpp
        lib/result/Result.hpp
        lib/lang_tools/utils/utils.h
        lib/lang_tools/utils/utils.cpp
        lib/lang_tools/parse/parse.hpp lib/lang_tools/eval/eval.hpp)