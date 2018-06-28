cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

project(i_am_an_idiot C)

set(CMAKE_C_STANDARD 11)
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

include_directories(.)

add_executable(i_am_an_idiot
        fahr_celsius.c
        fahr_celsius.h
        echo_eof.h
        echo_eof.c
        count_char.c
        count_char.h
        hello.c)

