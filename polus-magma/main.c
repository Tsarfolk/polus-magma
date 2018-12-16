#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <limits.h>

#define RELEASE
#ifndef RELEASE
int iterationCount = 1;
#else
int iterationCount = INT_MAX;
#endif

#ifdef RELEASE
#include "magma_v2.h"
#include "magma_lapack.h"
#include "cuComplex.h"
#endif

double randDenominator = 0;

double randNumber() {
    return (double)rand() / randDenominator;
}

#ifdef RELEASE
void init() {
    magma_init();
}

void print(const magmaDoubleComplex *A, int size) {
    magma_zprint(size, size, A, size);
}

void debug_print() {
    printf("Hello world!");
}

void calculate(int size) {
    magma_int_t mSize = size * size;
    
    magmaDoubleComplex *matrix;
    magma_zmalloc_pinned(&matrix, mSize);
    
    print(matrix, size);
    
    double max = 0;
    for (int i = 0; i < size; i++) {
        int index = i + size * i;
        double real = randNumber();
        double imaginary = randNumber();
        magmaDoubleComplex number = make_cuDoubleComplex(real, imaginary);
        matrix[index] = number;
        if (MAGMA_Z_ABS(number) > max) {
            max = number;
        }
    }

    printf("%d", max);

    for(int i = 0; i < size; ++i) {
        int index = i + size * i;
        matrix[index] = MAGMA_Z_ADD(matrix[index], MAGMA_Z_MAKE(max, max));
    }

    print(matrix, size);
}
#endif

void start() {
#ifdef RELEASE
    init();
#endif
    srand((unsigned int)time(NULL));
    randDenominator = RAND_MAX / 10000;
    
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
    
    count = iterationCount < count ? iterationCount : count;
    for (int i = 0; i < count; i++) {
        int size = sizes[i];
        
#ifdef RELEASE
        calculate(size);
#endif
    }
}

int main(int argc, const char * argv[]) {
    start();
    
    return 0;
}
