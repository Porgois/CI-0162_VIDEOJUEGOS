CC=g++
STD=-std=c++17
CFLAGS=-Wall
INC_PATH=-I"./libs/"
SRC=src/*.cpp
LFLAGS=-llua

build:
	$(CC) $(CFLAGS) $(STD) $(INC_PATH) $(SRC) $(LFLAGS) -o luatest

run:
	./luatest

clean:
	rm luatest