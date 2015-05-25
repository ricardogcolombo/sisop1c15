CC=g++
CFLAGS=-g -Wall -pedantic -m32 -lstdc++ -I../include/

all: ej4

ej4: ej4.o ../src/hdd.o ../src/mbr.o ../src/pentry.o ../src/ext2fs.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

ej4.o: ej4.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -c $^ -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -c $^ -o $@

clean:
	rm -rf *.o
	rm -rf ej4
