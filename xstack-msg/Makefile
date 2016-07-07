CC=gcc
DEPS=
FLAGS=-std=c99 

INC=
CFLAGS=-I. $(INC) $(FLAGS) 

ODIR=obj
_OBJ=xstack-msg.o 
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

xstack-msg: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)


clean: 
	rm -f $(ODIR)/*.o xstack-msg 
