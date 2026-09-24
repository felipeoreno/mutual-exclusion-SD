CXX=g++
CXXFLAGS=-I. -Wall -Wextra -O3
DEPS=main.hpp

all: p1 p2 p3

p1 p2 p3: p1.o p2.o p3.o
	$(CXX) -o $@ $@.o

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

clean:
	rm -f *.o p1 p2 p3 output.txt