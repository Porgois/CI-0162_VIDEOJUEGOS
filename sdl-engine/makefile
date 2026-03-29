CC=g++
STD=-std=c++17
CFLAGS=-Wall -Wextra
INC_PATH=-I"./libs/"
SRC=src/*.cpp src/game/*.cpp src/e.c.s./*.cpp src/assetManager/*.cpp src/controllerManager/*.cpp
LFLAGS=-lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -llua
EXEC=engine

build:
	$(CC) $(CFLAGS) $(STD) $(DEFINES) $(INC_PATH) $(SRC) $(LFLAGS) -o $(EXEC)

execute:
	clear ; make clean ; make ; make run

run:
	./$(EXEC)

clean:
	rm $(EXEC)