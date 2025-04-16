CC = gcc
CFLAGS = -c -O3 -mavx -mavx2 -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline		\
	-Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default		    	\
	-Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy            \
	-Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers \
	-Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing   \
	-Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE


# CFLAGS = -Wall -Wextra -O2 -fopenmp -march=native -mavx -mavx2 -ffast-math -Iinclude
LIBS = -lsfml-graphics -lsfml-window -lsfml-system
TARGET = mandelbrot

SRCS = src/Array.cpp src/Naive.cpp src/AVX.cpp src/Graphics.cpp src/main.cpp

OBJECTS = obj/Array.o obj/Naive.o obj/AVX.o obj/Graphics.o obj/main.o

all: build
	./$(TARGET)

build: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

obj/%.o: src/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(TARGET) $(OBJECTS)

.PHONY: all clean build run