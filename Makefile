#####################################################################################
# COMPILER SETTINGS
#####################################################################################
CXX := g++
CXXFLAGS := -std=c++17 -fsanitize=address -fsanitize=undefined -g
CFLAGS := -pthread
LDLIBS := -lm -lpthread

#####################################################################################
# DIRECTORY PATHS
#####################################################################################
SRC := src
OBJ := obj
TEST := tests

INCLUDE_DIRS := -Iinclude -Iinclude/ann -Iinclude/tensor -Iinclude/sformat \
                -Idemo -Isrc -Itests

MKDIR := mkdir -p
RM := rm -rf

#####################################################################################
# MAIN PROGRAM (Project demo)
#####################################################################################
BIN := program
SRCs := $(shell find $(SRC) -name "*.cpp")
OBJs := $(subst $(SRC), $(OBJ), $(SRCs:.cpp=.o))

#####################################################################################
# UNIT TEST FILES
#####################################################################################
TEST_BIN := main
MAIN := main.cpp

XARRAYLIST_TEST := tests/unit_test/xarraylist/unit_test.cpp
DLINKEDLIST_TEST :=	tests/unit_test/dlinkedlist/unit_test.cpp

HASH_TEST := tests/unit_test/hash/unit_test.cpp
HEAP_TEST := tests/unit_test/heap/unit_test.cpp

GRAPH_TEST := \
	tests/unit_test/graph/unit_test.cpp \
	tests/unit_test/graph/test/*.cpp

SORT_TOPO_TEST := \
	tests/unit_test/sort_topo/unit_test.cpp \
	tests/unit_test/sort_topo/test/*.cpp

TENSOR_SRC := src/tensor/*.cpp

ANN_COMMON := \
	src/ann/functions.cpp \
	src/ann/loss/LossLayer.cpp \
	src/ann/loss/CrossEntropy.cpp \
	src/ann/layer/Softmax.cpp \
	src/ann/layer/Sigmoid.cpp \
	src/ann/layer/FCLayer.cpp \
	src/ann/layer/ReLU.cpp \
	src/ann/layer/Layer.cpp \
	src/ann/layer/Tanh.cpp

#####################################################################################
# FORMAT SETTINGS (clang-format)
#####################################################################################
CLANG_FORMAT := clang-format

FORMAT_FILES := \
	$(shell find . -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.h" \) \
		-not -path "./include/sformat/*" \
		-not -path "./include/tensor/*")

#####################################################################################
# DEFAULT TARGET
#####################################################################################
.PHONY: all
all:
	@echo "================ AVAILABLE TARGETS ================"
	@echo " Program:"
	@echo "   make program"
	@echo ""
	@echo " Unit Test:"
	@echo "   make xarraylist | dlinkedlist"
	@echo "   make hash | heap"
	@echo "   make layer_relu | layer_sigmoid | layer_tanh | layer_softmax"
	@echo "   make loss"
	@echo "   make graph | sort_topo"
	@echo ""
	@echo " Utility:"
	@echo "   make format"
	@echo "   make clean"
	@echo "==================================================="

#####################################################################################
# PROGRAM BUILD
#####################################################################################
.PHONY: program
program: $(OBJs)
	$(CXX) $(CFLAGS) $(INCLUDE_DIRS) $(OBJs) -o $(BIN) $(LDLIBS)

$(OBJ)/%.o: $(SRC)/%.cpp
	$(MKDIR) $(dir $@)
	$(CXX) $(CFLAGS) $(INCLUDE_DIRS) -c $< -o $@

#####################################################################################
# UNIT TEST TARGETS
#####################################################################################
.PHONY: xarraylist
xarraylist: clean_test
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) \
		$(MAIN) $(XARRAYLIST_TEST) \
		-DTEST_XARRAYLIST \
		-o $(TEST_BIN)

.PHONY: dlinkedlist
dlinkedlist: clean_test
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) \
		$(MAIN) $(DLINKEDLIST_TEST) \
		-DTEST_DLINKEDLIST \
		-o $(TEST_BIN)


.PHONY: hash
hash: clean_test
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) \
		$(MAIN) $(HASH_TEST) $(HEAP_TEST) \
		-DTEST_HASH \
		-o $(TEST_BIN)

.PHONY: heap
heap: clean_test
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) \
		$(MAIN) $(HASH_TEST) $(HEAP_TEST) \
		-DTEST_HEAP \
		-o $(TEST_BIN)


.PHONY: layer_relu
layer_relu: clean_test
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) \
		$(TENSOR_SRC) $(MAIN) \
		tests/unit_test/layer/unit_test_relu.cpp \
		$(ANN_COMMON) \
		-DTEST_LAYER \
		-o $(TEST_BIN)

.PHONY: layer_sigmoid
layer_sigmoid: clean_test
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) \
		$(TENSOR_SRC) $(MAIN) \
		tests/unit_test/layer/unit_test_sigmoid.cpp \
		$(ANN_COMMON) \
		-DTEST_LAYER \
		-o $(TEST_BIN)

.PHONY: layer_tanh
layer_tanh: clean_test
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) \
		$(TENSOR_SRC) $(MAIN) \
		tests/unit_test/layer/unit_test_tanh.cpp \
		$(ANN_COMMON) \
		-DTEST_LAYER \
		-o $(TEST_BIN)

.PHONY: layer_softmax
layer_softmax: clean_test
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) \
		$(TENSOR_SRC) $(MAIN) \
		tests/unit_test/layer/unit_test_softmax.cpp \
		$(ANN_COMMON) \
		-DTEST_LAYER \
		-o $(TEST_BIN)

.PHONY: loss
loss: clean_test
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) \
		$(TENSOR_SRC) $(MAIN) \
		tests/unit_test/loss/unit_test_CrossEntropy.cpp \
		$(ANN_COMMON) \
		-DTEST_LOSS \
		-o $(TEST_BIN)

.PHONY: graph
graph: clean_test
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) \
		$(MAIN) $(GRAPH_TEST) \
		-DTEST_GRAPH \
		-o $(TEST_BIN)

.PHONY: sort_topo
sort_topo: clean_test
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) \
		$(MAIN) $(SORT_TOPO_TEST) \
		-DTEST_SORT_TOPO \
		-o $(TEST_BIN)

#####################################################################################
# FORMAT
#####################################################################################
.PHONY: format
format:
	@echo "Formatting project using .clang-format (excluding sformat & tensor)..."
	$(CLANG_FORMAT) -i $(FORMAT_FILES)
	@echo "Formatting done."

#####################################################################################
# CLEAN
#####################################################################################
.PHONY: clean clean_test
clean:
	$(RM) $(BIN) $(OBJ)

clean_test:
	$(RM) $(TEST_BIN)
