#include <stdio.h>
#include "magma.h"
#include "magma_lapack.h"

void init() {
    magma_init();
}

int main(int argc, const char * argv[]) {
    init();
    printf("Hello world!");
    
    return 0;
}
