all: clear
	gcc -DADD_ -std=c99 -I /usr/local/cuda-9.1/include -I /polusfs/soft/magma-2.4.0_open_blas/include -L/polusfs/soft/magma-2.4.0_open_blas/lib *.c -lm -lmagma -lmagma_sparse 
push:
	bsub -gpu- -o result.txt ./a.out
clear: 
	rm -f a.out core.*
