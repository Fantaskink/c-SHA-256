IDIR =./include
CC=gcc
CFLAGS =-I$(IDIR) -O2 -Wall 

ODIR=obj
LDIR=lib

_DEPS = 
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: src/%.c $(DEPS)
				$(CC) -c -o $@ $< $(CFLAGS)

sha256: $(OBJ)
				$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
				rm -f $(ODIR)/*.o *~ core $(IDIR)/*~
