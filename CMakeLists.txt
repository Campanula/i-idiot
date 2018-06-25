cmake_minimum_required(VERSION 3.10)
project(i_am_an_idiot C)

set(CMAKE_C_STANDARD 11)
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

include_directories(.)

add_executable(i_am_an_idiot
        fahr_celsius.c
        fahr_celsius.h
        hello.c)
