calc: calc_api.c libfuncs.so
	gcc calc_api.c -o calc -ldl

libfuncs.so: calc_funcs.o
	gcc -shared calc_funcs.o -o libfuncs.so

calc_funcs.o: calc_funcs.c
	gcc calc_funcs.c -fPIC -c
