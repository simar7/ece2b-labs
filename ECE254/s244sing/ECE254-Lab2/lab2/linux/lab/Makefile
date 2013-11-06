producer.out: producer.o
	gcc -o producer.out producer.o
producer.o: producer.c
	gcc -c producer.c
debug:
	gcc -g producer.c -o producer.out
clean:
	rm -f *.o *.out
