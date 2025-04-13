CC = g++
CFLAGS = -std=c++11 -Wall -Wextra -O2 -fopenmp -march=native -mavx -mavx2 -ffast-math
LIBS = -lsfml-graphics -lsfml-window -lsfml-system
TARGET = mandelbrot

OBJECTS = main.o Array/Array.o AVX/AVX.o Graphics/Graphics.o Naive/Naive.o

all: build
	./$(TARGET)

build: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJECTS)

.PHONY: all clean build run