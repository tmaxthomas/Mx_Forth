CC = gcc
CFLAGS = -std=c99 -g -m32 -lm -Wall -Werror
FDIR = forth

FDEPS = intmath.h input.h logic.h stackmanip.h strmanip.h output.h memory.h rstack.h control.h misc.h
DEPS = sys.h stack.h debug.h
DEPS += $(patsubst %, $(FDIR)/%, $(FDEPS))

FOBJ = intmath.o input.o logic.o output.o stackmanip.o strmanip.o memory.o rstack.o control.o misc.o
OBJ = main.o stack.o debug.o
OBJ += $(patsubst %, $(FDIR)/%, $(FOBJ))

%.o: %.c $(DEPS)
		$(CC) -c -o $@ $< $(CFLAGS)

mxf: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
	rm -f *.o $(FDIR)/*.o
