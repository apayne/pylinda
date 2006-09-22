all: mandelbrot

mandelbrot: mandelbrot.c
	gcc -g -O3 -ltcl8.3 -ltk8.3 -L/usr/lib/python2.3/ -lpython2.3 -I/usr/include/python2.3/ -o mandelbrot mandelbrot.c

clean:
	rm -f mandelbrot
