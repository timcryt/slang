slang: slang.o
	g++ -o slang slang.o -s

slang.o: slang.cpp
	g++ -std=c++11 -Wall -fexceptions -O2 -c slang.cpp -o slang.o

install: slang
	cp slang /usr/bin/

clean:
	rm -f slang slang.o 