CXX=g++
CXXFLAGS=-I. -Wall -Wall -O0 -ggdb3
DEPS=main.hpp

all: main

main: main.o
	$(CXX) -o $@ $^

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

clean:
	rm -f *.o main