#C_FLAGS= -Wall -Wpointer-arith -O3 -pipe -g -D_REENTRANT -D_GUN_SOURCE #-D_USE_THREAD
C_FLAGS= -Wall -Wpointer-arith -g -DNDEBUG -D_MULTI_THREAD
CXX = g++
AR = ar
INC= -I./include -I ../comm/include
SRC_DIR= src/
OBJ_DIR= obj/
LIB_DIR= lib/
SOURCES_FILES_CPP=$(wildcard $(SRC_DIR)*.cpp)
OBJECTS = $(patsubst %.cpp,$(OBJ_DIR)%.o,$(notdir $(SOURCES_FILES_CPP)))

LIB_TARGET = lib/libutils.a
LIB_TARGET_THREAD = lib/libutils_thread.a

OBJ_EXT= .o
CXXSRC_EXT= .cpp

$(OBJ_DIR)%$(OBJ_EXT): $(SRC_DIR)%$(CXXSRC_EXT)
	$(CXX) $(INC) $(C_FLAGS) -c $< -o $@

all:
	make clean_socket && \
	make $(LIB_TARGET)

thread:
	make clean_socket_thread && \
	make C_FLAGS="$(C_FLAGS) -D_USE_THREAD" $(LIB_TARGET_THREAD)

$(LIB_TARGET): $(OBJECTS)
	$(AR) rcs $(LIB_TARGET) $(OBJECTS)

$(LIB_TARGET_THREAD): $(OBJECTS)
	$(AR) rcs $(LIB_TARGET_THREAD) $(OBJECTS)

clean:
	rm -f $(LIB_TARGET) $(LIB_TARGET_THREAD) $(OBJECTS)

clean_socket:
	rm -f $(LIB_TARGET) $(OBJECTS)

clean_socket_thread:
	rm -f $(LIB_TARGET_THREAD) $(OBJECTS)
