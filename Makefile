CXX = g++
CC = gcc
CXXFLAGS = -Wall -g -Wno-unused-variable -Wno-delete-non-virtual-dtor -Wno-misleading-indentation -std=gnu++17
CFLAGS = -Wall -g -Wno-unused-variable -std=gnu11
LDFLAGS =
GTEST = -lgtest -lgtest_main -lpthread

# Path: src/
SRC = $(wildcard src/*.cpp src/*.c)
TEST = $(wildcard tests/*.cpp)

# Path: bin/
OBJ = $(patsubst src/%.cpp, bin/%.o, $(SRC))
OBJ_C = $(patsubst src/%.c, bin/%.o, $(SRC))
OBJ_TEST = $(patsubst tests/%.cpp, bin/%.o, $(TEST))
TESTS = Client

all: dir client

dir:
	@test -d bin || mkdir bin

bin/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(GTEST)

bin/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

bin/%.o: tests/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# deprecated
# client: clean $(OBJ) $(OBJ_C)
# 	$(CXX) $(CXXFLAGS) bin/*.o -o $@ $(LDFLAGS)

client: $(OBJ) $(OBJ_C) $(OBJ_TEST)
	$(CXX) $(CXXFLAGS) bin/*.o -o $@ $(LDFLAGS) $(GTEST)

test_all: client
	./client test

clean:
	rm -rf bin/* client