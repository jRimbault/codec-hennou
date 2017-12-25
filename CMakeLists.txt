cmake_minimum_required(VERSION 3.8)
project(codech)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        src/codec.c
        src/codec.h
        src/scheduler.c
        src/scheduler.h
        src/main.c
        src/matrix.c
        src/matrix.h
        src/structs.h
        src/workers.c
        src/workers.h)

add_executable(codech ${SOURCE_FILES})