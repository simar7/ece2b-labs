simple_ls.out: simple_ls.o
	gcc -o simple_ls.out simple_ls.o
simple_ls.o: simple_ls.c
	gcc -c simple_ls.c
debug:
	gcc -g simple_ls.c -o simple_ls.out
clean:
	rm -f *.o *.out
