#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void win() {
    system("/bin/sh");
}

void lose() {
    printf("haha you lost lolxd\n");
}

int main() {
    char *name = (char *)malloc(sizeof(int)); // 4 bytes
    int *p2 = (void *)malloc(sizeof(void *)); // 4 bytes
    void* losef = (void*)&lose;

    memcpy(p2, &losef, sizeof(void (*)()));

    printf("Enter your name: ");
    scanf("%s", (char *)name);
    printf("Hello %s\nLet's see if your\'e a winner\n", (char *)name);
    
    ((void (*)())(*p2))();
    free(name);
    free(p2);
    return 0;
}