FLAGS= -Wall -Werror -std=c11

mysync: mysync.o copy.o sync.o
	gcc mysync.o copy.o sync.o -o mysync ${FLAGS}

mysync.o: mysync.c mysync.h 
	gcc -c mysync.c ${FLAGS}

copy.o: copy.c mysync.h
	gcc -c copy.c ${FLAGS}

sync.o: sync.c mysync.h
	gcc -c sync.c ${FLAGS}


clean:
	rm *.o mysync