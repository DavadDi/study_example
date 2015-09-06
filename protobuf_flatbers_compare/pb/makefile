#makefile for those who have many excutable files

.PHONY: all strip clean sync install mem_check
.SUFFIXES: .sh .h .hpp .c .cc .cpp

CC			= gcc
CX			= g++
#CXFLAGS	=  -g -Wall -Werror -std=c++11 #-ggdb3
CXFLAGS		= -O3 -Werror -std=c++11
CX_MACRO	=

PB_INC		= -I./protobuf/include/
PB_LIB		= -L./protobuf/lib -lprotobuf

INC_DIR		= ./inc
SRC_DIR		= ./src
OBJ_DIR		= ./obj
BIN_DIR		= ./bin
BIN			= tellist_pb


INC			= $(PB_INC) -I./ -I$(INC_DIR)
LIB			= $(PB_LIB)

all: debug release
#strip

debug release: dir $(addprefix $(BIN_DIR)/,$(BIN))

#@ do not outoupt this command - ingnore its error and continue to make
dir:
	@-mkdir -p $(INC_DIR)
	@-mkdir -p $(SRC_DIR)
	@-mkdir -p $(OBJ_DIR)
	@-mkdir -p $(BIN_DIR)

$(OBJ_DIR)/%.o:$(SRC_DIR)/%.cc
	$(CX) $(CXFLAGS) $(CX_MACRO) $(INC) -o $@ -c $<

$(BIN_DIR)/tellist_pb:$(addprefix $(OBJ_DIR)/, tellist.pb.o tellist_pb.o)
	$(CX) $(CXFLAGS) $(CX_MACRO) -o $@ $^ $(LIB)

strip:
	strip $(addprefix $(BIN_DIR)/,$(BIN))

clean:
	@rm -f $(addprefix $(BIN_DIR)/,$(BIN)) $(OBJ_DIR)/*.o ./valgrind_info
	@rmdir $(BIN_DIR) $(OBJ_DIR)

mem_check:
	valgrind --log-file=valgrind_info --track-origins=yes --leak-check=full --show-reachable=yes -v bin/tellist_pb

