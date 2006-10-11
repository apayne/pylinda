all: mandelbrot mandelbrot++

mandelbrot: mandelbrot.c
	gcc -g -O3 -llinda -lexpat -lpthread -o mandelbrot mandelbrot.c

mandelbrot++: mandelbrot.cpp
	g++ -g -O3 -llinda++ -llinda -lexpat -lpthread -o mandelbrot++ mandelbrot.cpp

clean:
	rm -f mandelbrot
