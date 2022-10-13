#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    void (*fp)();
    char name[12];
} person;

void win() {
    system("/bin/sh");
}

void hello() {
    printf("Hello world, I am guest3 xdlol :)\n");
}

int main() {
    char option;
    person *guest1;
    person *guest2;
    person *guest3;
    int *size;

    guest1 = (person *)malloc(sizeof(person));
    guest2 = (person *)malloc(sizeof(person));
    guest3 = (person *)malloc(sizeof(person));

    guest3->fp = &hello;

    printf("enter name for guest1: ");
    fgets(guest1->name, 28, stdin);
    // the next chunk's size:
    size = (int *)(guest1->name + 24);
    // get size of chunk via metadata (while ignoring the three least significant bits) 
    // subtract size of metadata and alignment to obtain buffer size:
    fgets(guest2->name, (*size & 0xfffffff8) - 20, stdin); 

    guest3->fp();
    free(guest1);
    free(guest2);
    free(guest3);
    return 0;
}