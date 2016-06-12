cmake_minimum_required(VERSION 3.5)
project(DrunkStudents)

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
SET(CMAKE_C_COMPILER mpicc)
SET(CMAKE_CXX_COMPILER mpicxx)

set(SOURCE_FILES
        Lamport.cpp
        Lamport.h
        main.cpp
        Makefile
        State.cpp
        State.h
        Student.cpp
        Student.h
        StateEnum.h
        AlarmClock.cpp
        AlarmClock.h
        MessageTag.h Message.h settings.h)

include_directories(.)

add_executable(DrunkStudents ${SOURCE_FILES})