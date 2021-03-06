#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <limits.h>

#define RELEASE
#ifndef RELEASE
int iterationCount = 8;
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
    return (double) rand();
//    double randNum = (double) rand();
//    if (randNum > 100000) {
//        return randNum / randDenominator;
//    } else {
//        return randNum;
//    }
}

void print(const magmaDoubleComplex* A, magma_int_t size) {
//    magma_zprint(size, size, A, size);
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
    
    // -------- alloc --------
    printf("Will init matrix of size %lf\n", mSize);
    magmaDoubleComplex *matrix;
    magma_int_t result = magma_malloc_pinned( (void**) &matrix, mSize * sizeof(magmaDoubleComplex));
    if (result) {
        printf("[matrix] Error on allocation, result %d", result);
        return;
    }
    
//    magma_int_t idist = 1;
//    magma_int_t ISEED[4] = {0, 0, 0, 1};
//
//    lapackf77_zlarnv(&idist, ISEED, &mSize, matrix);
    
    // -------- matrix population --------
    printf("Allocated with elements count %d\n", mSize);
    double maxValue = 0;
    for (int i = 0; i < mSize; i++) {
        double real = randNumber();
        double imaginary = randNumber();
        magmaDoubleComplex number = make_cuDoubleComplex(real, imaginary);
        matrix[i] = number;
        double distance = MAGMA_Z_ABS(number);
        if (distance > maxValue) {
            maxValue = distance;
        }
    }
    
    printf("max is %lf", maxValue);
    print(matrix, zSize);
//
    for(int i = 0; i < size; ++i) {
        int index = i + size * i;
        matrix[index] = MAGMA_Z_ADD(matrix[index], MAGMA_Z_MAKE(maxValue, maxValue));
    }

//    print(matrix, zSize);
//    // -------- actual work --------
//
    magmaDoubleComplex *lWork, *tau;
    magma_int_t zgeqrfNb = magma_get_zgeqrf_nb(zSize, zSize);
    magma_int_t lWorkSize = zgeqrfNb * zSize;

    // -------- alloc --------
    result = magma_malloc_pinned( (void**) &lWork, lWorkSize * sizeof(magmaDoubleComplex));
    if (result) {
        printf("Error on allocation, result %d", result);
        return;
    }
    result = magma_malloc_pinned( (void**) &tau, zSize * sizeof(magmaDoubleComplex));
    if (result) {
        printf("Error on allocation, result %d", result);
        return;
    }

    // -------- qr factorization --------
    magma_int_t info;
    result = magma_zgeqrf(zSize, zSize, matrix, zSize, tau, lWork, lWorkSize, &info);

    if (result || info) {
        printf("Error on qr factorization %d", result);
        printf("Info is %d", info);
        printf("Result is %d", info);
        return;
    }

    // -------- orthonormal columns `matrix` --------
    print(matrix, zSize);
    result = magma_zungqr2(zSize, zSize, zSize, matrix, zSize, tau, &info);

    if (result || info) {
        printf("Error on qr factorization %d", result);
        printf("Info is %d", info);
        printf("Result is %d", info);
        return;
    }
    print(matrix, zSize);

    // -------- dealloc --------
    magma_free_pinned(lWork);
    magma_free_pinned(tau);

    // -------- alloc --------
    magmaDoubleComplex *eigMatrix, *A, *eig;
    result = magma_malloc_pinned( (void**) &eigMatrix, mSize * sizeof(magmaDoubleComplex));
    if (result) {
        printf("Error on allocation, result %d", result);
        return;
    }
    result = magma_malloc_pinned( (void**) &A, mSize * sizeof(magmaDoubleComplex));
    if (result) {
        printf("Error on allocation, result %d", result);
        return;
    }
    result = magma_malloc_pinned( (void**) &eig, zSize * sizeof(magmaDoubleComplex));
    if (result) {
        printf("Error on allocation, result %d", result);
        return;
    }

    for(int i = 0; i < mSize; i++){
        eigMatrix[i] = MAGMA_Z_ZERO;
        A[i] = MAGMA_Z_ZERO;
    }
    for(int i = 0; i < zSize; i++){
        double real = (rand() % 5000) / 2500.0 - 1;
        double imaginary = (rand() % 5000) / 2500.0 - 1;
        magmaDoubleComplex complexNumber = MAGMA_Z_MAKE(real, imaginary);
        eig[i] = complexNumber;
        eigMatrix[i + zSize * i] = complexNumber;
    }
//
    magma_device_t device = 0;
    magma_getdevice(&device);
    magma_queue_t queue = NULL;
    magma_queue_create(device, &queue);

    magmablas_zgemm(MagmaNoTrans, MagmaNoTrans, zSize, zSize, zSize, MAGMA_Z_ONE, eigMatrix, zSize, matrix, zSize, MAGMA_Z_ZERO, A, zSize, queue);
    magmablas_zgemm(MagmaConjTrans, MagmaNoTrans, zSize, zSize, zSize, MAGMA_Z_ONE, matrix, zSize, A, zSize, MAGMA_Z_ZERO, A, zSize, queue);

    magma_int_t workSize = (1 + 3 * zgeqrfNb) * zSize;
    magmaDoubleComplex *wMatr = NULL, *workMatr, *VL, *VR;
    result = magma_malloc_pinned( (void**) &wMatr, zSize * sizeof(magmaDoubleComplex));
    if (result) {
        printf("Error on allocation, result %d", result);
        return;
    }
    result = magma_malloc_pinned( (void**) &workMatr, workSize * sizeof(magmaDoubleComplex));
    if (result) {
        printf("Error on allocation, result %d", result);
        return;
    }

    double *realWork;
    result = magma_malloc_pinned( (void**) &realWork, 2 * zSize * sizeof(magmaDoubleComplex));
    if (result) {
        printf("Error on allocation, result %d", result);
        return;
    }

    double gpuTimeStart = magma_sync_wtime (NULL);
    result = magma_zgeev_m(MagmaNoVec, MagmaNoVec, zSize, A, zSize, wMatr, VL, zSize, VR, zSize, workMatr, workSize, realWork, &info);
    double gpuTimeTotal = magma_sync_wtime (NULL) - gpuTimeStart;

    for(int i = 0; i < zSize; ++i){
        magmaDoubleComplex lambda = wMatr[i];
        int j = 0;
        while(MAGMA_Z_ABS(MAGMA_Z_SUB(eig[j], lambda)) > 0.0001 && j < zSize) {
            ++j;
        }
        if (j < zSize) {
            eig[j] = MAGMA_Z_ZERO;
        }
    }

    double zblas = magma_cblas_dznrm2(zSize, eig, 1);

    printf("Matrix dimension %d\n", zSize);
    printf("GPU time: %f\n", gpuTimeTotal);
    printf("Is correct: %s\n", fabs(zblas) < 0.0001 ? "yes": "no");
    printf("Norma: %f\n", zblas);
    printf("------------------------------------------------\n");

    magma_free_pinned(VL);
    magma_free_pinned(VR);
    magma_free_pinned(wMatr);
    magma_free_pinned(eig);
    magma_free_pinned(eigMatrix);
    magma_free_pinned(A);
    magma_free_pinned(lWork);
    magma_free_pinned(matrix);
}
//#endif

void start() {
    //#ifdef RELEASE
    init();
    
    //#endif
    srand((unsigned int)time(NULL));
    randDenominator = 10000;
    
    calculate(5000);
    return;

    printf("Will generate\n");
    int sizes[100];
    int i = 0;
    int currentSize = 1;
    int count = 0;
    int targetSize = 10e3;
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
    
    printf("Target sizes:\n");
    for (int i = 0; i < count; i++) {
        printf("%d\n", sizes[count]);
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
