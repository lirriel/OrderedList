cmake_minimum_required(VERSION 3.6)
project(prj)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp
        JournalNetActivity.h JournalNetActivity.hpp
        NetActivity.cpp NetActivity.h
        OrderedList.h OrderedList.hpp
        SkipList.h SkipList.hpp
        TimeStamp.cpp TimeStamp.h)
add_executable(prj ${SOURCE_FILES})