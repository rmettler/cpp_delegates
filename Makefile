COMPILER_KIT = GNU

CXX_GNU = g++
CXXFLAGS_GNU = -std=c++14 -Wall -Werror -Wextra -pedantic
LDFLAGS_GNU = 

CXX = $(CXX_$(COMPILER_KIT))
CXXFLAGS = $(CXXFLAGS_GNU_$(COMPILER_KIT))
LDFLAGS = $(LDFLAGS_GNU_$(COMPILER_KIT))

TEST_EXE = test.exe

INCLUDES = -I./include -I./test/thirdparty
DEFINES = 

MAIN_SOURCE = test/test_main.cpp
SOURCES = \
	test/test_delegate.cpp

MAIN_OBJECT = $(MAIN_SOURCE:.cpp=.o)
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TEST_EXE)

test: $(TEST_EXE)
	@echo "[run]     ./$(TEST_EXE)"
	@./$(TEST_EXE)

clean:
	rm -rf $(TEST_EXE) $(OBJECTS)

clean.all:
	rm -rf $(TEST_EXE) $(MAIN_OBJECT) $(OBJECTS)

$(TEST_EXE): $(MAIN_OBJECT) $(OBJECTS)
	@echo "[link]    $(CXX) $(LDFLAGS) $(MAIN_OBJECT) $(OBJECTS) -o $@"
	@$(CXX) $(LDFLAGS) $(MAIN_OBJECT) $(OBJECTS) -o $@

%.o: %.cpp
	@echo "[compile] $(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@