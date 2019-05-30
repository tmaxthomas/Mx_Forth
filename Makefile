CC 		= gcc
CFLAGS  = -std=gnu99 -g -O0 -W -Werror -fPIE
LDFLAGS = -lm -pie
FDIR    = forth

DEPS    = $(wildcard $(FDIR)/*.h) $(wildcard *.h)
SRC     = $(wildcard $(FDIR)/*.c) $(wildcard *.c)
OBJ     = $(SRC:%.c=%.o)

all: mxf obj_clean

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

mxf: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)

.PHONY: obj_clean clean

obj_clean:
	rm -f $(OBJ)

clean: obj_clean
	rm -f mxf
