calc_api.o: calc_api.c
	gcc -c calc_api.c -o calc_api.o

calc_funcs.o: calc_funcs.c
	gcc -c calc_funcs.c -o calc_funcs.o

calc: calc_api.o calc_funcs.o
	gcc calc_api.o calc_funcs.o -o calc
