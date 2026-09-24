CXX=g++
CXXFLAGS=-I. -Wall -O0 -ggdb3
DEPS=main.hpp

all: p1 p2 p3

p1: p1.o
	$(CXX) -o $@ $^

p2: p2.o
	$(CXX) -o $@ $^

p3: p3.o
	$(CXX) -o $@ $^

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

clean:
	rm -f *.o p1 p2 p3