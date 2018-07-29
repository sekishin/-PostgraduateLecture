all: prog1.c prog2.c
	gcc -o prog1 prog1.c -lpthread
	gcc -o prog2 prog2.c -lpthread

debug_on: prog1.c
	gcc -DDEBUG -o prog1 prog1.c -lpthread

time: prog2.c CorrectData.c
	gcc -DTIME -o prog2 prog2.c -lpthread
	gcc -o CorrectData CorrectData.c

