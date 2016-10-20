#C_FLAGS= -Wall -Wpointer-arith -O3 -pipe -g -D_REENTRANT -D_GUN_SOURCE #-D_USE_THREAD

COLIB_DIR= ../libco

IMO_OPTIMIZE_DEFS = -DIMO_OPTIMIZE

C_FLAGS= -Wall -Wpointer-arith -O3 -g -DNDEBUG
C_FLAGS += $(IMO_OPTIMIZE_DEFS)
CXX = g++
AR = ar
INC= -I./src/strategy -I../net/src -I ${COLIB_DIR} -I../utils/include -I ../comm/include
SRC_DIR= src/
CO_DIR=libco/
OBJ_DIR= obj/
LIB_DIR= lib/
SOURCES_FILES_CC=$(wildcard $(SRC_DIR)*.cpp)
OBJECTS = $(patsubst %.cpp,$(OBJ_DIR)%.o,$(notdir $(SOURCES_FILES_CC)))
OBJECTS += ./obj/coctx.o ./obj/coctx_swap.o ./obj/co_routine.o
LIB_TARGET = lib/libcoroutine.a
LIB_TARGET_THREAD = lib/libcoroutine_thread.a

OBJ_EXT= .o
CXXSRC_EXT= .cpp

$(OBJ_DIR)%$(OBJ_EXT): $(SRC_DIR)%$(CXXSRC_EXT)
	$(CXX) $(INC) $(C_FLAGS) -c $< -o $@

all:
	make clean && \
	make mk_libco && \
	make $(LIB_TARGET)


$(LIB_TARGET): $(OBJECTS)
	$(AR) rcs $(LIB_TARGET) $(OBJECTS)


mk_libco:
	pushd ${COLIB_DIR} && \
	make colib && \
	popd && \
	mv ${COLIB_DIR}/*.o obj/

clean:
	pushd ${COLIB_DIR} && \
	make clean && \
	popd &&\
	rm -f $(LIB_TARGET) $(OBJECTS)
