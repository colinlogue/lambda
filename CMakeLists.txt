cmake_minimum_required(VERSION 3.16)
project(lambda)

set(CMAKE_CXX_STANDARD 20)

add_library(
        lambda
        SHARED
        src/parse.cpp
        src/parse.h
        src/eval.cpp
        src/eval.h
        src/lex.cpp
        src/lex.h
        src/prelude.h
        lib/lang_tools/lexer/lexer.hpp
        lib/lang_tools/repl/REPL.hpp
        lib/result/Result.hpp
        lib/lang_tools/utils/utils.h
        lib/lang_tools/utils/utils.cpp
        lib/lang_tools/parse/parse.hpp
        lib/lang_tools/eval/eval.hpp
)

add_executable(
        lambda_run
        lambda
        main.cpp
)

add_executable(
        lambda_test
        lambda
        test/lambda_tests.cpp
)

include_directories(src, lib)

target_link_libraries(lambda_run lambda)
target_link_libraries(lambda_test lambda)

target_include_directories(lambda_test PUBLIC src)