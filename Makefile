CC = g++

FLAGS = -c -O3 -fopenmp -march=native -mavx -mavx2 -O3

all: hello
		Mandelbrot

hello: main.o
	$(CC) main.o -o Mandelbrot

main.o: main.cpp
	$(CC) $(FLAGS) main.cpp -o main.o

clean:
	rm -f *.o *.exe *.exe.log *.exe.log.dot