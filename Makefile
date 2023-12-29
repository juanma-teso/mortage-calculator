.PHONY: all clean

CXX=g++ -g3 -o

all: mortage.cpp
	$(CXX) program mortage.cpp

clean:
	rm -f *.o program
