#! bin/sh

gcc -I /polusfs/soft/magma-2.4.0_open_blas/include -с example.c -o example.o
gcc -L /polusfs/soft/magma-2.4.0_open_blas/lib example.o -lmagma -o example.out
