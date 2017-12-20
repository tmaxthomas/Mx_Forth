CXX=g++
CXXFLAGS= -std=c++11 -Og -g -m32 -Wall -Werror -Wextra -pedantic
DEPS = sys.h stack.h Function.h
OBJ = main.o stack.o Function.o

%.o: %.c $(DEPS)
		$(CXX) -c -o $@ $< $(CXXFLAGS)

mxf: $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS)
	rm -f *.o
