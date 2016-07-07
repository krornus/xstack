CC=gcc
DEPS=xstack.c
FLAGS=-D _XOPEN_SOURCE

INC=`pkg-config --cflags --libs x11` 
CFLAGS=-I. $(INC) -lXtst $(FLAGS) 

ODIR=obj
_OBJ=xstack-server.o xstack.o 
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

xstack-server: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)


clean: 
	rm -f $(ODIR)/*.o xstack-server 
