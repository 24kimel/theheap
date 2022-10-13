#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUF_LEN 0x20

void get_shell(const char *perms) {
    int i;
    for(i = 1; i < BUF_LEN; i++) {
        if(perms[i] == 0) {
            puts("You aren't privileged enough to control this machine");
            puts("But you can still enjoy our browser");
            return;
        }
    }
    puts("Hello, privileged user :). I know I can trust you with my shell");
    system("/bin/sh");
}

char query[0x30] = { 0 };

int main() {
    char *perms;
    
    puts("Loading the browser...");
    puts("Welcome to Spirit, the world's most vulnerable web browser");

    //free()'ing the automatic variable query
    free(query); 
    perms = malloc(BUF_LEN);
    memset(perms, 0, BUF_LEN);
    while(1) {
        fgets(query, BUF_LEN, stdin);
        if(strncmp(query, "WEBSHELL", 8) == 0) {
            get_shell(perms);
        }
        if(strncmp(query, "EXIT", 4) == 0) {
            break;
        }
        if(strncmp(query, "SEARCH", 6) == 0) {
            puts("This service is unavailable due to maintenance");
        } 
    }
    free(perms);
    return 0;
}