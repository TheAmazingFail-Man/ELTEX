calc: calc_api.c libfuncs.a
	gcc calc_api.c -o calc -L. -lfuncs

libfuncs.a: calc_funcs.o
	ar rc libfuncs.a calc_funcs.o

calc_funcs.o: calc_funcs.c
	gcc -c calc_funcs.c -o calc_funcs.o
