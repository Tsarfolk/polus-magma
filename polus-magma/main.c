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
#endif

float randDenominator = 0;

float randNumber() {
    return (float)rand() / randDenominator;
}

#ifdef RELEASE
void init() {
    magma_init();
}

void print(const magmaFloatComplex *A, int size) {
    magma_cprint(size, size, A, size);
}

void debug_print() {
    printf("Hello world!");
}

void calculate(int size) {
    int size = sizes[i];
    magma_int_t mSize = size * size;
    
    magmaFloatComplex *matrix;
    magma_cmalloc_pinned(&matrix, mSize);
    
    float max = 0;
    for (int i = 0; i < size; i++) {
        float number = randNumber();
        matrix[i + size * i] = number;
        if (number > max) {
            max = number;
        }
    }
    
    printf("%d", max);
    
    for(int i = 0; i < size; ++i) {
        int index = i + size * i;
        matrix[index] = MAGMA_C_ADD(matrix[index],  MAGMA_C_MAKE(max, max));
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
