#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <limits.h>

//#define RELEASE
//#ifndef RELEASE
int iterationCount = 1;
//#else
//int iterationCount = INT_MAX;
//#endif

//#ifdef RELEASE
#include "magma_v2.h"
#include "magma_lapack.h"
#include "cuComplex.h"
//#endif

double randDenominator = 0;

double randNumber() {
    return (double)rand() / randDenominator;
}

void print(const magmaDoubleComplex* A, magma_int_t size) {
    magma_zprint(size, size, A, size);
}

//#ifdef RELEASE
void init() {
    magma_init();
}

void debug_print() {
    printf("Hello world!");
}

void calculate(int size) {
    magma_int_t zSize = size;
    printf("zSize is %d\n", zSize);
    magma_int_t mSize = zSize * zSize;
    
    printf("Will init matrix of size %d\n", mSize);
    magmaDoubleComplex *matrix;
    magma_int_t result = magma_malloc_pinned( (void**) &matrix, size * sizeof(magmaDoubleComplex));
    if (result) {
        printf("Error on allocation, result %d", result);
        return;
    }
    
//    magma_int_t idist = 1;
//    magma_int_t ISEED[4] = {0, 0, 0, 1};
//
//    lapackf77_zlarnv(&idist, ISEED, &mSize, matrix);
    
    printf("Allocated with elements count %d\n", mSize);
    double max = 0;
    for (int i = 0; i < size * size; i++) {
        int index = i;
        double real = randNumber();
        double imaginary = randNumber();
        magmaDoubleComplex number = make_cuDoubleComplex(real, imaginary);
        matrix[index] = number;
        printf("real: %lf, imaginary: %lf, index: %d", real, imaginary, index);
        double distance = MAGMA_Z_ABS(number);
        if (distance > max) {
            max = distance;
        }
    }
    
    printf("max is %d", max);
    print(matrix, zSize);

    for(int i = 0; i < size; ++i) {
        int index = i + size * i;
        matrix[index] = MAGMA_Z_ADD(matrix[index], MAGMA_Z_MAKE(max, max));
    }

    print(matrix, size);
    
//    magmaDoubleComplex *lWork, *tau;
//    magma_int_t nb = magma_get_zgeqrf_nb(size, size);
//    magma_int_t lWorkSize = nb * size;
//    magma_cmalloc_pinned(&lWork, lWorkSize);
//    magma_cmalloc_pinned(&tau, size);
//
//    magma_int_t info;
//    magma_int_t result = magma_zgeqrf(size, size, matrix, size, tau, lWork, lWorkSize, &info);
//    print(matrix, size);
//    printf("Info is %d", info);
//    printf("Result is %d", info);
//
//    magma_free_pinned(matrix);
}
//#endif

void start() {
    //#ifdef RELEASE
    printf("Will init\n");
    init();
    printf("Did init\n");
    //#endif
    srand((unsigned int)time(NULL));
    randDenominator = 1000;

    printf("Will generate\n");
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
        printf("current size: %d\n", currentSize);
    }
//
    count = iterationCount < count ? iterationCount : count;
    printf("Iterator count is %d\n", count);
    for (int i = 0; i < count; i++) {
        //#ifdef RELEASE
        int size = sizes[i];
        printf("Count is: %d\n", count);
        printf("Size is: %d\n", size);
        calculate(size);
        //#endif
    }
}

int main(int argc, const char * argv[]) {
    start();
//    debug_print();
    
    return 0;
}
