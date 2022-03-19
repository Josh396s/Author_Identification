CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic -O2

all: identify

identify: identify.o pq.o ht.o bf.o bv.o node.o speck.o parser.o text.o
	$(CC) -o identify identify.o pq.o ht.o bf.o bv.o node.o speck.o parser.o text.o -lm

identify.o: identify.c
	$(CC) $(CFLAGS) -c identify.c

ht.o: ht.c
	$(CC) $(CFLAGS) -c ht.c

bf.o: bf.c
	$(CC) $(CFLAGS) -c bf.c

bv.o: bv.c
	$(CC) $(CFLAGS) -c bv.c

node.o: node.c
	$(CC) $(CFLAGS) -c node.c

speck.o: speck.c
	$(CC) $(CFLAGS) -c speck.c

parser.o: parser.c
	$(CC) $(CFLAGS) -c parser.c

text.o: text.c
	$(CC) $(CFLAGS) -c text.c

pq.o: pq.c
	$(CC) $(CFLAGS) -c pq.c

clean:
	rm -f identify *.o ht *.o bf *.o bv *.o node *.o speck *.o parser *.o text *.o pq *.o

format:
	clang-format -i -style=file *.[ch]
