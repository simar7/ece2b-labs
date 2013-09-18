simple_ls.out: simple_ls.o
	gcc -o simple_ls.out simple_ls.o
simple_ls.o: simple_ls.c
	gcc -c simple_ls.c
debug:
	gcc -g simple_ls.c -o simple_ls.out
extra:
	gcc -g simple_ls.c -o simple_ls.out -Wall
clean:
	rm -f *.o *.out
