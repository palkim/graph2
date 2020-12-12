all:
	g++-10 -O3 *.cpp -c -std=c++11 -march=native -ljpeg -lm -g
	g++-10 *.o -o raytracer -lpthread -ljpeg -lm

clean:
	rm -f *.o
