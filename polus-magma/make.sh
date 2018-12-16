#! bin/sh

gcc -I /polusfs/soft/magma-2.4.0_open_blas/include -с main.c -o main.o
gcc -L /polusfs/soft/magma-2.4.0_open_blas/lib main.o -lmagma -o main.out
