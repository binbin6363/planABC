cmake_minimum_required(VERSION 2.6)


ADD_DEFINITIONS(
-O3 -g -W -Wall
 -Wunused-variable -Wunused-parameter -Wunused-function -Wunused
 -Wno-deprecated -Woverloaded-virtual -Wwrite-strings
 -D__WUR= -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DTIXML_USE_STL )


add_library(lib_selector

conshashselector.cpp
directselector.cpp
hashselector.cpp
selector.cpp
session.cpp
sessiongroup.cpp
crc32.c
) 

link_libraries(lib_selector) 

add_executable(selector 
    test.cpp
) 
