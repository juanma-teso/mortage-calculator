.PHONY: all clean

CXX=g++ -I. -g3 -o

all: programa.cpp
	$(CXX) programa programa.cpp

clean:
	rm -f *.o programa
