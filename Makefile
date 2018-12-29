all: slang slang.a slang.so

slang: slang.o main.o
	g++ -o slang slang.o main.o -s

slang.a: slang.o lib.o
	ar cr libslang.a slang.o lib.o

slang.so: slang.o lib.o
	gcc -shared -o libslang.so slang.o lib.o

slang.o: slang.cpp main.hpp slang.hpp
	g++ -fPIC -std=c++11 -Wall -fexceptions -O2 -c slang.cpp -o slang.o

main.o: main.cpp main.hpp
	g++ -std=c++11 -Wall -fexceptions -O2 -c main.cpp -o main.o

lib.o: lib.cpp main.hpp slang.hpp
	g++ -fPIC -std=c++11 -Wall -fexceptions -O2 -c lib.cpp -o lib.o

install: slang slang.a
	cp slang /usr/bin/
	cp slang.a /usr/lib/

clean:
	rm -f slang *.o libslang.a libslang.so
