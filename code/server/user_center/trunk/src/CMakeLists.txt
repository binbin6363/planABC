cmake_minimum_required(VERSION 2.6)
INCLUDE(versioninfo.cmake)
SET(LIB_PATH ../../../contri/)
SET(PROTOC_PATH ../../../contri/proto_src/)


INCLUDE_DIRECTORIES(${LIB_PATH}comm/include)
INCLUDE_DIRECTORIES(${LIB_PATH}coroutine/src/)
INCLUDE_DIRECTORIES(${LIB_PATH}coroutine/src/strategy)
INCLUDE_DIRECTORIES(${LIB_PATH}utils/include)
INCLUDE_DIRECTORIES(${LIB_PATH}/net/src)
INCLUDE_DIRECTORIES(${PROTOC_PATH}src/)
INCLUDE_DIRECTORIES(${LIB_PATH}libco/)
INCLUDE_DIRECTORIES(${LIB_PATH}/redis-3.0.0/deps/hiredis)
INCLUDE_DIRECTORIES(.)

FIND_LIBRARY(UTILS_LIB utils ${LIB_PATH}utils/lib/ NO_DEFAULT_PATH)
FIND_LIBRARY(NET_LIB net ${LIB_PATH}/net/src NO_DEFAULT_PATH)
FIND_LIBRARY(COMM_LIB common ${LIB_PATH}comm/lib NO_DEFAULT_PATH)
FIND_LIBRARY(PTHREAD_LIB pthread /usr/lib /usr/local/lib NO_DEFAULT_PATH)
FIND_LIBRARY(RUNTIME_LIB rt /usr/lib  /usr/local/lib NO_DEFAULT_PATH)
FIND_LIBRARY(LIBCO_LIB colib ${LIB_PATH}libco/lib NO_DEFAULT_PATH) 
FIND_LIBRARY(COROUTINE_LIB coroutine ${LIB_PATH}coroutine/lib/ NO_DEFAULT_PATH)
FIND_LIBRARY(PROTO_MSG adv_proto ${PROTOC_PATH}/lib/ NO_DEFAULT_PATH)
FIND_LIBRARY(PROTOC_LIB protoc /usr/lib  /usr/local/lib NO_DEFAULT_PATH)
FIND_LIBRARY(PROTOBUF_LIB protobuf /usr/lib  /usr/local/lib NO_DEFAULT_PATH)
FIND_LIBRARY(HIREDIS_LIB hiredis /usr/lib ${LIB_PATH}/redis-3.0.0/deps/hiredis NO_DEFAULT_PATH)

link_libraries(${LIBCO_LIB} ${COROUTINE_LIB} ${NET_LIB} ${UTILS_LIB} ${COMM_LIB}
           ${PTHREAD_LIB} ${RUNTIME_LIB} ${PROTO_MSG} ${PROTOC_LIB} ${PROTOBUF_LIB} ${HIREDIS_LIB} -lpthread -ldl)

ADD_DEFINITIONS(
-O3 -g -W -Wall
 -Wunused-variable -Wunused-parameter -Wunused-function -Wunused
 -Wno-deprecated -Woverloaded-virtual -Wwrite-strings
 -D__WUR= -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DTIXML_USE_STL
)

add_library(lib_uc
cmd.cpp
user.cpp
client_processor.cpp
dbp_processor.cpp
ldb_processor.cpp
status_processor.cpp
factories.cpp
xml_parser.cpp
client_msg.cpp
login_server_app.cpp
redis_client.cpp
)


link_libraries(lib_uc)
add_executable(usercenter
    login_server.cpp
    )

