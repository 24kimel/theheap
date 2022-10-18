#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define DEBUG_LEAK 1

void secrect_technician_area() {
    system("/bin/sh");
}

char padding[0x100];
char* name;

int main() {
    puts("Welcome to my vending machine :)");
    name = (char*)malloc(0x420);
    char* which_drink = (char*)malloc(0x420);
    bool got_drink = false;
    int i = 0;

#if DEBUG_LEAK
    printf("Hope I'll not leave it in debug mode in production! %p %p %p\n", &which_drink, &main, &name);
#endif

    while (1) {
        puts("Name:");
        fgets(name, 0x441, stdin); // TODO remove my special DEBUG size :)
        if (++i >= 3) {
            printf("Goodbye!\n");
            break;
        }

        if (!got_drink) {
            puts("Drink:");
            fgets(which_drink, 0x420, stdin);
            printf("You got your drink\n");
            got_drink = true;
            free(which_drink);
        }

        puts("Now you can rename yourself twice ;)");
    }

    return 0;
}
