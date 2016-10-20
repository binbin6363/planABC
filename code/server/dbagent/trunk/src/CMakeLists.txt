cmake_minimum_required(VERSION 2.6)

INCLUDE(versioninfo.cmake) 
SET(LIB_PATH ../../../contri/)

INCLUDE_DIRECTORIES(${LIB_PATH}utils/include/)
INCLUDE_DIRECTORIES(${LIB_PATH}comm/include)
INCLUDE_DIRECTORIES(${LIB_PATH}net/src)
INCLUDE_DIRECTORIES(${LIB_PATH}proto_src/src)
INCLUDE_DIRECTORIES(../include)

FIND_LIBRARY(UTILS_LIB utils ${LIB_PATH}utils/lib/ NO_DEFAULT_PATH) 
FIND_LIBRARY(NET_LIB net ${LIB_PATH}net/src/ NO_DEFAULT_PATH) 
FIND_LIBRARY(COMM_LIB common ${LIB_PATH}comm/lib/ NO_DEFAULT_PATH)
FIND_LIBRARY(PROTO_MSG_LIB adv_proto ${LIB_PATH}proto_src/lib/ NO_DEFAULT_PATH)
FIND_LIBRARY(PTHREAD_LIB pthread /usr/lib /usr/local/lib NO_DEFAULT_PATH) 
FIND_LIBRARY(RUNTIME_LIB rt /usr/lib  /usr/local/lib NO_DEFAULT_PATH) 
FIND_LIBRARY(PROTOBUF_LIB protobuf /usr/local/lib NO_DEFAULT_PATH)
FIND_LIBRARY(MYSQL_LIB mysqlclient /usr/lib64/mysql NO_DEFAULT_PATH)


link_libraries(${NET_LIB} ${UTILS_LIB} ${COMM_LIB} ${PROTO_MSG_LIB}
	${PTHREAD_LIB} ${RUNTIME_LIB} ${PROTOBUF_LIB}  ${MYSQL_LIB}) 

ADD_DEFINITIONS(
-O3 -g -W -Wall
 -Wunused-variable -Wunused-parameter -Wunused-function -Wunused
 -Wno-deprecated -Woverloaded-virtual -Wwrite-strings
 -D__WUR= -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DTIXML_USE_STL
)


add_library(lib_dbagent
client_accept.cpp
client_session.cpp
cmd.cpp
dbpool.cpp
msg.cpp
mysql_msg.cpp
mysqloperator.cpp
server_app.cpp
server_config.cpp
server.cpp
udp_cmd_session.cpp
)


link_libraries(lib_dbagent)
add_executable(dbagent
    server.cpp
    )


