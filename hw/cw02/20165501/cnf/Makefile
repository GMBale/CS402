CC=g++
CFLAGS=-I.
OBJ = cnf.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

cnf: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f cnf.o cnf
