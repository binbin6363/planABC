cmake_minimum_required(VERSION 2.6)

SET(CONTRI_DIR ../../contri/)

INCLUDE_DIRECTORIES(${CONTRI_DIR}/utils/include/)
INCLUDE_DIRECTORIES(${CONTRI_DIR}/comm/include/)
INCLUDE_DIRECTORIES(${CONTRI_DIR}/net/src)
INCLUDE_DIRECTORIES(../include)

FIND_LIBRARY(UTILS_LIB utils ${CONTRI_DIR}/utils/lib/ NO_DEFAULT_PATH)
FIND_LIBRARY(COMMON_LIB common ${CONTRI_DIR}/comm/lib/ NO_DEFAULT_PATH)
FIND_LIBRARY(NET_LIB net  ${CONTRI_DIR}/net/src/ NO_DEFAULT_PATH)
FIND_LIBRARY(PTHREAD_LIB pthread /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu/ NO_DEFAULT_PATH)
FIND_LIBRARY(RUNTIME_LIB rt /usr/lib  /usr/local/lib NO_DEFAULT_PATH)


link_libraries(${COMMON_LIB} ${NET_LIB} ${UTILS_LIB} ${PTHREAD_LIB} ${RUNTIME_LIB})

ADD_DEFINITIONS(
-O3 -g -W -Wall
 -Wunused-variable -Wunused-parameter -Wunused-function -Wunused
 -Wno-deprecated -Woverloaded-virtual -Wwrite-strings
 -D__WUR= -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DTIXML_USE_STL
)



add_library(lib_mobile
        client_session.cpp
        server_app.cpp
        server_config.cpp
        cmd.cpp
        udp_cmd_session.cpp
)

link_libraries(lib_mobile)
add_executable(mobiled
    server.cpp
    )

