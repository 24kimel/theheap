#include <stdio.h>
#include <stdlib.h>

int main() {
    long* p1;
    long* p2;
    long* p3;
    int i;

    setbuf(stdout, NULL);
    
    void *ptrs[7];
    for (int i=0; i<7; i++) {
        ptrs[i] = malloc(8);
    }

    p1 = malloc(8);
    p2 = malloc(8);

    for (int i=0; i<7; i++) {
        free(ptrs[i]);
    }

    free(p1);
    free(p2);
    free(p1);

    for (int i=0; i<7; i++) {
        ptrs[i] = malloc(8);
    }

    p1 = malloc(8);
    malloc(8);
    p3 = malloc(8);
    printf("p1: %p, p3: %p\n", p1, p3);

    return 0;
}