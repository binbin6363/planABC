cmake_minimum_required(VERSION 2.6)

INCLUDE(versioninfo.cmake) 
SET(LIB_PATH ../../contri) 

INCLUDE_DIRECTORIES(${LIB_PATH}/comm/include)
INCLUDE_DIRECTORIES(${LIB_PATH}/net/src)
INCLUDE_DIRECTORIES(${LIB_PATH}/utils/include/)
INCLUDE_DIRECTORIES(${LIB_PATH}/leveldb-master)
INCLUDE_DIRECTORIES(${LIB_PATH}/leveldb-master/include)
INCLUDE_DIRECTORIES(.)

FIND_LIBRARY(UTILS_LIB utils ${LIB_PATH}/utils/lib/ NO_DEFAULT_PATH) 
FIND_LIBRARY(COMM_LIB common ${LIB_PATH}/comm/lib NO_DEFAULT_PATH)
FIND_LIBRARY(NET_LIB net ${LIB_PATH}/net/src NO_DEFAULT_PATH) 
FIND_LIBRARY(PTHREAD_LIB pthread /usr/lib/i386-linux-gnu/ /usr/lib /usr/local/lib NO_DEFAULT_PATH) 
FIND_LIBRARY(RUNTIME_LIB rt /usr/lib/i386-linux-gnu/ /usr/lib /usr/local/lib NO_DEFAULT_PATH) 
FIND_LIBRARY(LEVELDB_LIB leveldb /usr/local/lib NO_DEFAULT_PATH)
#FIND_LIBRARY(ICONV_LIB iconv /usr/local/lib NO_DEFAULT_PATH)


link_libraries(${NET_LIB} ${UTILS_LIB} ${COMM_LIB}
	${PTHREAD_LIB} ${RUNTIME_LIB} ${LEVELDB_LIB}) 

ADD_DEFINITIONS(
-O3 -g -W -Wall
 -Wunused-variable -Wunused-parameter -Wunused-function -Wunused
 -Wno-deprecated -Woverloaded-virtual -Wwrite-strings
 -D__WUR= -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DTIXML_USE_STL -DNDEBUG
)


add_library(lib_leveldb
    alivecmd.cpp
    backend_sync.cpp
    binlog.cpp
    client_accept.cpp
    client_session.cpp
    cmd_thread.cpp
    handlemsgcmd.cpp
    leveldb_dump.cpp
    leveldb_engine.cpp
    leveldb_server_app.cpp
    leveldb_server.cpp
    master_session.cpp
    msg.cpp
    nobinlog_iterator.cpp
    serverconfig.cpp
    slave_session.cpp
    udp_cmd_session.cpp
    utils.cpp
)


link_libraries(lib_leveldb)
add_executable(leveldbserver
    leveldb_server.cpp
    )


