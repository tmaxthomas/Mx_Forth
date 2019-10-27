CC 		= gcc
CFLAGS  = -std=gnu99 -g -O0 -W -Werror -fPIE
LDFLAGS = -lm -pie -lncurses -lreadline

TARGET  = $(BINDIR)/mxf

SRCDIR  = src
OBJDIR 	= obj
BINDIR  = bin

SRC     = $(shell find $(SRCDIR) -type f -name *.c)
HEADER  = $(shell find $(SRCDIR) -type f -name *.h)
OBJ     = $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

all: directories $(TARGET)

directories:
	@mkdir -p $(BINDIR)
	@mkdir -p $(OBJDIR)

obj_clean:
	@rm -rf $(BUILDDIR)

clean: obj_clean
	@rm -rf $(BINDIR)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(HEADER)
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: all obj_clean clean directories
