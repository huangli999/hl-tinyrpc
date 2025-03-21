##################################
# makefile
# ikerli
# 2022-05-23
##################################

PATH_BIN = bin
PATH_LIB = lib
PATH_OBJ = obj

PATH_HL = /home/hl/hl-tinyrpc/hl
PATH_COMM = $(PATH_HL)/common


PATH_TESTCASES = testcases

#安装路径
PATH_INSTALL_LIB_ROOT = /home/hl/hl-tinyrpc/lib
PATH_INSTALL_INC_ROOT = /home/hl/hl-tinyrpc
PATH_INSTALL_INC_COMM = $(PATH_INSTALL_INC_ROOT)/$(PATH_COMM)



#编译
CXX := g++

CXXFLAGS += -g -O0 -std=c++11 -Wall -Wno-deprecated -Wno-unused-but-set-variable

CXXFLAGS += -I./ -I$(PATH_HL)	-I$(PATH_COMM) 

#外部静态库
LIBS += /usr/local/lib/libprotobuf.a	/usr/local/lib/libtinyxml.a

#源和目标文件
COMM_OBJ := $(patsubst $(PATH_COMM)/%.cc, $(PATH_OBJ)/%.o, $(wildcard $(PATH_COMM)/*.cc))

#目标
ALL_TESTS : $(PATH_BIN)/test_log 

TEST_CASE_OUT := $(PATH_BIN)/test_log 

LIB_OUT := $(PATH_LIB)/libhl.a

#测试日志
$(PATH_BIN)/test_log: $(LIB_OUT)
	$(CXX) $(CXXFLAGS) $(PATH_TESTCASES)/test_log.cc -o $@ $(LIB_OUT) $(LIBS) -ldl -pthread


$(LIB_OUT): $(COMM_OBJ) 
	cd $(PATH_OBJ) && ar rcv libhl.a *.o && cp libhl.a ../lib/

$(PATH_OBJ)/%.o : $(PATH_COMM)/%.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@




# print something test
# like this: make PRINT-PATH_BIN, and then will print variable PATH_BIN
PRINT-% : ; @echo $* = $($*)


# to clean 
clean :
	rm -f $(COMM_OBJ) $(NET_OBJ) $(TESTCASES) $(TEST_CASE_OUT) $(PATH_LIB)/libhl.a $(PATH_OBJ)/libhl.a $(PATH_OBJ)/*.o

# install
install:
	mkdir -p $(PATH_INSTALL_INC_COMM) $(PATH_INSTALL_INC_NET) $(PATH_INSTALL_INC_TCP) $(PATH_INSTALL_INC_CODER) $(PATH_INSTALL_INC_RPC)\
		&& cp $(PATH_COMM)/*.h $(PATH_INSTALL_INC_COMM) \
		&& cp $(PATH_NET)/*.h $(PATH_INSTALL_INC_NET) \
		&& cp $(PATH_TCP)/*.h $(PATH_INSTALL_INC_TCP) \
		&& cp $(PATH_CODER)/*.h $(PATH_INSTALL_INC_CODER) \
		&& cp $(PATH_RPC)/*.h $(PATH_INSTALL_INC_RPC) \
		&& cp $(LIB_OUT) $(PATH_INSTALL_LIB_ROOT)/


# uninstall
uninstall:
	rm -rf $(PATH_INSTALL_INC_ROOT)/hl && rm -f $(PATH_INSTALL_LIB_ROOT)/libhl.a