#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void get_shell(const char *perms) {
    int i;
    for(i = 1; i < 0x30; i++) {
        if(perms[i] == 0) {
            puts("You aren't privileged enough to control this machine");
            puts("But you can still enjoy our browser");
            return;
        }
    }
    puts("Hello, privileged user :). I know I can trust you with my shell");
    system("/bin/sh");
}

char query[0x30] __attribute__ ((aligned (0x10))) = { 0 };

int main() {
    char *perms;
    int i;
    unsigned long long *a = (unsigned long long *)&query[0x10];
    char *message = malloc(0x30);

    strcpy(message, "Loading the browser...");
    setbuf(stdout, NULL);

    puts(message);
    free(message);
    puts("Welcome to Spirit, the world's most vulnerable web browser");

    read(0, query, 0x40);
    free(a); 
    perms = malloc(0x30);
    memset(perms, 0, 0x30);
    while(1) {
        if(strncmp(query, "WEBSHELL", 8) == 0) {
            get_shell(perms);
        }
        if(strncmp(query, "EXIT", 4) == 0) {
            break;
        }
        if(strncmp(query, "SEARCH", 6) == 0) {
            puts("This service is unavailable due to maintenance");
        }
        fgets(query + 0x10, 0x30, stdin);
    }
    free(perms);
    return 0;
}