#include <stdio.h>
#include <magma.h>
#include <magma_lapack_v2.h>

void init() {
    magma_init();
}

int main(int argc, const char * argv[]) {
    init();
    printf("Hello world!");
    
    return 0;
}
