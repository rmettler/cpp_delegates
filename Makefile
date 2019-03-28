CXX = g++
CXXFLAGS = -std=c++14 -Wall -Werror -Wextra -pedantic
LDFLAGS = 

INCLUDES = -I./include -I./test/thirdparty
DEFINES = 

MAIN_SOURCE = test/test_main.cpp

SOURCES = \
	test/test_delegate.cpp

MAIN_OBJECT = $(MAIN_SOURCE:.cpp=.o)

OBJECTS = $(SOURCES:.cpp=.o)

all: test.exe

test: test.exe
	@./test.exe

clean:
	rm -rf test.exe $(OBJECTS)

clean.all:
	rm -rf test.exe $(MAIN_OBJECT) $(OBJECTS)

test.exe: $(MAIN_OBJECT) $(OBJECTS)
	@echo "[LD]  $(CXX) $(LDFLAGS) $(MAIN_OBJECT) $(OBJECTS) -o $@"
	@$(CXX) $(LDFLAGS) $(MAIN_OBJECT) $(OBJECTS) -o $@

%.o: %.cpp
	@echo "[CXX] $(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@