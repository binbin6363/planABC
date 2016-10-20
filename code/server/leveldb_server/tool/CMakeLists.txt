cmake_minimum_required(VERSION 2.6)

SET(LIB_PATH ../../../../) 


INCLUDE_DIRECTORIES(${LIB_PATH}thirdparty/leveldb-master)
INCLUDE_DIRECTORIES(${LIB_PATH}thirdparty/leveldb-master/include)
INCLUDE_DIRECTORIES(.)

FIND_LIBRARY(LEVELDB_LIB leveldb /usr/local/lib NO_DEFAULT_PATH)


link_libraries(${LEVELDB_LIB}) 

ADD_DEFINITIONS(
-O3 -g -W -Wall
 -Wunused-variable -Wunused-parameter -Wunused-function -Wunused
 -Wno-deprecated -Woverloaded-virtual -Wwrite-strings
 -D__WUR= -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DTIXML_USE_STL -DNDEBUG
)



add_executable(ldb_cmp_tool
    leveldb_tool.cpp
    )


