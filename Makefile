CXX=g++
CXXFLAGS= -std=c++11 -Og -g -m32 -Wall -Werror -Wextra -pedantic
FDIR = forth

FDEPS = intmath.h logic.h stackmanip.h strmanip.h output.h memory.h rstack.h
DEPS = sys.h stack.h Function.h
DEPS += $(patsubst %, $(FDIR)/%, $(FDEPS))

FOBJ = intmath.o logic.o output.o stackmanip.o strmanip.o memory.o rstack.o
OBJ = main.o stack.o Function.o
OBJ += $(patsubst %, $(FDIR)/%, $(FOBJ))

%.o: %.c $(DEPS)
		$(CXX) -c -o $@ $< $(CXXFLAGS)

mxf: $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS)
	rm -f *.o $(FDIR)/*.o
