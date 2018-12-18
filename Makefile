CC = gcc
CFLAGS = -std=c99 -g -m32 -Wall -Werror -fPIE
LDFLAGS = -lm -pie
FDIR = forth

DEPS = $(wildcard $(FDIR/*.h)) $(wildcard *.h)
SRC = $(wildcard $(FDIR)/*.c) $(wildcard *.c)
OBJ = $(SRC:%.c=%.o)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

mxf: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)
	rm -f *.o $(FDIR)/*.o
