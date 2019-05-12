all: slang libslang.a libslang.so

slang: compiler.o main.o vm.o
	g++ -o slang compiler.o main.o vm.o -s

libslang.a: compiler.o lib.o vm.o
	ar cr libslang.a compiler.o lib.o vm.o

libslang.so: compiler.o lib.o vm.o
	gcc -shared -o libslang.so compiler.o lib.o vm.o

compiler.o: compiler.cpp main.hpp slang.hpp
	g++ -fPIC -std=c++11 -Wall -fexceptions -O2 -c compiler.cpp -o compiler.o

vm.o: vm.cpp main.hpp slang.hpp
	g++ -fPIC -std=c++11 -Wall -fexceptions -O2 -c vm.cpp -o vm.o

main.o: main.cpp main.hpp
	g++ -std=c++11 -Wall -fexceptions -O2 -c main.cpp -o main.o

lib.o: lib.cpp main.hpp slang.hpp
	g++ -fPIC -std=c++11 -Wall -fexceptions -O2 -c lib.cpp -o lib.o

install: slang slang.a slang.so
	cp slang /usr/bin/
	cp libslang.a /usr/lib/
	cp libslang.so /usr/lib/
	cp slang.hpp /usr/include/

clean:
	rm -f slang *.o libslang.a libslang.so
