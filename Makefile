.PHONY: all clean

CXX=g++ -g3 -o

all: programa.cpp
	$(CXX) programa programa.cpp

clean:
	rm -f *.o programa
