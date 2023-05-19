CXX = g++
CC = gcc
CXXFLAGS = -Wall -g -Wno-unused-variable -std=gnu++14
CFLAGS = -Wall -g -Wno-unused-variable -std=gnu11
LDFLAGS =

# Path: src/
SRC = $(wildcard src/*.cpp src/*.c)

# Path: bin/
OBJ = $(patsubst src/%.cpp, bin/%.o, $(SRC))
OBJ_C = $(patsubst src/%.c, bin/%.o, $(SRC))

all: dir client

dir:
	@test -d bin || mkdir bin

bin/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

bin/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

client: $(OBJ) $(OBJ_C)
	$(CXX) $(CXXFLAGS) bin/*.o -o $@ $(LDFLAGS)

clean:
	rm -rf bin/* client