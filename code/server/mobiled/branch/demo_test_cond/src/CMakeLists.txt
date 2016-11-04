cmake_minimum_required(VERSION 2.6)

INCLUDE(versioninfo.cmake)

SET(CONTRI_DIR ../../../contri/)

INCLUDE_DIRECTORIES(${CONTRI_DIR}/utils/include/)
INCLUDE_DIRECTORIES(${CONTRI_DIR}/comm/include/)
INCLUDE_DIRECTORIES(${CONTRI_DIR}/net/src)
INCLUDE_DIRECTORIES(${CONTRI_DIR}/jsoncpp/include)
INCLUDE_DIRECTORIES(${CONTRI_DIR}/proto_src/src)
INCLUDE_DIRECTORIES(../include)

FIND_LIBRARY(UTILS_LIB utils ${CONTRI_DIR}/utils/lib/ NO_DEFAULT_PATH)
FIND_LIBRARY(COMMON_LIB common ${CONTRI_DIR}/comm/lib/ NO_DEFAULT_PATH)
FIND_LIBRARY(NET_LIB net  ${CONTRI_DIR}/net/src/ NO_DEFAULT_PATH)
FIND_LIBRARY(JSONCPP_LIB jsoncpp  ${CONTRI_DIR}/jsoncpp/src/lib_json NO_DEFAULT_PATH)
FIND_LIBRARY(PROTO_MSG_LIB adv_proto  ${CONTRI_DIR}/proto_src/lib NO_DEFAULT_PATH)
FIND_LIBRARY(PROTO_LIB protobuf  /usr/lib /usr/local/lib NO_DEFAULT_PATH)
FIND_LIBRARY(PTHREAD_LIB pthread /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu/ NO_DEFAULT_PATH)
FIND_LIBRARY(RUNTIME_LIB rt /usr/lib  /usr/local/lib NO_DEFAULT_PATH)


link_libraries(${COMMON_LIB} ${NET_LIB} ${UTILS_LIB} ${PTHREAD_LIB}
${JSONCPP_LIB} ${PROTO_MSG_LIB} ${PROTO_LIB} ${RUNTIME_LIB})

ADD_DEFINITIONS(
-O3 -g -W -Wall
 -Wunused-variable -Wunused-parameter -Wunused-function -Wunused
 -Wno-deprecated -Woverloaded-virtual -Wwrite-strings
 -D__WUR= -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DTIXML_USE_STL
# -D__WUR= -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DTIXML_USE_STL -D_DEBUG
)



add_library(lib_mobile
        client_session.cpp
        server_app.cpp
        server_config.cpp
        cmd.cpp
        udp_cmd_session.cpp
        user_center_session.cpp
        task_center_session.cpp
        dbp_session.cpp
        request.cpp
        backend_request.cpp
        custom_type.cpp
        inner_common_msg.cpp
)

link_libraries(lib_mobile)
add_executable(mobiled
    server.cpp
    )

