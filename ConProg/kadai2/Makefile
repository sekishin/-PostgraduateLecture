all: ProdCons.c CorrectData.c
	gcc -o ProdCons ProdCons.c
	gcc -o CorrectData CorrectData.c

calc_time: ProdCons.c
	gcc -o ProdCons -DTIME ProdCons.c
