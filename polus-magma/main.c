#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "magma_v2.h"
#include "magma_lapack.h"

void init() {
    magma_init();
}

void print(const magmaFloatComplex *A, int size) {
    magma_cprint(size, size, A, size);
}

void debug_print() {
    printf("Hello world!");
}

int main(int argc, const char * argv[]) {
    init();
    
    int sizes[100];
    int i = 0;
    int currentSize = 1;
    int count = 0;
    int targetSize = 10e4;
    while (1) {
        int firstSize = currentSize * 5;
        int secondSize = firstSize * 2;
        sizes[i++] = firstSize;
        sizes[i++] = secondSize;
        currentSize = secondSize;
        count += 2;
        if (secondSize > targetSize) {
            break;
        }
    }
    
    for (int i = 0; i < 1; i++) {
        int size = sizes[i];
        magma_int_t mSize = size * size;
        
        magmaFloatComplex *matrix;
        magma_cmalloc_pinned(&matrix, mSize);

        print(matrix, size);
//        magma_int_t ione     = 1;
//        magma_int_t ISEED[4] = {0 ,0 ,0 ,1};
//        lapackf77_clarnv( &ione, ISEED, &size, matrix );
        
//        float m = 0;
//        for (int i = 0; i < size; ++i){
//            m = MAGMA_C_ABS(matrix[i]) > m ? MAGMA_C_ABS(matrix[i]) : m;
//        }
//        for(int i = 0; i < size; ++i){
//            matrix[i + size * i] = MAGMA_C_ADD(matrix[i + size * i],  MAGMA_C_MAKE(m, m));
//        }
//
//        print(matrix, size);
        
        

    }
    return 0;
}
