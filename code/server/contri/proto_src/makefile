C_FLAGS= -Wall -Wpointer-arith -O3 -g -DNDEBUG
CXX = g++
AR = ar
INC= -I./src
HEADER_DIR= src/
PROTO_DIR= proto/
SRC_DIR= src/
OBJ_DIR= obj/
LIB_DIR= lib/
SOURCES_FILES_CC=$(wildcard $(SRC_DIR)*.cc)
OBJECTS = $(patsubst %.cc,$(OBJ_DIR)%.o,$(notdir $(SOURCES_FILES_CC)))

LIB_TARGET = lib/libadv_proto.a

OBJ_EXT= .o
CXXSRC_EXT= .cc

$(OBJ_DIR)%$(OBJ_EXT): $(SRC_DIR)%$(CXXSRC_EXT)
	$(CXX) $(INC) $(C_FLAGS) -c $< -o $@

all:
	make mkproto && \
	make clean_proto && \
	make $(LIB_TARGET)

$(LIB_TARGET): $(OBJECTS)
	$(AR) rcs $(LIB_TARGET) $(OBJECTS)

mkproto:
	pushd $(PROTO_DIR) && \
	protoc --cpp_out=../$(SRC_DIR) ./*.proto && \
	popd

clean:
	rm -f $(LIB_TARGET) $(LIB_TARGET_THREAD) $(OBJECTS)

clean_proto:
	rm -f $(LIB_TARGET) $(OBJECTS)
