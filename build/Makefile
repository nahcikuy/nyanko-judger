CXX = g++
CXXFLAGS = -std=c++17
LDFLAGS =
LBLIBS = -pthread -lseccomp

TARGETS = njudger njudgerd nchecker

SRC_COMMON_C = $(wildcard ../src/common/*.c)
SRC_COMMON = $(wildcard ../src/common/*.cpp)
SRC_NJUDGER = $(wildcard ../src/njudger/*.cpp)
SRC_NJUDGERD = $(wildcard ../src/njudgerd/*.cpp)
SRC_NCHECKER = $(wildcard ../src/nchecker/*.cpp)

OBJ_COMMON_C = $(SRC_COMMON_C:../src/common/%.c=./objects/common/%.o)
OBJ_COMMON = $(SRC_COMMON:../src/common/%.cpp=./objects/common/%.o)
OBJ_NJUDGER = $(SRC_NJUDGER:../src/njudger/%.cpp=./objects/njudger/%.o)
OBJ_NJUDGERD = $(SRC_NJUDGERD:../src/njudgerd/%.cpp=./objects/njudgerd/%.o)
OBJ_NCHECKER = $(SRC_NCHECKER:../src/nchecker/%.cpp=./objects/nchecker/%.o)

.PHONY: all clean

all: $(TARGETS)

$(OBJ_COMMON_C): ./objects/common/%.o:../src/common/%.c
	-@mkdir -p ./objects/common
	$(CXX) $(CXXFLAGS) -o $@ -c $^
$(OBJ_COMMON): ./objects/common/%.o:../src/common/%.cpp
	-@mkdir -p ./objects/common
	$(CXX) $(CXXFLAGS) -o $@ -c $^
$(OBJ_NJUDGER): ./objects/njudger/%.o:../src/njudger/%.cpp
	-@mkdir -p ./objects/njudger
	$(CXX) $(CXXFLAGS) -o $@ -c $^
$(OBJ_NJUDGERD): ./objects/njudgerd/%.o:../src/njudgerd/%.cpp
	-@mkdir -p ./objects/njudgerd
	$(CXX) $(CXXFLAGS) -o $@ -c $^
$(OBJ_NCHECKER): ./objects/nchecker/%.o:../src/nchecker/%.cpp
	-@mkdir -p ./objects/nchecker
	$(CXX) $(CXXFLAGS) -o $@ -c $^

njudger: $(OBJ_COMMON_C) $(OBJ_COMMON) $(OBJ_NJUDGER)
	$(CXX) $(LDFLAGS) -o $@ $(OBJ_NJUDGER) $(OBJ_COMMON) $(OBJ_COMMON_C) $(LBLIBS)
njudgerd: $(OBJ_COMMON_C) $(OBJ_COMMON) $(OBJ_NJUDGERD)
	$(CXX) $(LDFLAGS) -o $@ $(OBJ_NJUDGERD) $(OBJ_COMMON) $(OBJ_COMMON_C) $(LBLIBS)
nchecker: $(OBJ_COMMON_C) $(OBJ_COMMON) $(OBJ_NCHECKER)
	$(CXX) $(LDFLAGS) -o $@ $(OBJ_NCHECKER) $(OBJ_COMMON) $(OBJ_COMMON_C) $(LBLIBS)

clean:
	-rm -rf $(TARGETS) ./objects