CC=g++
CFLAGS=-std=c++11 -I.
OBJ = nonogram.o

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

nonogram: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f nonogram.o nonogram
