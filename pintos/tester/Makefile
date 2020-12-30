OBJECTS = 20170364.o list.o hash.o bitmap.o hex_dump.o

testlib: $(OBJECTS)
	gcc -o testlib $(OBJECTS)

20170364.o: 20170364.h 20170364.h
	gcc -c 20170364.c

list.o: list.h list.c
	gcc -c list.c

hash.o: hash.h hash.c
	gcc -c hash.c

bitmap.o: bitmap.h bitmap.c
	gcc -c bitmap.c

hex_dump.o: hex_dump.h hex_dump.c
	gcc -c hex_dump.c

clean:
	rm *.o testlib


