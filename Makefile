CC = gcc
CFLAGS = -std=c99 -Og -g -m32 -Wall -Werror -Wextra -pedantic
FDIR = forth

FDEPS = intmath.h logic.h stackmanip.h strmanip.h output.h memory.h rstack.h
DEPS = sys.h stack.h
DEPS += $(patsubst %, $(FDIR)/%, $(FDEPS))

FOBJ = intmath.o logic.o output.o stackmanip.o strmanip.o memory.o rstack.o
OBJ = main.o stack.o
OBJ += $(patsubst %, $(FDIR)/%, $(FOBJ))

%.o: %.c $(DEPS)
		$(CC) -c -o $@ $< $(CFLAGS)

mxf: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
	rm -f *.o $(FDIR)/*.o
