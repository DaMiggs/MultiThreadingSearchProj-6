CC = gcc
CFLAGS = -I. -g -w -pthread
DEPS = pa6.h
OBJ = PA6.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

PA6X: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
		rm -rf PA6X
		rm -rf *.o

t:
		make
		./PA6X -verbose -nthreads 16 -len 6,8,15 < puzzle
		
x:
		make
		./PA6X -verbose -time -nthreads 16 -len 8,12,15 < puzzle
		
y:
		make
		./PA6X -verbose -nthreads 16 < puzzle