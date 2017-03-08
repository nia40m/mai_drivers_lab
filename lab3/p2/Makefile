all: n4_queue
	gcc n4_queue.o test.c -pthread

n4_queue:
	gcc -pthread -c n4_queue.c

clean:
	rm *.o
	rm *.out