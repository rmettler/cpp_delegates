COMPILER_KIT = GNU

CXX_GNU = g++
CXXFLAGS_GNU = -std=c++14 -Wall -Werror -Wextra -pedantic
LDFLAGS_GNU = 

CXX = $(CXX_$(COMPILER_KIT))
CXXFLAGS = $(CXXFLAGS_GNU_$(COMPILER_KIT))
LDFLAGS = $(LDFLAGS_GNU_$(COMPILER_KIT))

TEST_EXE = test.exe
BUILD_DIR = ./build

INCLUDES = -I./include -I./test/thirdparty
DEFINES = 

MAIN_SRC = test/test_main.cpp
SRCS = \
	test/test_function_ptr.cpp \
	test/test_bad_delegate_call.cpp \
	test/delegate/test_delegate.cpp \
	test/delegate/test_delegate/test_declarations.cpp \
	test/delegate/test_delegate/test_explicit_instantiations.cpp \
	test/delegate/test_delegate/test_default_construction.cpp \
	test/delegate/test_delegate/test_create.cpp

MAIN_OBJ = $(MAIN_SRC:%.cpp=$(BUILD_DIR)/%.o)
OBJS = $(SRCS:%.cpp=$(BUILD_DIR)/%.o)
MAIN_DEP = $(MAIN_SRC:%.cpp=$(BUILD_DIR)/%.d)
DEPS = $(SRCS:%.cpp=$(BUILD_DIR)/%.d)

all: $(TEST_EXE)

test: $(TEST_EXE)
	@echo "[run]     ./$(TEST_EXE)"
	@./$(TEST_EXE)

$(TEST_EXE): $(BUILD_DIR)/$(TEST_EXE)
	@echo "[move]    mv -f $(BUILD_DIR)/$(TEST_EXE) $(TEST_EXE)"
	@mv -f $(BUILD_DIR)/$(TEST_EXE) $(TEST_EXE)

$(BUILD_DIR)/$(TEST_EXE): $(MAIN_OBJ) $(OBJS)
	@mkdir -p $(@D)
	@echo "[link]    $(CXX) $(LDFLAGS) $(MAIN_OBJ) $(OBJS) -o $@"
	@$(CXX) $(LDFLAGS) $(MAIN_OBJ) $(OBJS) -o $@

-include $(DEPS)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	@echo "[compile] $(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -c $< -o $@

.PHONY: clean clean.all

clean:
	rm -rf $(TEST_EXE) $(BUILD_DIR)/$(TEST_EXE) $(OBJS) $(DEPS)

cleanall:
	rm -rf $(TEST_EXE) $(BUILD_DIR)
